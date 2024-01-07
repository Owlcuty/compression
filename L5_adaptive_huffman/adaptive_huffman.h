#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

#include "prefix_code.h"


class AHuffman
{
public:
    AHuffman(const std::string& str);
    
    ~AHuffman();

    bool OK();

    std::string str_error();

    void encode(const std::string& filename = "");

    void write_encoded(std::string filename = "compressed");
    std::string get_encoded() { return compressed; }

    void decode();
    std::string get_decoded() { return decoded; }

private:
    enum class ERROR {OK, WRONG_DATA_TO_DECODE, CANT_OPEN_READ_FILE, FAILED_HANDLE_FILE};
    const std::string _STR_ERROR[4] = {
        "OK",
        "Wrong data. Impossible to decode",
        "Cannot open file to read",
        "Something wrong with file"
    };

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

    void write(const std::string& filename, const std::string& src, bool append = false);

private:
    std::string decoded;

    size_t num_bits;
    char* compressed = nullptr;
    int out_size;

    std::string _str;
    Prefix_Code _code;

    ERROR _err;
    std::string _str_error;
};