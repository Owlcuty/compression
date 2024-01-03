#include <iostream>
#include <string>
#include <cassert>
#include <fstream>

// #define TEST_MAIN

// L3.1
// Based on the outline of the simple HDC algorithm, derive your version of
// the HDC algorithm in pseudocode which allows an easy implementation
// in your favourite program language.
// L3.2
// Implement your version of HDC algorithm. Use MyHDC as the name of your main class/programm

// Seq of symbols contains ASCII 128
class MyHDC
{
public:
    MyHDC(const std::string& str) : _str(str)
    {
        num_symbols = _str.length();
        _err = ERROR::OK;
    }
    ~MyHDC() {}

    bool OK()
    {
        return _err == ERROR::OK;
    }
    std::string str_error()
    {
        return _str_error;
    }


    void encode()
    {
        if (num_symbols < 3)
        {
            encoded = out_nonrun(num_symbols, _str);
            return;
        }
        int index = 0;

        while (index + 2 < num_symbols)
        {
            if (begin_run(index))
            {
                size_t len = find_longest_run(index);
                encoded += out_run(len, _str[index]);
                index += len;
            }
            else
            {
                size_t len = find_longest_nonrun(index);
                encoded += out_nonrun(len, std::string(_str, index, len));
                index += len;
            }
        }
    }

    void decode()
    {
        decoded = "";
        for (int i = 0; i < num_symbols; i++)
        {
            int len = 0;
            if ((len = get_len_sym_nonrun(_str[i])) != -1)
            {
                decoded += std::string(_str, i + 1, len);
                i += len;
            }
            else if ((len = get_len_sym_run(_str[i])) != -1)
            {
                if (i == num_symbols - 1 || is_run(_str[i + 1]) || is_nonrun(_str[i + 1]))
                {
                    decoded += std::string(len, ' ');
                }
                else
                {
                    decoded += std::string(len, _str[i + 1]);
                    i++;
                }
            }
            else
            {
                set_error(ERROR::WRONG_DATA_TO_DECODE);
            }
        }
    }

    std::string get_encoded()
    {
        return encoded;
    }
    std::string get_decoded()
    {
        return decoded;
    }

private:
    enum class ERROR {OK, WRONG_DATA_TO_DECODE};
    const std::string _STR_ERROR[2] = {
        "OK",
        "Wrong data. Impossible to decode"
    };

private:
    void _set_error(int err)
    {
        _str_error = _STR_ERROR[err];
    }
    void set_error(ERROR err)
    {
        _err = err;
        int error = (int)err;
        _set_error(error);
    }

    std::string out_nonrun(const int len, const std::string& str)
    {
        assert(len <= 64);

        std::string ret;
        char control_symbol = control_symbol_nonrun + len - 1;
        ret += control_symbol;
        ret += str;

        return ret;
    }

    std::string out_run(const int len, char symbol)
    {
        assert(len < 64);


        std::string ret;
        char control_symbol = control_symbol_run + len - 2;
        ret += control_symbol;
        if (symbol != ' ')
            ret += symbol;

        return ret;
    }

    bool begin_run(const size_t ind)
    {
        return (_str[ind] == ' ' && _str[ind + 1] == ' ') || (_str[ind] == _str[ind + 1] && _str[ind] == _str[ind + 2]);
    }

    int find_longest_run(const size_t index)
    {
        int len = 1;
        for (int i = index + 1; i < _str.length(); i++)
        {
            if (_str[i] == _str[i - 1])
                len++;
            else
                break;
            
            if (len == 63)
                break;
        }

        return len;
    }
    int find_longest_nonrun(const size_t index)
    {
        int len = 2;
        int i = 0;
        for (i = index + 2; i < num_symbols; i++)
        {
            if (_str[i - 1] != _str[i - 2])
                len++;
            else if (_str[i] != ' ' && _str[i] != _str[i - 1])
                len++;
            else
                break;
            if (len == 64)
                break;
        }
        if (i != num_symbols && len != 2)
        {
            len -= 2;
        }

        return len;
    }

    bool is_nonrun(char control_symbol)
    {
        return control_symbol_nonrun <= control_symbol && control_symbol < control_symbol_nonrun + 64;
    }
    int get_len_sym_nonrun(char control_symbol)
    {
        if (is_nonrun(control_symbol))
            return (int)(control_symbol - control_symbol_nonrun) + 1;
        else
            return -1;
    }

    bool is_run(char control_symbol)
    {
        return control_symbol_run <= control_symbol && control_symbol < control_symbol_run + 63;
    }
    int get_len_sym_run(char control_symbol)
    {
        if (is_run(control_symbol))
            return (int)(control_symbol - control_symbol_run) + 2;
        else
            return -1;
    }


private:

    std::string _str;
    size_t num_symbols;
// s2, .., s63
    const char control_symbol_run = 128;
// n1, .., n64
    const char control_symbol_nonrun = 191;

    std::string encoded;
    std::string decoded;

    ERROR _err;
    std::string _str_error;
};






#ifdef TEST_MAIN
int main()
{
    std::ifstream good_case("testGOOD");
    std::string good_to_enc;
    if (good_case.is_open())
    {
        std::string line;
        while (std::getline(good_case, line))
        {
            good_to_enc += line;
        }
    }
    else
    {
        std::cerr << "Bad ifstream\n";
        return -1;
    }

    std::ifstream bad_case("testBAD");
    std::string bad_to_enc;
    if (bad_case.is_open())
    {
        std::string line;
        while (std::getline(bad_case, line))
        {
            bad_to_enc += line;
        }
    }
    else
    {
        std::cerr << "Bad ifstream\n";
        return -1;
    }


    MyHDC encoder(good_to_enc);
    encoder.encode();

    std::string compressed = encoder.get_encoded();
    std::cout << "Size of compressed data = " << compressed.length() << std::endl;
    // std::cout << "Compressed data: {" << compressed << "}\n";

    MyHDC decoder(compressed);
    decoder.decode();

    if (decoder.OK())
    {
        std::string decompressed = decoder.get_decoded();
        // std::cout << "Decompressed data: {" << decompressed << "}\n";
        if (decompressed.compare(good_to_enc) == 0)
        {
            std::cout << "Lossless compression was successful\n";
        }
        else
        {
            std::cout << "Failure. Decompressed data doesn't equal source\n";// {" << good_to_enc << "}\n";
        }
    }
    else
    {
        std::cout << "Decoder: " << decoder.str_error() << std::endl;
    }







    MyHDC encoder_bad(bad_to_enc);
    encoder_bad.encode();

    compressed = encoder_bad.get_encoded();
    std::cout << "Size of compressed data = " << compressed.length() << std::endl;
    // std::cout << "Compressed data: {" << compressed << "}\n";

    MyHDC decoder_bad(compressed);
    decoder_bad.decode();

    if (decoder_bad.OK())
    {
        std::string decompressed = decoder_bad.get_decoded();
        // std::cout << "Decompressed data: {" << decompressed << "}\n";
        if (decompressed.compare(bad_to_enc) == 0)
        {
            std::cout << "Lossless compression was successful\n";
        }
        else
        {
            std::cout << "Failure. Decompressed data doesn't equal source\n"; // {" << bad_to_enc << "}\n";
        }
    }
    else
    {
        std::cout << "Decoder: " << decoder_bad.str_error() << std::endl;
    }

    return 0;
}
#endif