#include "adaptive_huffman.h"
#include <fstream>
#include <string.h>

AHuffman::AHuffman(const std::string& str) : _str(str)
{
    _err = ERROR::OK;
}

AHuffman::~AHuffman()
{
    free(compressed);
}


// error handling block
bool
AHuffman::OK()
{
    return _err == ERROR::OK;
}

std::string
AHuffman::str_error()
{
    if (_str_error.empty())
        _str_error = _STR_ERROR[0];
    return _str_error;
}

// Encoding
#define DEBUG_ENCODE

// TODO: decide how to separate common pieces of code (unbundle)
void
AHuffman::encode()
{
#ifdef DEBUG_ENCODE
    int cnt_ = 0;
    std::string check_str;
    std::string check_bin;
#endif
    std::string encoded;
    char byte = 0;
    int bit = 0;
    num_bits = 0;
    for (size_t i = 0; i < _str.length(); i ++)
    {
        if (bit == 8)
        {
            encoded += byte;
            bit = 0;
            byte = 0;
        }
        std::string code;
        code = _code.get_code(_str[i]);
        bool is_new = false;
        if (code.empty())
        {
            code = _code.get_shift_code();
            is_new = true;
        }
        for (int b = 0; b < (int)code.length(); b++)
        {
            if (code[b] == '1')
            {
                byte += 1 << (7 - bit);
#ifdef DEBUG_ENCODE
                check_str += '1';
                check_bin += '1';
#endif
            }
#ifdef DEBUG_ENCODE
            else
            {
                check_str += '0';
                check_bin += '0';
            }
#endif
            bit++;
            num_bits++;
            if (bit == 8)
            {
                encoded += byte;
                bit = 0;
                byte = 0;
            }
        }
        if (is_new)
        {
#ifdef DEBUG_ENCODE
            check_str += _str[i];
#endif
            {
                for (int b = 0; b < 8; b ++)
                {
                    if (((_str[i] >> (7 - b)) & 0x01))
                    {
                        byte += 1 << (7 - bit);
#ifdef DEBUG_ENCODE
                        check_bin += '1';
#endif
                    }
#ifdef DEBUG_ENCODE
                    else
                    {
                        check_bin += '0';
                    }
#endif
                    bit++;
                    num_bits++;
                    if (bit == 8)
                    {
                        encoded += byte;
                        bit = 0;
                        byte = 0;
                    }
                }
            }
        }
#ifdef DEBUG_ENCODE
        // check_str += '_';
        // check_bin += '_';
        cnt_ ++;
        if (170 < cnt_ && cnt_ < 190)
        {
            std::cout << "VERB::" << _str[i] << ":: " << std::string(check_bin.begin() + (check_bin.length() - 10), check_bin.end()) << std::endl;
            std::cout << "FULL::" << _str[i] << "::\n" << check_bin << std::endl;
        }
#endif
        _code.update_tree(_str[i]);

    }
    if (bit > 0)
    {
        encoded += byte;
        byte = 0;
        bit = 0;
    }

#ifdef DEBUG_ENCODE
    for (size_t i = 0; i < check_bin.length(); i++)
    {
        if (check_bin[i] == '_') cnt_++;
        if (170 < cnt_ && cnt_ < 190)
        {
            std::cout << check_bin[i];
        }
    }
    std::cout << std::endl;
    // std::cout << "Result: {" << std::string(check_str, 200, 600) << "}\n";
    // std::cout << "Bin re: {" << std::string(check_bin, 200, 600) << "}\n";
    std::cout << "Result: {" << check_str << "}\n";
    std::cout << "Bin re: {" << check_bin << "}\n";
#endif
    compressed = (char*)calloc(encoded.length() + sizeof(size_t), 1);
    memcpy(compressed, (char*)&num_bits, sizeof(size_t));
    memcpy(compressed + sizeof(size_t), encoded.data(), encoded.length());
}

void
AHuffman::write_encoded(std::string filename)
{
    std::ofstream out(filename);

    if (num_bits % 8 == 0)
        out.write(compressed, num_bits / 8 + sizeof(size_t));
    else
        out.write(compressed, num_bits / 8 + 1 + sizeof(size_t));
}

void
AHuffman::decode()
{
    std::cout << "Work strong!\n";
    memcpy((char*)&num_bits, _str.data(), sizeof(size_t));
    size_t offset = sizeof(size_t);

    size_t i = offset;
    int bit = 0;
    size_t all_bits = num_bits + offset * 8;
    while (i * 8 + bit < all_bits)
    {
        char tow;
        int ret = _code.get_symbol(_str.c_str(), i, bit, all_bits, tow);
        if (ret < 0)
        {
            set_error(ERROR::WRONG_DATA_TO_DECODE);
            return;
        }
        decoded += tow;
        std::cout << "Before update for " << tow << std::endl;
        std::cout << "Gessed encoded is:\n" << _code.get_checker() << std::endl;
        _code.update_tree(tow);
    }
    std::cout << "Guess encoded was::\n";
    std::cout << _code.get_checker() << std::endl;
}
