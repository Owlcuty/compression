#include <unordered_map>
#include <string>
#include <vector>

// TODO: write common BASE class compression
// TODO: make possible to expand : not fully from one string, but from packages
// Therefore have to protect with mutex

class LZW
{
public:
    LZW(std::string);
    ~LZW();

    void compress();
    void decompress();

    void write(std::string filename);

    void dump(bool readable = false);
private:
    void init_code();

private:
    std::unordered_map<std::string, int> code_;
    std::string str_;
    std::vector<int> output_;
};
