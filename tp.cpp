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
#include <map>

#include <wordexp.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;

inline int log(const char* format, ...)
{
#ifndef ONLINE_JUDGE
    va_list args;
    va_start(args, format);
    int code = vfprintf(stderr, format, args);
    va_end(args);
    return code;
#else
    return 0;
#endif
}

const double EPS      = 10e-8;
const int MAX         = 1000;
const int BUFFER_SIZE = 1024 * 1024;
const int INF         = 1 << 30;
const string COLOR_OFF  = "\e[0m";
const string GREEN      = "\e[0;32m";
const string RED        = "\e[0;31m";

const string BOLD_BLUE  = "\e[1;34m";
const string BOLD_WHITE = "\e[1;37m";

const string RC_FILE    = "~/.tprc";
const string SUPPORTED_LANGUAGES[] = {"cpp", "java", "python", "ruby"};

const char BASIC_CPP_TEMPLATE[] = "\
#include <bits/stdc++.h>\n\
using namespace std;\n\
int main(int argc, char **argv)\n\
{\n\
    return EXIT_SUCCESS;\n\
}\n";

const char BASIC_JAVA_TEMPLATE[] = "\
public class Main {\n\
  public static void main(String[] args) {\n\
  }\n\
}\n";

const char BASIC_PYTHON_TEMPLATE[] = "\
#!/usr/bin/python\n\
print \"Let's do it!\"\n";

const char BASIC_RUBY_TEMPLATE[] = "\
#!/usr/bin/ruby\n\
puts \"Let's do it!\"\n";

bool is_int(const char *str)
{
    int n;
    return sscanf(str, "%d", &n) != EOF;
}

string to_s(int n)
{
    char buff[15];
    sprintf(buff, "%d", n);
    return string(buff);
}

bool do_test(int id, string exec_name)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    exec_name = "./" + exec_name;
    string tmp_out = "/tmp/out_" + to_s(id);
    string in = ".in_" + to_s(id) + ".txt";
    string command = exec_name + " < " + in + " > " + tmp_out;
    system(command.c_str());

    string out = ".out_" + to_s(id) + ".txt";
    string diff = "diff --ignore-all-space "
                + out + " " + tmp_out + " 2>&1 > /dev/null";
    bool ret = true;
    int code = system(diff.c_str());

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    double t = (seconds * 1000 + useconds * 0.001) * 0.001;

    if (code == 0) {
        cout << GREEN << fixed << setprecision(3) << "[" << t << "] "
             << "Test #" + to_s(id) + ": PASS" << COLOR_OFF << endl;
    } else {
        cout << RED << fixed << setprecision(3) << "[" << t << "] "
             << "Test #" + to_s(id) + ": FAIL" << COLOR_OFF << endl;
        cout << "== EXPECTED OUTPUT == " << endl;
        string cat = "cat " + out;
        system(cat.c_str());

        cout << endl << "== ACTUAL OUTPUT == " << endl;
        cat = "cat " + tmp_out;
        system(cat.c_str());
        cout << endl;
        ret = false;
    }

    string rm = "rm " + tmp_out;
    system(rm.c_str());

    return ret;
}

