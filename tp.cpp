#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <string>
#include <cstring>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <regex>
#include <map>
#include <set>

#include <wordexp.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;
const char tp_version_string[] =
    "Test Program v0.1\n"
    "Copyright (C) 2014 Rafael Rendon Pablo\n"
    "License GPLv3+: GNU GPL version 3 or later"
    " <http://gnu.org/licenses/gpl.html>.\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.";


const char tp_usage_string[] =
    "usage: tp [--version] [--help] <command> [args]";
const char tp_help_string[] =
    "The available commands are:\n"
    "   init    Setups a contest environment.\n"
    "   test    Create, show, edit and delete test cases.\n"
    "   gen     Generate solution template.\n"
    "   clean   Remove any test related file.\n\n"
    "Use tp <command> --help to obtain more info about a specific command.";

const char tp_init_help_string[] =
    "tp init - Setups a programming contest environment in the current\n"
    "          working directory.\n\n"
    "usage: tp init <contest> [template]\n"
    "Where contest is a key of a supported programming contest, e.g. "
    "codeforces.\n"
    "With [template] you can also indicate which programming language you\n"
    "want for the solution templates passing the file extension of a\n"
    "supported language. See the following list:\n\n"
    "   cpp     =>  C++\n"
    "   java    =>  Java\n"
    "   rb      =>  Ruby\n"
    "   py      =>  Python";


        

const char tp_test_help_string[] =
    "tp test - Manage the test cases for a particular problem.\n\n"
    "Available actions:\n"
    "   add     Creates a new test case manually.\n"
    "   rm      Deletes a specific test case.\n"
    "   edit    Edit a specific test case.\n"
    "   show    Shows the contents of a specific test case.\n"
    "   run     Executes a particular test case. If no test case is specified\n"
    "           all test cases will be executed.\n"
    "Each test case is identified by an integer, all actions, with exception\n"
    "of add, need this identifier to proceed.";

const char tp_gen_help_string[] =
    "tp gen - Generates a solution template for a supported language.\n\n"
    "usage: tp gen <file.lang>\n\n"
    "Where lang corresponds to the programming language extension, these\n" 
    "are the extensions for the supported languages:\n\n"
    "   cpp     =>  C++\n"
    "   java    =>  Java\n"
    "   rb      =>  Ruby\n"
    "   py      =>  Python";

const char tp_clean_help_string[] =
    "tp clean - Remove all file generated by this tool.\n\n"
    "Available options:\n"
    "   -r  Find and deletes generated files recursively.";

const string COLOR_OFF  = "\e[0m";
const string GREEN      = "\e[0;32m";
const string RED        = "\e[0;31m";
const string YELLOW     = "\e[0;33m";
const string MAGENTA    = "\e[0;35m";

const string TEST_INFO_FILE = ".test_info";
const string RC_FILE    = "~/.tprc";
const set<string> SUPPORTED_LANGUAGES = {"cpp", "java", "python", "ruby"};

const string CONTEST_CODEFORCES = "codeforces";

const int ACCEPTED                  = 0;
const int GENERAL_ERROR             = 1;
const int WRONG_ANSWER              = 2;
const int RUNTIME_ERROR             = 3;
const int TIME_LIMIT_EXCEEDED       = 4;

const char BASIC_CPP_TEMPLATE[] =
"#include <bits/stdc++.h>\n"
"using namespace std;\n"
"int main(int argc, char **argv)\n"
"{\n"
"    return EXIT_SUCCESS;\n"
"}\n";

const char BASIC_JAVA_TEMPLATE[] =
"public class Main {\n"
"  public static void main(String[] args) {\n"
"  }\n"
"}\n";

const char BASIC_PYTHON_TEMPLATE[] =
"#!/usr/bin/python\n"
"print \"Let's do it!\"\n";

const char BASIC_RUBY_TEMPLATE[] =
"#!/usr/bin/ruby\n"
"puts \"Let's do it!\"\n";

