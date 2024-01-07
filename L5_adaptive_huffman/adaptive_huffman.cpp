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

// TODO: decide how to separate common pieces of code (unbundle)
void
AHuffman::encode(const std::string& filename)
{
    const size_t block_size = 1024 * 16;
    std::string encoded;
    char byte = 0;
    int bit = 0;
    num_bits = 0;
    bool to_write = false;
    if (!filename.empty())
        to_write = true;
    bool append = false;
    size_t writed = 0;
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
                byte += 1 << (7 - bit);
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
            for (int b = 0; b < 8; b ++)
            {
                if (((_str[i] >> (7 - b)) & 0x01))
                {
                    byte += 1 << (7 - bit);
                }
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
        _code.update_tree(_str[i]);

        if (to_write && !encoded.empty() && encoded.length() % block_size == 0 && encoded.length() != writed)
        {
            std::string pack(encoded, writed, block_size);
            write(filename, pack, append);
            append = true;
            writed += block_size;
        }
    }
    if (bit > 0)
    {
        encoded += byte;
        byte = 0;
        bit = 0;
    }

    compressed = (char*)calloc(encoded.length() + sizeof(size_t), 1);
    memcpy(compressed, (char*)&num_bits, sizeof(size_t));
    memcpy(compressed + sizeof(size_t), encoded.data(), encoded.length());

    if (to_write && encoded.length() < block_size)
    {
        write_encoded(filename);
    }
    else if (to_write)
    {
        std::string pack(encoded, writed, encoded.length() - writed);
        write(filename, pack, append);
        writed += encoded.length() - writed;
    }
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
AHuffman::write(const std::string& filename, const std::string& src, bool append)
{
    std::ofstream out;
    if (append)
    {
        out.open(filename, std::ios_base::app);
    }
    else
    {
        out.open(filename);
    }
    out << src;
}

void
AHuffman::decode()
{
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
        _code.update_tree(tow);
    }
}
