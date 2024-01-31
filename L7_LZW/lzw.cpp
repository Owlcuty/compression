#include <iostream>
#include <cctype>
#include <fstream>
#include <chrono>

#define PROFILE_FUNCS

#include "lzw.h"

LZW::LZW(std::string str) : str_(str)
{
    init_code();
}

LZW::~LZW()
{

}


void
LZW::init_code()
{
    for (unsigned char i = 0; i < 255; i++)
    {
        code_[std::string(1, i)] = (int)i + 1;
    }
}

void
LZW::compress()
{
    const std::string pr = "Progres: ";
    std::string word;
    int tenth = 0.1l * str_.length();
#ifdef PROFILE_FUNCS
    using milli = std::chrono::milliseconds;
    auto start = std::chrono::high_resolution_clock::now();
#endif
    for (size_t i = 0; i < str_.length(); i++)
    {
        std::string tmp_word = word + str_[i];
        if (code_.find(tmp_word) != code_.end())
        {
            word = tmp_word;
        }
        else
        {
            output_.push_back(code_[word]);
            code_[tmp_word] = code_.size();
            word = std::string(1, str_[i]);
        }
        if (i % tenth == 0)
        {
            std::cout << pr << i / tenth * 10 << std::endl;
#ifdef PROFILE_FUNCS
        auto finish = std::chrono::high_resolution_clock::now();
        std::cout << "Progress of " << i / tenth * 10 << "% took "
              << std::chrono::duration_cast<milli>(finish - start).count()
              << " milliseconds\n";
#endif
        }
    }
    output_.push_back(code_[word]);
}

void
LZW::dump(bool readable)
{
    std::cout << "Code:\n";
    for (auto it = code_.begin(); it != code_.end(); it++)
    {
        if (readable)
        {
            u_char ch = it->first[0];
            if (!(std::isdigit(ch) || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || std::string("\"\';:<>,./\\!@#$%^&*(){}[]|").find(ch) != std::string::npos))
            {
                continue;
            }
        }
        std::cout << "{" << it->first << "}:: " << it->second << std::endl;
    }
    std::cout << "Compressed with " << output_.size() << " codewords:\n";
    for (size_t i = 0; i < output_.size(); i++)
    {
        std::cout << output_[i] << " ";
    }
    std::cout << std::endl;
}

void
LZW::write(std::string filename)
{
    std::ofstream of(filename, std::ios::out | std::ios::binary);
    of.write((char*)&output_[0], output_.size() * sizeof(int));

    of.close();
}