/* ========== Utility functions ========== */

/* Basic test to check if the str represents an integer. */
bool is_int(const char *str)
{
    int length = strlen(str);
    if (length == 0) {
        return false;
    }

    int i = 0;
    if (str[0] == '+' || str[0] == '-') {
        i++;
    }

    while (i < length) {
        if (!isdigit(str[i])) {
            return false;
        }
        i++;
    }

    return true;
}

string to_s(int n)
{
    char buff[15];
    sprintf(buff, "%d", n);
    return string(buff);
}

bool starts_with(const string &str, const string &prefix)
{
    int n = str.length();
    int m = prefix.length();
    if (m > n) {
        return false;
    }

    while (m > 0) {
        if (str[m-1] != prefix[m-1]) {
            return false;
        }
        m--;
    }

    return true;
}

bool ends_with(const string &str, const string &suffix)
{
    int n = str.length();
    int m = suffix.length();
    if (m > n) {
        return false;
    }

    while (n > 0 && m > 0) {
        if (str[n-1] != suffix[m-1]) {
            return false;
        }
        n--;
        m--;
    }
    
    return true;
}

string get_language(string file_name)
{
    string lang;
    int length = file_name.length(); // Prevent files like ".ext"
    if (length > 4 && ends_with(file_name, ".cpp")) {
        lang = "cpp";
    } else if (length > 5 && ends_with(file_name, ".java")) {
        lang = "java";
    } else if (length > 3 && ends_with(file_name, ".rb")) {
        lang = "ruby";
    } else if (length > 3 && ends_with(file_name, ".py")) {
        lang = "python";
    }

    return lang;
}

void unknown_option(string str)
{
    cerr << "Unknown option: " << str << endl;
}

/* ============== The tests ============== */

bool do_test(int id, string src_file, string lang, string time_limit)
{
    string in = ".in_" + to_s(id) + ".txt";
    string out = "/tmp/out_" + to_s(id) + ".txt";
    string ans = ".out_" + to_s(id) + ".txt";
    string command = "tp-runner " + src_file + " " + in + " " + out + " "
                   + ans + " " + lang + " " + time_limit;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    int run_exit_code = system(command.c_str());
    gettimeofday(&end, NULL);

    // When the external program does not terminate normally the code passed to
    // exit() is stored in the last 8 bits. In case of success this operation
    // doesn't have effect.
    run_exit_code >>= 8;

    bool ret = true;
    long seconds = end.tv_sec - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    double t = (seconds * 1000 + useconds * 0.001) * 0.001;

    if (run_exit_code == RUNTIME_ERROR)  {
        cout << MAGENTA << fixed << setprecision(3) << "[" << t << "] "
            << "Test #" + to_s(id) + ": RTE" << COLOR_OFF << endl;
        ret = false;
    } else if (run_exit_code == TIME_LIMIT_EXCEEDED) {
        cout << YELLOW << fixed << setprecision(3) << "[" << t << "] "
            << "Test #" + to_s(id) + ": TLE" << COLOR_OFF << endl;
        ret = false;
    } else  if (run_exit_code == ACCEPTED) {
            cout << GREEN << fixed << setprecision(3) << "[" << t << "] "
                << "Test #" + to_s(id) + ": AC" << COLOR_OFF << endl;
    } else if (run_exit_code == WRONG_ANSWER) {
        cout << RED << fixed << setprecision(3) << "[" << t << "] "
            << "Test #" + to_s(id) + ": WA" << COLOR_OFF << endl;

        cout << "== EXPECTED OUTPUT == " << endl;
        string cat = "cat " + ans;
        system(cat.c_str());

        cout << endl << "== ACTUAL OUTPUT == " << endl;
        cat = "cat " + out;
        system(cat.c_str());
        cout << endl;
        ret = false;
    } else {
        cout << RED << "Error..." << COLOR_OFF << endl;
        ret = false;
    }

    command = "rm " + out;
    system(command.c_str());
    return ret;
}

