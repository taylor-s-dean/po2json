# po2json

![C/C++ CI](https://github.com/limitz404/po2json/workflows/C/C++%20CI/badge.svg?branch=master)

A CLI tool/library to facilitate the conversion of gettext Portable Object (PO) files to JSON.

## Building

```sh
git clone https://github.com/taylor-s-dean/po2json.git
cd po2json
make
```

## Usage

```
po2json is a CLI tool that converts gettext Portable Object (.po) files to JSON format.
Usage: ./po2json [OPTIONS] input-file

Positionals:
  input-file TEXT REQUIRED    Path to a Portable Object (.po) file.

Options:
  -h,--help                   Print this help message and exit
  -o,--output-file TEXT       Path to a file to write the resulting JSON.
  -p,--print                  Print the resulting JSON object to stdout.
```

### Example

```sh
po2json ru.po --output-file ru.json
```

```sh
cat ru.po
```

```po
msgid ""
msgstr ""
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"
"Language: ru\n"
"Plural-Forms:  nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);\n"

msgid "%d user likes this."
msgid_plural "%d users like this."
msgstr[0] "one"
msgstr[1] "few"
msgstr[2] "many"
msgstr[3] "other"

msgctxt "This is some context "
"about the string."
msgid "Accept language "
"%{accept_language} was rejected"
msgstr "Принять "
"языки %{accept_language} были отклонены"

msgctxt "Button label"
msgid "Log in"
msgstr "Войти"

msgctxt "Dialog title"
msgid "Log in"
msgstr "Вход в систему"

msgid "One piggy went to the market."
msgstr "Одна свинья ушла на рынок."

msgctxt "Context with plural"
msgid "One piggy went to the market."
msgstr "Одна свинья ушла на рынок."

#, fuzzy
msgctxt ""
"Context with plural"
msgid ""
"One piggy went to the market."
msgid_plural ""
"One piggy went to the market."
msgstr[0] ""
"Одна свинья ушла на рынок."
msgstr[1] ""
"%d свиньи пошли на рынок."
msgstr[2] "На рынок вышли %d поросят."
msgstr[3] "%d поросят вышли на рынок."
```

```sh
cat ru.json
```

```json
{
    "": {
        "": {
            "MIME-Version": "1.0",
            "Content-Type": "text/plain; charset=UTF-8",
            "Content-Transfer-Encoding": "8bit",
            "Language": "ru",
            "Plural-Forms": "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);"
        },
        "%d user likes this.": {
            "plurals": [
                "one",
                "few",
                "many",
                "other"
            ]
        },
        "One piggy went to the market.": {
            "translation": "Одна свинья ушла на рынок."
        }
    },
    "This is some context about the string.": {
        "Accept language %{accept_language} was rejected": {
            "translation": "Принять языки %{accept_language} были отклонены"
        }
    },
    "Button label": {
        "Log in": {
            "translation": "Войти"
        }
    },
    "Dialog title": {
        "Log in": {
            "translation": "Вход в систему"
        }
    },
    "Context with plural": {
        "One piggy went to the market.": {
            "translation": "Одна свинья ушла на рынок.",
            "plurals": [
                "Одна свинья ушла на рынок.",
                "%d свиньи пошли на рынок.",
                "На рынок вышли %d поросят.",
                "%d поросят вышли на рынок."
            ]
        }
    }
}
```
