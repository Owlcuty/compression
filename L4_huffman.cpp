#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <fstream>
#include <memory>
#include <string.h>
#include <vector>
#include <chrono>
#include <cmath>

#define TEST_MAIN
#define PROFILE_FUNCS
// #define MEASURE


struct Node
{
    friend bool operator<(const Node& node_l, const Node& node_r)
    {
        return node_l.probability < node_r.probability;
    }
    Node& operator++()
    {
        probability++;
        return *this;
    }
    Node operator++(int)
    {
        Node old = *this;
        operator++();
        return old;
    }
    Node& operator+=(const Node& node)
    {
        probability += node.probability;
        return *this;
    }
    bool leaf = false;
    std::string value;
    int probability;
    struct Node* left = nullptr;
    struct Node* right = nullptr;
};

struct comp {
    template <typename T>
 
    // Comparator function
    bool operator()(const T& l, const T& r) const
    {
        return *l < *r;
    }
};

class Prefix_Code
{
public:
    Prefix_Code(const std::string& str) : _str(str)
    {
#ifdef PROFILE_FUNCS
        using milli = std::chrono::milliseconds;
        auto start = std::chrono::high_resolution_clock::now();
#endif
        calc_frequency();
#ifdef PROFILE_FUNCS
        auto finish = std::chrono::high_resolution_clock::now();
        std::cout << "calc_frequency() took "
              << std::chrono::duration_cast<milli>(finish - start).count()
              << " milliseconds\n";
#endif

        build_bin_tree();
        build_code(_root, "");
        std::cout << "Build code ended\n";
#ifdef MEASURE
        double code_efficiency = calc_code_efficiency();
        std::cout << "Code efficiency = " << code_efficiency << std::endl;
#endif

        // std::cout << "Code:\n";
        // for (auto it = _code.begin(); it != _code.end(); it++)
        // {
        //     std::cout << it->first << ":: " << it->second << std::endl;
        // }
        // std::cout << std::endl;
    }
    ~Prefix_Code()
    {
        delete_tree(_root);
    }

    std::string get_code(const std::string& value)
    {
        return _code[value];
    }
    int get_symbol(const char* str, size_t& offset, int& bit, size_t num_bits, std::string& value)
    {
        Node* tmp = _root;
        if (tmp->leaf)
        {
            value = tmp->value;
            bit++;
            if (bit == 8)
            {
                offset++;
                bit = 0;
            }
        }
        while (offset * 8 + bit < num_bits && tmp)
        {
            if (bit == 8)
            {
                offset++;
                bit = 0;
            }
            if (tmp->leaf)
            {
                value = tmp->value;
                return 0;
            }
            if (!((str[offset] >> (7 - bit)) & 0x01))
            {
                tmp = tmp->left;
                bit++;

            }
            else if ((str[offset] >> (7 - bit)) & 0x01)
            {
                tmp = tmp->right;
                bit++;
            }
            else
            {
                return -1;
            }
        }
        if (tmp && tmp->leaf)
        {
            value = tmp->value;
            return 0;
        }
        return -1;
    }
private:
    void delete_tree(Node* node)
    {
        if (node == nullptr) return;

        delete_tree(node->left);
        delete_tree(node->right);
        delete node;
    }

    void calc_frequency()
    {
        // for (int chr = 0; chr < 255; chr++)
        // {
        //     Node* node = new Node();
        //     node->leaf = true;
        //     node->value = chr;
        //     freq[(char)chr] = node;
            
        // }
        for (int i = 0; i < _str.length(); i+=2)
        {
// faster :c
            std::string tmp;
            tmp += _str[i];
            if (i < _str.length() - 1)
                tmp += _str[i + 1];
            if (freq.find(tmp) == freq.end())
            {
                Node* node = new Node();
                node->leaf = true;
                node->value = tmp;
                freq[tmp] = node;
            }
            (*freq[tmp]) ++;
        }
    }

#ifdef MEASURE
    double entropy_L2_1(const std::vector<double>& probabilities)
    {
        double entropy = 0;
        for (int i = 0; i < probabilities.size(); i++)
        {
            entropy -= probabilities[i] * log2(probabilities[i]);
        }

        return entropy;
    }
    double averageLength_L2_2(const std::vector<int>& length, const std::vector<double>& probabilities)
    {
        double avlen = 0;
        for (int i = 0; i < length.size(); i++)
        {
            avlen += probabilities[i] * length[i];
        }

        return avlen;
    }

    double calc_code_efficiency()
    {
        std::vector<double> probabilities;
        std::vector<int> len;
        for (auto it = freq.begin(); it != freq.end(); it++)
        {
            // with "* 2" approximately for now
            probabilities.push_back((it->second->probability * 1.0) / _str.length() * 2);
            len.push_back(_code[it->first].length());
        }
        double entropy = entropy_L2_1(probabilities);
        std::cout << "Entropy = " << entropy << std::endl;
        double avlen = averageLength_L2_2(len, probabilities);
        std::cout << "AverageLen = " << avlen << std::endl;

        double code_efficiency = 0;
        code_efficiency = entropy / avlen * 100;

        return code_efficiency;
    }
#endif