int show_test(int id)
{
    cout << "Test #" + to_s(id) + ":" << endl;
    string in_name = ".in_" + to_s(id) + ".txt";
    string out_name = ".out_" + to_s(id) + ".txt";
    ifstream input(in_name.c_str());
    ifstream output(out_name.c_str());

    if (!input.is_open()) {
        cerr << "Couldn't open input file!" << endl;
        return 1;
    }

    if (!output.is_open()) {
        cerr << "Couldn't open output file!" << endl;
        return 1;
    }

    cout << "== INPUT ==" << endl;
    int i, max_lines = 20;
    string line;
    for (i = 0; getline(input, line) && i < max_lines; i++)
        cout << line << endl;
    if (i == max_lines)
        cout << ".\n.\n.\n";

    cout << endl << "== OUTPUT ==" << endl;
    for (i = 0; getline(output, line) && i < max_lines; i++)
        cout << line << endl;
    if (i == max_lines)
        cout << ".\n.\n.\n";

    cout << endl;
    input.close();
    output.close();

    return 0;
}

int read_config_file(string file_name, map<string, string> &config)
{
    wordexp_t exp_result;
    wordexp(file_name.c_str(), &exp_result, 0);
    ifstream config_file(exp_result.we_wordv[0]);
    if (config_file.fail()) {
        return 1;
    }

    string line;
    while (getline(config_file, line)) {
        string key = "";
        string value = "";
        int i = 0, end = line.length();

        // Trim
        while (end > 0 && line[end-1] == ' ') { end--; }
        while (i < end && line[i] == ' ')     { i++;   }

        while (i < end && line[i] != ' ' && line[i] != '=') {
            key += line[i++];
        }

        while (i < end && line[i] == ' ') { i++; }
        // Invalid key-value entry
        if (i >= end || line[i] != '=')   { continue; }

        i++; // skip '=' 
        while (i < end && line[i] == ' ') { i++; }

        while (i < end) {
            value += line[i++];
        }

        if (!key.empty() && !value.empty()) {
            config[key] = value;
        }
    }

    config_file.close();
    return 0;
}

int write_config_file(string file_name, map<string, string> &config)
{
    ofstream config_file(file_name);
    if (config_file.fail()) {
        cerr << "Couldn't open file: " + file_name << endl;
        return 1;
    }

    for  (auto c : config) {
        config_file << c.first << " = " << c.second << endl;
    }

    config_file.close();
    return 0;
}

int generate_template(string lang, string file_name)
{
    string key = "";
    if (lang == "cpp") {
        key = "cpp_template";
    } else if (lang == "java") {
        key = "java_template";
    } else if (lang == "python") {
        key = "python_template";
    } else if (lang == "ruby") {
        key = "ruby_template";
    }

    map<string, string> config;
    read_config_file(RC_FILE, config);

    // If the user has specified a custom template
    if (config.count(key) == 1) {
        string command = "cp " + config[key] + " " + file_name;
        int status = system(command.c_str()) >> 8;
        return status;
    } else {
        // Generate minimal template
        ofstream solution(file_name);
        if (solution.fail()) {
            cerr << "Couldn't open file " + file_name << endl;
            return 1;
        }

        if (lang == "cpp") {
            solution << BASIC_CPP_TEMPLATE;
        } else if (lang == "java") {
            solution << BASIC_JAVA_TEMPLATE;
        } else if (lang == "python") {
            solution << BASIC_PYTHON_TEMPLATE;
        } else if (lang == "ruby") {
            solution << BASIC_RUBY_TEMPLATE;
        }
        solution.close();
    }

    return 0;
}

/**
 * Generates Makefile for the corresponding language.
 * @param src_file Source code file name.
 * @param lang Programming language
 * @return Integer describing the status of the operation, success or failure.
 */
