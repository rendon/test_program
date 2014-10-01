/**
 * @file
 * @author Rafael Rendon Pablo <rafaelrendonpablo@gmail.com>
 * @version 1.0
 *
 * @section Description
 *
 * Functions to work with strings.
 */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H
#include <cstring>
#include <string>
using namespace std;

/**
 * Perform some basic tests to determine if *str* is a valid representation of
 * an integer.
 * 
 * @param str The string to process.
 */
bool is_int(const char *str)
{
    if (!str || str[0] == '\0') {
        return false;
    }

    int i = 0, length = strlen(str);
    if (str[0] == '+' || str[0] == '-') {
        i++;
    }

    for ( ; i < length; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }

    return true;
}

/**
 * Returns the string representation of *n*.
 */
string to_s(int n)
{
    char buff[15];
    sprintf(buff, "%d", n);
    return string(buff);
}

/**
 * Determines if *prefix* is a prefix of *str*.
 */
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

/**
 * Determines if *suffix* is a suffix of *str*.
 */
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

string dir_name(string file_name)
{
    string dir;
    int length = file_name.length();
    while (length > 0 && file_name[--length] != '/');
    for (int i = 0; i < length; i++) {
        dir += file_name[i];
    }

    return dir;
}

string base_name(string file_name)
{
    string base;
    int length = file_name.length();
    while (length > 0 && file_name[length-1] != '/') {
        base += file_name[--length];
    }

    reverse(base.begin(), base.end());
    return base;
}

string to_lower(string str)
{
    for (int i = 0; i < int(str.length()); i++) {
        str[i] = tolower(str[i]);
    }

    return str;
}

#endif
