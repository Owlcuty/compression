#include "prefix_code.h"
#include "measure.h"


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
    char value;
    int probability;
    struct Node* left = nullptr;
    struct Node* right = nullptr;
};

struct comp {
    template <typename T>
 
    bool operator()(const T& l, const T& r) const
    {
        return *l < *r;
    }
};

Prefix_Code::Prefix_Code()
{
    init_tree();
}

Prefix_Code::~Prefix_Code()
{
    for (auto it = freq.begin(); it != freq.end(); it++)
    {
        delete it->second;
    }
    for (size_t i = 0; i < bind_nodes.size(); i++)
    {
        delete bind_nodes[i];
    }
}

void
Prefix_Code::init_tree()
{
    Node* node = new Node();
    node->leaf = true;
    node->value = shift;
    node->probability = 0;
    freq[shift] = node;

    for (int i = 0; i < 255; i ++)
    {
        Node* node = new Node();
        node->leaf = false;
        bind_nodes.push_back(node);
    }

    update_tree(shift);
    node->probability = 0;
}

void
Prefix_Code::update_tree(const char symbol)
{
    if (freq.find(symbol) == freq.end())
    {
        Node* node = new Node();
        node->leaf = true;
        node->value = symbol;
        node->probability = 0;
        freq[symbol] = node;
    }
    (*freq[symbol]) ++;

    for (size_t i = 0; i < bind_nodes.size(); i++)
    {
        bind_nodes[i]->left = nullptr;
        bind_nodes[i]->right = nullptr;        
    }

    build_bin_tree();
#ifdef VERBOSE
    std::cout << index << ":: Update tree after " << (int)symbol << std::endl;
    std::cout << "Build code:\n";
#endif
    build_code(_root, "");
    index ++;
}


const std::string
Prefix_Code::get_code(const char value)
{
    return _code[value];
}

const std::string
Prefix_Code::get_shift_code()
{
    return _code[shift];
}

// TODO REDUCE SIGNIFINCANTLY (common pieces of code)
int
Prefix_Code::get_symbol(const char* str, size_t& offset, int& bit, const size_t num_bits, char& value)
{
    std::cout << index << "::\n";
    std::string got_string;
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
        if (tmp->value == shift)
        {
            got_string += " _Shift::" + get_shift_code() + "_ ";
            check_decoder += get_shift_code();
            char byte = 0;
            for (int b = 0; b < 8; b++)
            {
                if ((str[offset] >> (7 - bit)) & 0x01)
                {
                    byte += 1 << (7 - b);
                    got_string += '1';
                    check_decoder += '1';
                }
                else
                {
                    got_string += '0';
                    check_decoder += '0';
                }
                bit++;
                if (bit == 8)
                {
                    offset++;
                    bit = 0;
                }
            }
            value = byte;
        }
        std::cout << "For " << value << " got string {" << got_string << "}\n";
        return 0;
    }

    std::cout << "Before while on " << offset << "byte " << bit << "bit = " << offset * 8 + bit << "bit\n";
    while (offset * 8 + bit < num_bits && tmp)
    {
        if (bit == 8)
        {
            offset++;
            bit = 0;
        }
        if (tmp->leaf)
        {
            if (tmp->value == shift)
            {
                got_string += " _shift::" + get_shift_code() + "_ ";
                // check_decoder += get_shift_code();
                char byte = 0;
                for (int b = 0; b < 8; b++)
                {
                    if ((str[offset] >> (7 - bit)) & 0x01)
                    {
                        byte += 1 << (7 - b);
                        got_string += '1';
                        check_decoder += '1';
                    }
                    else
                    {
                        got_string += '0';
                        check_decoder += '0';
                    }
                    bit++;
                    if (bit == 8)
                    {
                        offset++;
                        bit = 0;
                    }
                }
                value = byte;
            }
            else
                value = tmp->value;
        std::cout << "fOr " << value << " got string {" << got_string << "}\n";
        std::cout << "Currently on " << offset << "byte " << bit << "bit = " << offset * 8 + bit << "bit\n";
            return 0;
        }
        if (!((str[offset] >> (7 - bit)) & 0x01))
        {
if (offset * 8 + bit > 600 && offset * 8 + bit < 650)
{
    std::cout << (int)str[offset] << ":: " << offset << "byte " << bit << "bit = " << offset * 8 + bit << "bit: 0\n";
}
            got_string += '0';
            check_decoder += '0';
            tmp = tmp->left;
            bit++;
        }
        else if ((str[offset] >> (7 - bit)) & 0x01)
        {
if (offset * 8 + bit > 600 && offset * 8 + bit < 650)
{
    std::cout << (int)str[offset] << ":: " << offset << "byte " << bit << "bit = " << offset * 8 + bit << "bit: 1\n";
}
            got_string += '1';
            check_decoder += '1';
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
        if (tmp->value == shift)
        {
            got_string += " _shifT::" + get_shift_code() + "_ ";
            // check_decoder += get_shift_code();
            char byte = 0;
            for (int b = 0; b < 8; b++)
            {
                if ((str[offset] >> (7 - bit)) & 0x01)
                {
                    byte += 1 << (7 - b);
                    got_string += '1';
                    check_decoder += '1';
                }
                else
                {
                    got_string += '0';
                    check_decoder += '0';
                }
                bit++;
                if (bit == 8)
                {
                    offset++;
                    bit = 0;
                }
            }
            value = byte;
        }
        else
            value = tmp->value;
        std::cout << "foR " << value << " got string {" << got_string << "}\n";
        return 0;
    }
    return -1;
}


// private:


void
Prefix_Code::build_bin_tree()
{
    std::multiset<Node*, comp> alphabet;
    for (auto it = freq.begin(); it != freq.end(); it++)
    {
        alphabet.insert(it->second);
    }
    int bind_index = 0;

    while (alphabet.size() > 1)
    {
        auto last = alphabet.begin();
        auto prelast = last;
        prelast++;

        Node* bind_node = bind_nodes[bind_index];
        bind_node->probability = 0;
        bind_node->probability += (*last)->probability;
        bind_node->probability += (*prelast)->probability;
        bind_node->left = *prelast;
        bind_node->right = *last;
        alphabet.erase(last);
        alphabet.erase(prelast);
        alphabet.insert(bind_node);
        bind_index++;
    }
    _root = *(alphabet.begin());
}

void
Prefix_Code::build_code(const Node* node, std::string codeword)
{
    if (!node) return;
    if (node->leaf)
    {
        if (codeword.empty()) codeword = "0";
        _code[node->value] = codeword;
#ifdef VERBOSE
        if (node->value == shift) std::cout << "Shift";
        else std::cout << node->value;
        std::cout << ":: codeword : " << codeword << "\n";
#endif
    }
    else
    {
        build_code(node->left, codeword + '0');
        build_code(node->right, codeword + '1');
    }
}
