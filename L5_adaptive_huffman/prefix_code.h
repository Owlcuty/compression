#include <iostream>

#include <string>
#include <unordered_map>
#include <set>
#include <vector>

// #define VERBOSE

struct Node;


class Prefix_Code
{
public:
    Prefix_Code();
    ~Prefix_Code();

    const std::string get_code(const char value);
    int get_symbol(const char* str, size_t& offset, int& bit, size_t num_bits, char& value);
    const std::string get_shift_code();
    void update_tree(const char symbol);

private:
    void init_tree();
    void build_bin_tree();
    void build_code(const Node* node, std::string codeword);

private:
    std::unordered_map<char, Node*> freq;
    std::vector<Node*> bind_nodes;
    std::unordered_map<char, std::string> _code;

    std::string _str;
    Node* _root;

    const char shift = 254;
};

