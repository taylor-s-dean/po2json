/**
 * @file po2json.cpp
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
 * A CLI tool to facilitate the conversion of gettext Portable Object (PO)
 * files to JSON files.
 */
#include <string>

#include "rapidjson/document.h"
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include "po2json.hpp"

int
main(int argc, char** argv) {
    CLI::App app{
        "po2json is a CLI tool that converts gettext Portable Object (.po) "
        "files to JSON format."};

    std::string input_file;
    app.add_option(
           "input-file", input_file, "Path to a Portable Object (.po) file.")
        ->required(true);

    std::string output_file;
    app.add_option(
           "-o,--output-file",
           output_file,
           "Path to a file to write the resulting JSON.")
        ->required(false);

    bool should_print(false);
    app.add_flag(
           "-p,--print",
           should_print,
           "Print the resulting JSON object to stdout.")
        ->required(false);

    CLI11_PARSE(app, argc, argv);

    std::ifstream fin(input_file);
    std::string file_contents{
        std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>()};

    rapidjson::Document po_json;
    po2json::po2json(file_contents, po_json);

    if (should_print) {
        po2json::print_json(po_json);
    }

    if (!output_file.empty()) {
        po2json::write_json(po_json, output_file);
    }
}