int generate_makefile(string src_file)
{
    string lang = get_language(src_file);
    if (lang.empty()) {
        return 1;
    }

    char buffer[256];
    string content;
    if (lang == "cpp") {
        regex r("\\.cpp$");
        string exec_file = regex_replace(src_file, r, "");
        const char* s = src_file.c_str();
        const char* e = exec_file.c_str();
        sprintf(buffer, "%s: %s\n\tg++ -o %s %s -Wall -std=c++11", e, s, e, s);
        content = buffer;
    } else if (lang == "java") {
        regex r("\\.java$");
        string class_file = regex_replace(src_file, r, "") + ".class";
        const char *s = src_file.c_str();
        const char *c = class_file.c_str();
        sprintf(buffer, "%s: %s\n\tjavac %s", c, s, s);
        content = buffer;
    }

    if (!content.empty()) {
        ofstream makefile("Makefile");
        if (makefile.is_open()) {
            makefile << content;
            makefile.close();
            return 0;
        } else {
            return 1;
        }
    }

    return 0;
}

int compile_solution(string lang)
{
    if (SUPPORTED_LANGUAGES.find(lang) == SUPPORTED_LANGUAGES.end()) {
        return 1;
    }

    if (lang == "cpp" || lang == "java") {
        return system("make -s") >> 8;
    } else {
        return 0;
    }
}

void help(string cmd)
{
    if (cmd == "tp") {
        cout << tp_help_string << endl;
    } else if (cmd == "init") {
        cout << tp_init_help_string << endl;
    } else if (cmd == "test") {
        cout << tp_test_help_string << endl;
    } else if (cmd == "gen") {
        cout << tp_gen_help_string << endl;
    } else if (cmd == "clean") {
        cout << tp_clean_help_string << endl;
    } else {
        cout << "Unknown command: " << cmd << endl;
    }
}

int init(int argc, char **argv)
{
    if (argc < 3) {
        cerr << "Please specify a contest name, e.g. codeforces" << endl;
        return 1;
    }

    string contest = string(argv[2]);
    string lang = "cpp"; // C++ by default

    if (argc > 3) {
        lang = string(argv[3]);
    }

    if (argc > 4) {
        unknown_option(argv[4]);
        help("init");
        return 1;
    }

    if (SUPPORTED_LANGUAGES.find(lang) == SUPPORTED_LANGUAGES.end()) {
        cerr << "Unsupported language: " << lang << endl;
        return 1;
    }

    if (contest == CONTEST_CODEFORCES) {
        int contest_id = -1;
        string str;
        cout << "Please indicate the Round ID: ";
        cin >> str;

        if (!is_int(str.c_str())) {
            cerr << "Invalid codeforces Round ID!" << endl;
            return 1;
        }

        contest_id = atoi(str.c_str());

        string problems[] = {"A", "B", "C", "D", "E"};
        for (string problem : problems) {
            system(("mkdir -p " + problem).c_str());
            string file_name = problem + "/";

            // Keep file name for Java, usually Main.java
            if (lang == "java") {
                file_name += "Main.java";
            } else {
                file_name += problem;
                if (lang == "cpp") {
                    file_name += ".cpp";
                } else if (lang == "ruby") {
                    file_name += ".rb";
                } else if (lang == "python") {
                    file_name += ".py";
                }
            }

            generate_template(lang, file_name);
        }

        // Download pretests
        if (contest_id != -1) {
            string id = to_s(contest_id);
            return system(("cf-parser " + id + " " + lang).c_str());
        }
    }

    return 0;
}

