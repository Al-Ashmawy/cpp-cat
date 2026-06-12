#include <cstdlib>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string_view>
#include <vector>
#include <span>
#include <cstring>

using namespace std;
namespace fs = std::filesystem;

struct Options {
        vector<fs::path> files {};
        optional<fs::path> program_name {};
        bool number {false};
        bool number_nonblank {false};
        bool from_stdin {false};
        bool show_ends {false};
        bool show_tabs {false};
        bool squeeze_blank {false};
        bool show_nonprinting {false};
        bool end_option_list {false};
};

void print_version(const Options& args) {
        cout << args.program_name->string() << " 1.0" << endl;
        exit(EXIT_SUCCESS);
}

void print_help(const Options& args) {
        cout <<
                "Usage: " << args.program_name->string() << " [OPTION]... [FILE]...\n"
                "Concatenate FILE(s) to standard output.\n"
                "\n"
                "With no FILE, or when FILE is -, read standard input.\n"
                "-A, --show-all           equivalent to -v -E -T\n"
                "-b, --number-nonblank    number nonempty output lines, overrides -n\n"
                "-e                       equivalent to -v -E\n"
                "-E, --show-ends          display $ or ^M$ at end of each line\n"
                "-n, --number             number all output lines\n"
                "-s, --squeeze-blank      suppress repeated empty output lines\n"
                "-t                       equivalent to -v -T\n"
                "-T, --show-tabs          display TAB characters as ^I\n"
                "-u                       (ignored)\n"
                "-v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB\n"
                "--help\n"
                "        display this help and exit\n"
                "--version\n"
                "        output version information and exit\n"
                << endl;
        exit(EXIT_SUCCESS);
}

void parse_option(string_view option, Options& args) {

        if (args.end_option_list) {
                args.files.push_back(option);
        }
        else if (option == "--") {
                args.end_option_list = true;
        }
        else if (option == "--help") {
                print_help(args);
        }
        else if (option == "--version") {
                print_version(args);
        }
        else if (option == "-n" || option == "--number") {
                args.number = true;
        }
        else if (option == "-b" || option == "--number-nonblank") {
                args.number_nonblank = true;
        }
        else if (option == "-E" || option == "--show-ends") {
                args.show_ends = true;
        }
        else if (option == "-T" || option == "--show-tabs") {
                args.show_tabs = true;
        }
        else if (option == "-s" || option == "--squeeze-blank") {
                args.squeeze_blank = true;
        }
        else if (option == "-v" || option == "--show-nonprinting") {
                args.show_nonprinting = true;
        }
        else if (option == "-e") {
                args.show_nonprinting = true;
                args.show_ends = true;
        }
        else if (option == "-t") {
                args.show_nonprinting = true;
                args.show_tabs = true;
        }
        else if (option == "-A" || option == "--show-all") {
                args.show_nonprinting = true;
                args.show_ends = true;
                args.show_tabs = true;
        }
        else if (option == "-u") {
                // useless flag
                // for POSIX compatibility.
        }
        else if (option == "-") {
                args.from_stdin = true;
        }
        else {
                if (option.starts_with("--")) {
                        cerr << args.program_name->string() << ": "
                                << "unrecognized option "
                                << "'" << option << "'" << endl;

                        exit(EXIT_FAILURE);
                }
                else if (option.starts_with("-")) {
                        cerr << args.program_name->string() << ": "
                                << "invalid option -- "
                                << "'" << option[1] << "'" << endl;
                        exit(EXIT_FAILURE);
                }

                args.files.push_back(option);
        }
}

Options parse_arguments(span<char*> line) {
        Options args {};

        if (line.size() > 0) {
                args.program_name = line[0];
        }

        for (size_t i = 1; i < line.size(); i++) {
                string_view word = line[i];

                // if combined options
                if (word.starts_with("-") && !word.starts_with("--") && word.size() > 2) {
                        for (size_t i = 1; i < word.size(); i++) {
                                char c = word[i];

                                if (c == '-') {
                                        cerr << args.program_name->string()
                                                << ": " << "invalid option "
                                                << "-- '-'" << endl;
                                        exit(EXIT_FAILURE);
                                }

                                string option = string("-") + c;
                                parse_option(option, args);
                        }
                }
                else {
                        parse_option(word, args);
                }
        }

        if (args.number_nonblank) {
                args.number = false;
        }

        return args;
}

void print_stream(istream& stream, const Options& args) {
        static unsigned long long line_number = 1;

        string line;
        bool last_printed_is_blank = false;
        while (getline(stream, line)) {

                bool is_blank = line.size() == 0;
                if (args.squeeze_blank && is_blank && last_printed_is_blank) {
                        continue;
                }

                if (args.number || (args.number_nonblank && !is_blank) ) {
                        cout << "    " << line_number << "\t";
                        line_number++;
                }

                for (unsigned char c : line) {
                        if (c == '\r') {
                                if (args.show_ends) {
                                        cout << "^M";
                                }
                                continue;
                        }
                        if ((c == '\t') && args.show_tabs) {
                                cout << "^I";
                                continue;
                        }
                        if (args.show_nonprinting) {
                                if (c < 32) {
                                        char printable = c + 64;
                                        cout << "^" << printable;
                                        continue;
                                }
                                else if (c > 127) {
                                        char printable = c - 128 + 64;
                                        cout << "M-^" << printable;
                                        continue;
                                }
                        }

                        cout << c;
                }
                if (args.show_ends) {
                        cout << "$";
                }
                cout << endl;

                last_printed_is_blank = is_blank;
        }
}

void main_program(const Options& args) {
        if (!args.program_name.has_value()) {
                cerr << "Program must have a name!\n" << endl;
                exit(EXIT_FAILURE);
        }

        for (auto file_path : args.files) {

                ifstream file(file_path);
                int error = errno;
                if (!file.is_open()) {
                        cout << args.program_name.value().string() << ": ";
                        cout << file_path.string() << ": ";
                        cout << strerror(error) << endl;
                        continue;
                }

                print_stream(file, args);
        }

        if (args.from_stdin || args.files.size() == 0) {
                print_stream(cin, args);
        }
}

int main(int argc, char** argv) {
        Options args = parse_arguments(span(argv, argc));

        main_program(args);

        return 0;
}
