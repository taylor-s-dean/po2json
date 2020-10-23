/**
 * @file po2json.hpp
 * @author Taylor Dean <taylor@makeshift.dev>
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 * A set of tools to facilitate the conversion of gettext Portable Object (PO)
 * files to JSON files.
 */
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

namespace po2json {
/**
 * Converts the string representation of a .po file (Portable Object)
 * to a JSON document.
 *
 * @param po_json The resulting JSON representation.
 * @return A boolean indicates success or failure of the conversion.
 */
bool
po2json(const std::string& file_contents, rapidjson::Document& po_json) {
    po_json.SetObject();
    std::istringstream fin(file_contents);

    enum class State {
        unspecified,
        msgctxt,
        msgid,
        msgstr,
        msgid_plural,
        msgstr_plural,
    };

    struct Key {
        std::stringstream msgctxt;
        std::stringstream msgid;
        std::stringstream msgstr;
        std::stringstream msgid_plural;
        std::vector<std::stringstream> msgstr_plural;
    };

    std::map<State, std::string> state_strings;
    state_strings[State::unspecified] = "unspecified";
    state_strings[State::msgctxt] = "msgctxt";
    state_strings[State::msgid] = "msgid";
    state_strings[State::msgstr] = "msgstr";
    state_strings[State::msgid_plural] = "msgid_plural";
    state_strings[State::msgstr_plural] = "msgstr_plural";

    const std::regex comment_regex(R"(\s*#.*)");
    const std::regex empty_regex(R"(\s*)");
    const std::regex msgctxt_regex(R"(msgctxt\s+\"(.*)\")");
    const std::regex msgid_regex(R"(msgid\s+\"(.*)\")");
    const std::regex msgstr_regex(R"(msgstr\s+\"(.*)\")");
    const std::regex msgid_plural_regex(R"(msgid_plural\s+\"(.*)\")");
    const std::regex msgstr_plural_regex(R"(msgstr\[\d+\]\s+\"(.*)\")");
    const std::regex string_regex(R"(\"(.*)\")");
    const std::regex header_key_value(R"(([a-zA-Z0-9-]+): (.*?)\\n)");

    std::set<State> valid_next_states{State::msgctxt, State::msgid};

    auto print_states = [valid_next_states, state_strings]() -> std::string {
        std::stringstream ss;
        ss << '{';
        for (const State state : valid_next_states) {
            ss << state_strings.at(state) << ",";
        }
        ss << '}';
        return ss.str();
    };

#define expect_state(X)                                                        \
    if (valid_next_states.find(X) == valid_next_states.cend()) {               \
        std::cout << "Invalid .po file. Found " << state_strings[X]            \
                  << ", expected one of " << print_states() << std::endl;      \
    }

#define add_key_to_json()                                                      \
    std::string msgctxt{current_key->msgctxt.str()};                           \
    std::string msgid{current_key->msgid.str()};                               \
    std::string msgstr{current_key->msgstr.str()};                             \
    std::string msgid_plural{current_key->msgid_plural.str()};                 \
    std::vector<std::string> msgstr_plural;                                    \
    for (const std::stringstream& plural : current_key->msgstr_plural) {       \
        msgstr_plural.push_back(plural.str());                                 \
    }                                                                          \
                                                                               \
    if (!po_json.HasMember(msgctxt.c_str())) {                                 \
        po_json.AddMember(                                                     \
            rapidjson::Value(msgctxt.c_str(), po_json.GetAllocator()).Move(),  \
            rapidjson::Value().Move(),                                         \
            po_json.GetAllocator());                                           \
        po_json[msgctxt.c_str()].SetObject();                                  \
    }                                                                          \
    rapidjson::Value& msgctxt_obj{po_json[msgctxt.c_str()]};                   \
                                                                               \
    if (!msgctxt_obj.HasMember(msgid.c_str())) {                               \
        msgctxt_obj.AddMember(                                                 \
            rapidjson::Value(msgid.c_str(), po_json.GetAllocator()).Move(),    \
            rapidjson::Value().Move(),                                         \
            po_json.GetAllocator());                                           \
        msgctxt_obj[msgid.c_str()].SetObject();                                \
    }                                                                          \
    rapidjson::Value& msgid_obj{msgctxt_obj[msgid.c_str()]};                   \
                                                                               \
    if (msgstr.length() > 0) {                                                 \
        if (msgid.length() == 0) {                                             \
            std::smatch smatch;                                                \
            std::string s{msgstr};                                             \
            while (std::regex_search(s, smatch, header_key_value)) {           \
                std::string key{smatch.str(1)};                                \
                std::string value{smatch.str(2)};                              \
                s = smatch.suffix().str();                                     \
                if (!msgid_obj.HasMember(key.c_str())) {                       \
                    msgid_obj.AddMember(                                       \
                        rapidjson::Value(key.c_str(), po_json.GetAllocator())  \
                            .Move(),                                           \
                        rapidjson::Value().Move(),                             \
                        po_json.GetAllocator());                               \
                }                                                              \
                msgid_obj[key.c_str()].SetString(                              \
                    value.c_str(), po_json.GetAllocator());                    \
            }                                                                  \
        } else {                                                               \
            if (!msgid_obj.HasMember("translation")) {                         \
                msgid_obj.AddMember(                                           \
                    rapidjson::Value("translation", po_json.GetAllocator())    \
                        .Move(),                                               \
                    rapidjson::Value().Move(),                                 \
                    po_json.GetAllocator());                                   \
            }                                                                  \
            msgid_obj["translation"].SetString(                                \
                msgstr.c_str(), po_json.GetAllocator());                       \
        }                                                                      \
    }                                                                          \
    if (msgstr_plural.size() > 0) {                                            \
        if (!msgid_obj.HasMember("plurals")) {                                 \
            msgid_obj.AddMember(                                               \
                rapidjson::Value("plurals", po_json.GetAllocator()).Move(),    \
                rapidjson::Value().Move(),                                     \
                po_json.GetAllocator());                                       \
            msgid_obj["plurals"].SetArray();                                   \
        }                                                                      \
        for (const std::string& plural : msgstr_plural) {                      \
            msgid_obj["plurals"].PushBack(                                     \
                rapidjson::Value(plural.c_str(), po_json.GetAllocator())       \
                    .Move(),                                                   \
                po_json.GetAllocator());                                       \
        }                                                                      \
    }

    std::unique_ptr<Key> current_key{new Key};
    State current_state{State::unspecified};
    for (std::string line; std::getline(fin, line);) {
        // Ignore the line if it is a comment.
        // We expect the next line to be anything.
        if (std::regex_match(line, comment_regex)) {
            continue;
        }

        // If this is an empty line, then we expect the next
        // non-empty non-comment line to be msgctxt or msgid.
        if (std::regex_match(line, empty_regex)) {
            add_key_to_json();

            current_key.reset(new Key);
            current_state = State::unspecified;
            valid_next_states = {State::msgctxt, State::msgid};
            continue;
        }

        std::smatch submatch;

        // If this is a msgctxt line, then:
        // 1) msgctxt must be a valid state.
        // 2) We expect the next line to be either a string or a msgid.
        if (std::regex_match(line, submatch, msgctxt_regex)) {
            expect_state(State::msgctxt);
            current_state = State::msgctxt;
            valid_next_states = {State::msgid};
            current_key->msgctxt << submatch.str(1);
            continue;
        }

        // If this is a msgid line, then:
        // 1) msgid must be a valid state.
        // 2) We expect the next line to be either a string, msgstr, or
        // msgid_plural.
        if (std::regex_match(line, submatch, msgid_regex)) {
            expect_state(State::msgid);
            current_state = State::msgid;
            valid_next_states = {State::msgid_plural, State::msgstr};
            current_key->msgid << submatch.str(1);
            continue;
        }

        // If this is a msgstr line, then:
        // 1) msgstr must be a valid state.
        // 2) We expect the next line to be either a string or blank.
        if (std::regex_match(line, submatch, msgstr_regex)) {
            expect_state(State::msgstr);
            current_state = State::msgstr;
            current_key->msgstr << submatch.str(1);
            continue;
        }

        // If this is a msgid_plural line, then:
        // 1) msgid_plural must be a valid state.
        // 2) We expect the next line to be either a string or msgstr_plural.
        if (std::regex_match(line, submatch, msgid_plural_regex)) {
            expect_state(State::msgid_plural);
            current_state = State::msgid_plural;
            valid_next_states = {State::msgstr_plural};
            current_key->msgid_plural << submatch.str(1);
            continue;
        }

        // If this is a msgstr_plural line, then:
        // 1) msgstr_plural must be a valid state.
        // 2) We expect the next line to be either a string, msgstr_plural, or
        // blank.
        if (std::regex_match(line, submatch, msgstr_plural_regex)) {
            expect_state(State::msgstr_plural);
            current_state = State::msgstr_plural;
            valid_next_states = {State::msgstr_plural};
            current_key->msgstr_plural.emplace_back(submatch.str(1));
            continue;
        }

        // If this is a string continuation, then:
        // 1) Append the string to the existing string as determined by the
        // current_state.
        if (std::regex_match(line, submatch, string_regex)) {
            switch (current_state) {
            case State::msgctxt:
                current_key->msgctxt << submatch.str(1);
                break;
            case State::msgid:
                current_key->msgid << submatch.str(1);
                break;
            case State::msgstr:
                current_key->msgstr << submatch.str(1);
                break;
            case State::msgid_plural:
                current_key->msgid_plural << submatch.str(1);
                break;
            case State::msgstr_plural:
                current_key->msgstr_plural.back() << submatch.str(1);
                break;
            case State::unspecified:
                std::cout << "Encountered invalid state. Please ensure the "
                             "input file is in a valid .po format."
                          << std::endl;
                return false;
            }
            continue;
        }
    }

    add_key_to_json();

    return true;
}

/**
 * Pretty prints the given JSON document to stdout.
 *
 * @param document The JSON document to print.
 */
void
print_json(const rapidjson::Document& document) {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);
    std::cout << sb.GetString() << std::endl;
}

/**
 * Writes the given JSON document to a file.
 *
 * @param document The JSON document to write to a file.
 * @param file_name The name of the file to write to.
 */
void
write_json(const rapidjson::Document& document, const std::string& file_name) {
    FILE* fout = fopen(file_name.c_str(), "w");

    char buffer[65536];
    rapidjson::FileWriteStream os(fout, buffer, sizeof(buffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    document.Accept(writer);

    fclose(fout);
}
} // namespace po2json