int test(int argc, char **argv)
{
    if (argc < 3) {
        cerr << "Please specify an action." << endl << endl;
        help("test");
        return 1;
    }

    if (argc > 4) {
        unknown_option(argv[4]);
        return 1;
    }

    string action = string(argv[2]);
    map<string, string> config;
    int status = read_config_file(TEST_INFO_FILE, config);
    if (status != 0 && action != "add") {
        cerr << "Couldn't open info file." << endl;
        return 1;
    }

    int total = 0;
    if (action == "add") {
        //region test add
        if (argc > 3) {
            unknown_option(argv[3]);
            return 1;
        }

        string src_file;
        string lang;
        string time_limit;
        if (status != 0) {
            total = 1;
            cout << "Source file: ";
            cin >> src_file;
            time_limit = "1s"; // One second by default
            lang = get_language(src_file);
            if (lang.empty()) {
                cerr << "Bad file name or unsupported language :(" << endl;
                return 1;
            }
            config["tests"] = to_s(1);
            config["src_file"] = src_file;
            config["time_limit"] = time_limit;
            config["lang"] = lang;
            generate_makefile(src_file);
        } else {
            total = atoi(config["tests"].c_str()) + 1;
            src_file = config["src_file"];
            lang = config["lang"];
            time_limit = config["time_limit"];

            config["tests"] = to_s(total);
        }

        status = write_config_file(TEST_INFO_FILE, config);
        if (status != 0) {
            cerr << "Couldn't write Test Info file." << endl;
            return 1;
        }

        string in_name = ".in_" + to_s(total) + ".txt";
        ofstream input_file(in_name.c_str(), ios::out);
        if (!input_file.is_open()) {
            cerr << "Couldn't open input file!" << endl;
            return 1;
        }

        string out_name = ".out_" + to_s(total) + ".txt";
        ofstream output_file(out_name.c_str(), ios::out);
        if (!output_file.is_open()) {
            cerr << "Couldn't open output file!" << endl;
            return 1;
        }

        input_file.close();
        output_file.close();
        string command = "vim -O " + in_name + " " + out_name;
        return system(command.c_str()) >> 8;
        //endregion
    } else if (action == "rm") {
        if (argc < 4) {
            cerr << "You must specify a test ID." << endl;
            return 1;
        }

        total = atoi(config["tests"].c_str());
        int id = atoi(argv[3]);
        if (!is_int(argv[3]) || id < 1 || id > total) {
            cerr << "Invalid test ID!" << endl;
            return 1;
        }

        status = 0;
        string command = "rm .in_" + to_s(id) + ".txt " +
                         ".out_" + to_s(id) + ".txt";
        status += system(command.c_str()) >> 8;
        for (int i = id + 1; i <= total; i++) {
            command = "mv .in_" + to_s(i) + ".txt " +
                      "   .in_" + to_s(i - 1) + ".txt";
            status += system(command.c_str()) >> 8;

            command = "mv .out_" + to_s(i) + ".txt " +
                      "   .out_" + to_s(i - 1) + ".txt";
            status += system(command.c_str()) >> 8;
        }

        total--;
        config["tests"] = to_s(total);
        status += write_config_file(TEST_INFO_FILE, config);
        if (status == 0) {
            cout << "Test " << id << " deleted." << endl;
        }
        return status;
    } else if (action == "edit") {
        //region test edit
        if (argc < 4) {
            cerr << "You must specify a test ID." << endl;
            return 1;
        }

        total = atoi(config["tests"].c_str());
        int id = atoi(argv[3]);
        if (!is_int(argv[3]) || id < 1 || id > total) {
            cerr << "Invalid test ID!" << endl;
            return 1;
        }

        string in_name = ".in_" + to_s(id) + ".txt";
        string out_name = ".out_" + to_s(id) + ".txt";
        string vim = "vim -O " + in_name + " " + out_name;
        status = system(vim.c_str()) >> 8;

        return status;
        //endregion
    } else if (action == "show") {
        //region test show
        total = atoi(config["tests"].c_str());

        status = 0;
        if (argc == 4 && is_int(argv[3])) {
            int id = atoi(argv[3]);
            if (id < 1 || id > total) {
                cerr << "Invalid test ID!" << endl;
                return 1;
            }
            status += show_test(id);
        } else {
            for (int id = 1; id <= total; id++) {
                status += show_test(id);
            }
        }
        return status;
        //endregion
    } else if (action == "run") {
        //region test run
        total = atoi(config["tests"].c_str());
        string src_file = config["src_file"];
        string lang = config["lang"];
        string time_limit = config["time_limit"];
        cout << "Compiling solution: ... ";
        cout.flush();

        if (compile_solution(lang) == 0) {
            cout << "OK" << endl << endl;
        } else {
            cerr << "FAIL" << endl << endl;
            return 1;
        }

        int total_tests = total;
        int success = 0;
        if (argc == 4 && is_int(argv[3])) {
            int id = atoi(argv[3]);
            if (id < 1 || id > total) {
                cerr << "Invalid test ID!" << endl;
                return 1;
            }

            total_tests = 1;
            if (do_test(id, src_file, lang, time_limit)) {
                success++;
            }
        } else {
            for (int id = 1; id <= total; id++) {
                if (do_test(id, src_file, lang, time_limit)) {
                    success++;
                }
            }
        }

        cout << "----------------------" << endl;
        cout << "Total tests: " << total_tests << endl
            << GREEN << "PASS: " << success;

        if (success < total_tests)
            cout << RED << endl << "FAIL: " << total_tests - success;

        cout << COLOR_OFF << endl;

        if (success == total_tests) {
            return 0;
        } else {
            return 1;
        }
        //endregion
    } else {
        unknown_option(action);
        return 1;
    }

    return 0;
}