void show_test(int id)
{
    cout << "Test #" + to_s(id) + ":" << endl;
    string in_name = ".in_" + to_s(id) + ".txt";
    string out_name = ".out_" + to_s(id) + ".txt";
    ifstream input(in_name.c_str());
    ifstream output(out_name.c_str());

    if (!input.is_open()) {
        cerr << "Couldn't open input file!" << endl;
        exit(1);
    }

    if (!output.is_open()) {
        cerr << "Couldn't open output file!" << endl;
        exit(1);
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
}

int read_rc_file(map<string, string> &config)
{
    wordexp_t exp_result;
    wordexp(RC_FILE.c_str(), &exp_result, 0);
    ifstream file(exp_result.we_wordv[0]);
    if (file.fail()) { return 1; }

    string line;
    while (getline(file, line)) {
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

        config[key] = value;
    }

    file.close();
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        cerr << "Usage: " <<  argv[0] 
             << " <init> <code_template> <contest_name> [CF Round ID]|"
             << " <add> |"
             << " <run [test_id]> |"
             << " <show [test_id]> |"
             << " <edit [test_id]> |"
             << " <clean>" << endl;
        exit(1);
    }

    if (strcmp(argv[1], "init") == 0) {
        if (argc < 3) {
            cerr << "Please specify a code template, e.g. cpp" << endl;
            exit(1);
        }
        if (argc < 4) {
            cerr << "Please specify a contest name, e.g. codeforces" << endl;
            exit(1);
        }

        map<string, string> config;
        read_rc_file(config);

        string code_template = string(argv[2]);
        bool valid = false;
        for (string ct : SUPPORTED_LANGUAGES) {
            if (code_template == ct) {
                valid = true;
            }
        }

        if (!valid) {
            cerr << "Unsupported language: " << code_template << endl;
            exit(1);
        }

        string key = "";
        string ext = "";
        if (code_template == "cpp") {
            key = "cpp_template";
            ext = "cpp";
        } else if (code_template == "java") {
            key = "java_template";
            ext = "java";
        } else if (code_template == "python") {
            key = "python_template";
            ext = "py";
        } else if (code_template == "ruby") {
            key = "ruby_template";
            ext = "rb";
        }

        if (strcmp(argv[3], "codeforces") == 0) {
            int contest_id = -1;
            if (argc == 5) {
                if (!is_int(argv[4])) {
                    cerr << "Invalid codeforces contest ID!" << endl;
                    exit(1);
                }
                contest_id = atoi(argv[4]);
            }
            string problems[] = {"A", "B", "C", "D", "E"};
            for (string problem : problems) {
                system(("mkdir -p " + problem).c_str());
                if (config.count(key) == 1) {
                    string command = "cp " + config[key];

                    // Keep file name for Java, usually Main.java
                    if (code_template == "java") {
                        command += " " + problem + "/";
                        system(command.c_str());
                    } else {
                        command += " " + problem + "/" + problem + "." + ext;
                        system(command.c_str());
                    }
                } else {
                    string file_name = problem + "/";
                    // Keep file name for Java
                    if (code_template == "java") {
                        file_name += "Main.java";
                    } else {
                        file_name += problem + "." + ext;
                    }
                    ofstream solution(file_name);
                    if (solution.fail()) {
                        cerr << "Couldn't open file " + file_name << endl;
                        exit(1);
                    }

                    if (code_template == "cpp") {
                        solution << BASIC_CPP_TEMPLATE;
                    } else if (code_template == "java") {
                        solution << BASIC_JAVA_TEMPLATE;
                    } else if (code_template == "python") {
                        solution << BASIC_PYTHON_TEMPLATE;
                    } else if (code_template == "ruby") {
                        solution << BASIC_RUBY_TEMPLATE;
                    }
                    solution.close();
                }

            }

            // Download pretests
            if (contest_id != -1) {
                string id = to_s(contest_id);
                system(("cf_parser " + id).c_str());
            }
        }
    } if (strcmp(argv[1], "add") == 0) {
        int test_id = 0;
        string exec_name;
        ifstream info_file(".test_info");
        if (info_file.fail()) {
            test_id = 1;
            cout << "Executable: ";
            cin >> exec_name;
            getchar(); // '\n'
        } else {
            info_file >> test_id;
            info_file >> exec_name;
            info_file.close();
            test_id++;
        }

        ofstream new_info_file(".test_info");
        new_info_file << test_id << " " << exec_name;
        new_info_file.close();

        string in_name = ".in_" + to_s(test_id) + ".txt";
        ofstream input_file(in_name.c_str(), ios::out);
        if (!input_file.is_open()) {
            cerr << "Couldn't open input file!" << endl;
            exit(1);
        }

        string out_name = ".out_" + to_s(test_id) + ".txt";
        ofstream output_file(out_name.c_str(), ios::out);
        if (!output_file.is_open()) {
            cerr << "Couldn't open output file!" << endl;
            exit(1);
        }

        input_file.close();
        output_file.close();
        string vim = "vim -O " + in_name + " " + out_name;
        int code = system(vim.c_str());
        exit(code);
    } else if (strcmp(argv[1], "edit") == 0) {
        if (argc < 3) {
            cerr << "You must specify a test ID." << endl;
            exit(1);
        }
        if (!is_int(argv[2])) {
            cerr << "Invalid test ID!" << endl;
            exit(1);
        }

        int test_id;
        ifstream info_file(".test_info");
        if (info_file.fail()) {
            cerr << "Couldn't open info file." << endl;
            exit(1);
        }
        info_file >> test_id;
        info_file.close();

        int id = atoi(argv[2]);
        if (id < 1 || id > test_id) {
            cerr << "Invalid test ID!" << endl;
            exit(1);
        }

        string in_name = ".in_" + to_s(id) + ".txt";
        string out_name = ".out_" + to_s(id) + ".txt";
        string vim = "vim -O " + in_name + " " + out_name;
        int code = system(vim.c_str());
        exit(code);
    } else if (strcmp(argv[1], "run") == 0) {
        int test_id;
        string exec_name;
        ifstream info_file(".test_info");
        if (info_file.fail()) {
            cerr << "Couldn't open info file." << endl;
            exit(1);
        } 
        info_file >> test_id;
        info_file >> exec_name;
        info_file.close();

        int total_tests = test_id;
        int success = 0;
        if (argc == 3 && is_int(argv[2])) {
            int id = atoi(argv[2]);
            if (id < 1 || id > test_id) {
                cerr << "Invalid test ID!" << endl;
                exit(1);
            }

            total_tests = 1;
            if (do_test(id, exec_name))
                success++;
        } else {
            for (int id = 1; id <= test_id; id++)
                if (do_test(id, exec_name))
                    success++;
        }

        cout << "----------------------" << endl;
        cout << "Total tests: " << total_tests << endl
             << GREEN << "PASS: " << success;

        if (success < total_tests)
            cout << RED << endl << "FAIL: " << total_tests - success;

        cout << COLOR_OFF << endl;
    } else if (strcmp(argv[1], "show") == 0) {
        int test_id;
        string exec_name;
        ifstream info_file(".test_info");
        if (info_file.fail()) {
            cerr << "Couldn't open info file." << endl;
            exit(1);
        }
        
        info_file >> test_id;

        if (argc == 3 && is_int(argv[2])) {
            int id = atoi(argv[2]);
            if (id < 1 || id > test_id) {
                cerr << "Invalid test ID!" << endl;
                exit(1);
            }
            show_test(id);
        } else {
            for (int id = 1; id <= test_id; id++)
                show_test(id);
        }
    } else if(strcmp(argv[1], "clean") == 0) {
        // clean recursively
        if (argc > 2 && strcmp(argv[2], "-r") == 0) {
            system("find  -regex '.*/\\.\\(in_\\|out_\\|test_info\\).*'\
                    -exec rm -v {} \\;");
        } else {
            int test_id;
            string exec_name;
            ifstream info_file(".test_info");
            if (info_file.fail()) {
                cerr << "Couldn't open info file." << endl;
                exit(1);
            }

            info_file >> test_id;
            for (int id = 1; id <= test_id; id++) {
                string in_name = ".in_" + to_s(id) + ".txt";
                string out_name = ".out_" + to_s(id) + ".txt";
                string command = "rm " + in_name;
                system(command.c_str());

                command = "rm " + out_name + " &> /dev/null";
                system(command.c_str());
            }

            info_file.close();
            system("rm .test_info &> /dev/null");
        }
    }

    return EXIT_SUCCESS;
}

