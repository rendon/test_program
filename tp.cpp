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


int main(int argc, char **argv)
{
    if (argc < 2) {
        cerr << "Usage: " <<  argv[0] 
             << " <add> |"
             << " <run [test_id]> |"
             << " <show [test_id]> |"
             << " <edit [test_id]> |"
             << " <clean>" << endl;
        exit(1);
    }

    if (strcmp(argv[1], "add") == 0) {
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