int clean(int argc, char **argv)
{
    int status = 0;
    if (argc > 3) {
        unknown_option(argv[3]);
        return 1;
    }

    // clean recursively
    if (argc == 3 && strcmp(argv[2], "-r") == 0) {
        status = system("find  -regex '.*/\\.\\(in_\\|out_\\|test_info\\).*'\
                             -exec rm -v {} \\;") >> 8;
    } else {
        status = system("find -maxdepth 1\
                            -regex '.*/\\.\\(in_\\|out_\\|test_info\\).*'\
                            -exec rm -v {} \\;") >> 8;
    }
    return status;
}


int gen(int argc, char **argv)
{
    if (argc < 3) {
        cerr << "Please specify a file name for the template, e.g. A.cpp\n\n";
        return 1;
    }

    if (argc > 3) {
        unknown_option(argv[3]);
        return 1;
    }

    const char *file_name = argv[2];
    string lang = get_language(file_name);
    if (lang.empty()) {
        cerr << "Bad file name or unsupported language :(" << endl;
        return 1;
    }

    int length = strlen(file_name);
    while (length > 0 && file_name[--length] != '/');
    string dir;
    for (int i = 0; i < length; i++) {
        dir += file_name[i];
    }

    if (dir != "") {
        if (system(("mkdir -p " + dir).c_str())) {
            return 1;
        }
    }
    
    return generate_template(lang, file_name);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        cerr << tp_usage_string << endl;
        exit(EXIT_FAILURE);
    }

    const char *cmd = argv[1];

    // Process options
    if (starts_with(cmd, "--")) {
        if (strcmp(cmd, "--version") == 0) {
            cout << tp_version_string << endl;
            exit(EXIT_SUCCESS);
        } else if (strcmp(cmd, "--help") == 0) {
            cout << tp_usage_string << endl << endl;
            cout << tp_help_string << endl;
            exit(EXIT_SUCCESS);
        } else {
            unknown_option(cmd);
            cout << tp_help_string;
            exit(EXIT_FAILURE);
        }
    }

    int status = 0;
    // Process commands
    if (strcmp(cmd, "help") == 0) {
        if (argc < 3) {
            help("tp");
        } else {
            help(argv[2]);
        }
    } else if (strcmp(cmd, "init") == 0) {
        status = init(argc, argv);
    } else if (strcmp(cmd, "test") == 0) {
        status = test(argc, argv);
    } else if (strcmp(cmd, "gen") == 0) {
        status = gen(argc, argv);
    } else if (strcmp(cmd, "clean") == 0) {
        status = clean(argc, argv);
    } else {
        unknown_option(cmd);
        status = 1;
    }

    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

