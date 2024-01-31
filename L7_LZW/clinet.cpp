#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>

#include "lzw.h"

std::string read_file(std::string filename)
{
    std::string str;
    std::stringstream ss;
    std::ifstream bigFile(filename);
    ss << bigFile.rdbuf();
    str = ss.str();
    return str;
}


int main()
{
    std::string filename = "bigTest";
    std::string str = read_file("../tests/" + filename);
    LZW lz(str);
    lz.compress();
    // lz.dump(true);
    std::cout << str.length() << std::endl;

    lz.write("../tests/" + filename + ".lzw");

    return 0;
}