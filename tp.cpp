#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <string>
#include <cstring>
#include <cmath>
#include <cassert>
#include <algorithm>
using namespace std;

clock_t _start_clock = clock();

inline void _time()
{
#ifndef ONLINE_JUDGE
    fprintf(stderr, "Time: %.2fs\n",
            double(clock() - _start_clock) / CLOCKS_PER_SEC);
#endif
}

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

void do_test(int id, string exec_name)
{
    cout << "Test #" + to_s(id) + ": ";
    exec_name = "./" + exec_name;
    string tmp_out = "/tmp/out_" + to_s(id);
    string in = ".in_" + to_s(id) + ".txt";
    string command = exec_name + " < " + in + " > " + tmp_out;
    system(command.c_str());

    string out = ".out_" + to_s(id) + ".txt";
    string diff = "diff --ignore-all-space " + out + " " + tmp_out;
    int code = system(diff.c_str());
    if (code == 0)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;

    string rm = "rm " + tmp_out;
    system(rm.c_str());
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
             << " <add> | <del test_id> |"
             << " <run [test_id]> | <show [test_id]> |"
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
        printf("Type (NL + Ctrl + D) or (2 * Ctrl + D) to end input.\n");
        printf("\t== INPUT ==\n");
        string input;
        char ch;
        while ((ch = getchar()) != EOF)
            input += ch;

        string output;
        printf("\t== EXPECTED OUTPUT ==\n");
        while ((ch = getchar()) != EOF)
            output += ch;

        input_file << input;
        input_file.close();
        output_file << output;
        output_file.close();
    } else if (strcmp(argv[1], "del") == 0) {
        if (argc < 3) {
            cerr << "You must specify a test ID." << endl;
            exit(1);
        }

        if (!is_int(argv[2])) {
            cerr << "ID isn't valid." << endl;
            exit(1);
        }
        //int id = atoi(argv[2]);

        // del test with ID = id

    } else if (strcmp(argv[1], "run") == 0) {
        int test_id;
        string exec_name;
        ifstream info_file(".test_info");
        if (info_file.fail()) {
            cerr << "Couldn't open info file." << endl;
            exit(1);
        } else {
            info_file >> test_id;
            info_file >> exec_name;
            info_file.close();
        }
        if (argc == 3 && is_int(argv[2])) {
            int id = atoi(argv[2]);
            if (id < 1 || id > test_id) {
                cerr << "Invalid test ID!" << endl;
                exit(1);
            }

            do_test(id, exec_name);
        } else {
            for (int id = 1; id <= test_id; id++)
                do_test(id, exec_name);
        }
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

            command = "rm " + out_name;
            system(command.c_str());
        }

        info_file.close();
        system("rm .test_info");
    }

    //_time();
    return EXIT_SUCCESS;
}

