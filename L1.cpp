#include <iostream>
#include <set>
#include <map>
#include <string>

// #define TEST_MAIN

// L1.1
// Design and implement a program that displays a set of English letters in a given string
std::set<char> set_string_L1_1(std::string str)
{
    std::set<char> ret;
    for (int i = 0; i < str.length(); i++)
    {
        ret.insert(str[i]);
    }
    return ret;
}

// L1.2
// Write a method that takes a string as a parameter and returns a histogram of letters in the string.
// Thi i_th element of the histogram should contain the number of the i_th character in the string alphabet.
std::string histogram_L1_2(std::string str)
{
    std::string histogram;
    std::map<char, int> data;
    for (int i = 0; i < str.length(); i++)
    {
        data[str[i]] += 1;
    }

    for (auto it = data.begin(); it != data.end(); it++)
    {
        histogram += it->first;
        histogram += " ";
        for (int i = 0; i < it->second; i++) histogram += "x";
        histogram += "\n";
    }

    return histogram;
}

#if TEST_MAIN
int main()
{
    std::string str = "AAAABBCCSSSDDDD";
    std::string hist = histogram_L1_2(str);
    std::cout << hist << std::endl;

    return 0;
}
#endif
