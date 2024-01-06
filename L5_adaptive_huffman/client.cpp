#include "adaptive_huffman.h"
#include <string>
#include <iostream>
#include <fstream>

std::string read_file(std::string filename)
{
    std::ifstream in(filename);

    std::string out;
    if (in.is_open())
    {
        std::string line;
        while (std::getline(in, line))
        {
            out += line;
        }
    }
    return out;
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
    // std::string text = read_file(filename);
    // if (text.empty())
    // {
    //     std::cout << "Text is empty\n";
    //     return -1;
    // }

    // AHuffman encoder(text);
    // encoder.encode();
    // encoder.write_encoded(out_compressed_filename);

    std::string compressed = read_file(out_compressed_filename);

    AHuffman decoder(compressed);
    decoder.decode();
    std::string result = decoder.get_decoded();
    std::cout << "Decode result: " << result << std::endl;

    return 0;
}