    void build_bin_tree()
    {
        std::multiset<Node*, comp> alphabet;
        for (auto it = freq.begin(); it != freq.end(); it++)
        {
            alphabet.insert(it->second);
        }

        while (alphabet.size() > 1)
        {
            auto last = alphabet.begin();
            auto prelast = last;
            prelast++;

            Node* bind_node = new Node();
            bind_node->probability += (*last)->probability;
            bind_node->probability += (*prelast)->probability;
            bind_node->left = *prelast;
            bind_node->right = *last;
            alphabet.erase(last);
            alphabet.erase(prelast);
            alphabet.insert(bind_node);
        }
        _root = *(alphabet.begin());
    }

    void build_code(const Node* node, std::string codeword)
    {
        if (!node) return;
        if (node->leaf)
        {
            if (codeword.empty()) codeword = "0";
            _code[node->value] = codeword;
        }
        else
        {
            build_code(node->left, codeword + '0');
            build_code(node->right, codeword + '1');
        }
    }

private:
    std::unordered_map<std::string, Node*> freq;
    std::unordered_map<std::string, std::string> _code;

    std::string _str;
    Node* _root;
};



class Huffman
{
public:
    Huffman(const std::string& str, std::shared_ptr<Prefix_Code> const& code, bool decompress = false) : _code(code)
    {
        if (decompress)
        {
            read_compressed(str);
        }
        else
        {
            _str = str;
        }
    }
    
    ~Huffman()
    {
        free(compressed);
    }

    bool OK()
    {
        return _err == ERROR::OK;
    }
    std::string str_error()
    {
        if (_str_error.empty())
            _str_error = _STR_ERROR[0];
        return _str_error;
    }

    void encode()
    {
        std::string encoded;
        for (int i = 0; i < _str.length(); i += 2)
        {
            std::string tmp;
            tmp += _str[i];
            if (i < _str.length() - 1)
                tmp += _str[i + 1];
            std::string codeword = _code->get_code(tmp);
            if (codeword.empty())
            {
                set_error(ERROR::WRONG_DATA_TO_DECODE);
                return;
            }
            encoded += codeword;
        }
        num_bits = encoded.length();
        compressed = (char*)calloc(num_bits / 8 + 1 + sizeof(size_t), 1);
        int offset = 0;
        int pos = 0;
        char byte = 0;
        memcpy(compressed, (char*)&num_bits, sizeof(size_t));
        pos += sizeof(size_t);
        
        for (size_t i = 0; i < num_bits; i++, offset++)
        {
            if (i % 8 == 0 && i)
            {
                compressed[pos] = byte;
                pos++;
                offset = 0;
                byte = 0;
            }
            if (encoded[i] == '1')
                byte += 1 << (7 - offset);
            if (i == encoded.length() - 1)
            {
                compressed[pos] = byte;
                pos++;
            }
        }
        out_size = pos;
    }
    void write_encoded(std::string filename = "compressed")
    {
        std::ofstream out(filename);

        if (num_bits % 8 == 0)
            out.write(compressed, num_bits / 8 + sizeof(size_t));
        else
            out.write(compressed, num_bits / 8 + 1 + sizeof(size_t));
    }
    std::string get_encoded() { return compressed; }

    void read_compressed(const std::string& filename)
    {
        std::ifstream infile(filename);
        if (!infile.is_open()){
            set_error(ERROR::CANT_OPEN_READ_FILE);
            return;
        }
        infile.seekg(0, infile.end);
        ssize_t length = infile.tellg();
        infile.seekg(0, infile.beg);
        if (length == -1)
        {
            set_error(ERROR::FAILED_HANDLE_FILE);
            return;
        }

        infile.read((char*)&num_bits, sizeof(size_t));
        
        _str.assign((std::istreambuf_iterator<char>(infile)),
                    std::istreambuf_iterator<char>());
    }
    void decode()
    {
        size_t i = 0;
        int bit = 0;
        while (i * 8 + bit < num_bits)
        {
            std::string tow;
            int ret = _code->get_symbol(_str.c_str(), i, bit, num_bits, tow);
            if (ret < 0)
            {
                set_error(ERROR::WRONG_DATA_TO_DECODE);
                return;
            }
            decoded += tow;
        }
    }
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

private:
    std::string decoded;

    size_t num_bits;
    char* compressed = nullptr;
    int out_size;

    std::string _str;
    std::shared_ptr<Prefix_Code> _code;

    ERROR _err;
    std::string _str_error;
};


#ifdef TEST_MAIN

int main()
{
    std::ifstream bad_case("bigTest");
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

    std::shared_ptr<Prefix_Code> code = std::make_shared<Prefix_Code>(bad_to_enc);
    Huffman huf(bad_to_enc, code);
    huf.encode();
    std::string encoded = huf.get_encoded();
    // std::cout << bad_to_enc << std::endl;

    std::string filename_compressed = "bigTest_extended_hf";
    huf.write_encoded(filename_compressed);
    std::cout << huf.str_error() << std::endl;

    Huffman dec(filename_compressed, code, true);
    std::cout << dec.str_error() << std::endl;
    dec.decode();
    std::cout << dec.str_error() << std::endl;
    std::cout << ((bad_to_enc.compare(dec.get_decoded()) == 0) ? "Success\n" : "Failed\n");
    // std::cout << dec.get_decoded() << std::endl;

    return 0;
}
#endif