#include "adaptive_huffman.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

std::string read_file(const std::string& filename)
{
    std::ifstream in(filename);

    std::stringstream ss;
    ss << in.rdbuf();
    std::string out = ss.str();
    return out;
}

int write_file(const std::string& filename, const std::string& data)
{
    std::ofstream out(filename);
    if (!out.is_open())
        return -1;
    out << data;
    if (!out)
        return -1;

    return 0;
}

int main(int argc, char** argv)
{
    std::string filename = "default.test";
    std::string out_compressed_filename = "default.ahf";
    if (argc > 1)
    {
        filename = argv[1];
    }
    if (argc > 2)
    {
        out_compressed_filename = argv[2];
    }
    std::string text = read_file(filename);
    if (text.empty())
    {
        std::cout << "Text is empty\n";
        return -1;
    }

    AHuffman encoder(text);
    encoder.encode();
    encoder.write_encoded(out_compressed_filename);

    std::string compressed = read_file(out_compressed_filename);

    AHuffman decoder(compressed);
    decoder.decode();
    if (!decoder.OK())
    {
        std::cout << "Failed: " << decoder.str_error() << std::endl;
        return -1;
    }
    std::string result = decoder.get_decoded();
    bool equal = (result.compare(text) == 0);
    std::cout << (equal ? "Success" : "Failed") << std::endl;
    if (!equal)
    {
        for (size_t i = 0; i < std::min(result.length(), text.length()); i++)
        {
            if (result[i] != text[i])
            {
                std::cout << std::string(result, 0, i + 10) << std::endl;
                std::cout << std::string(text, 0, i + 10) << std::endl;                
                std::cout << std::endl << i << std::endl;
                break;
            }
        }
    }
    std::string out_decompressed_filename = "default.decoded";
    write_file(out_decompressed_filename, result);
    return 0;
}