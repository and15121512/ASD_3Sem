#include <iostream>
#include <vector>
#include <queue>
#include <cstdint>
#include <string>
#include <memory>



inline char index_from_char(char c) { return c - 'a'; }

class Trie {
public:
    Trie(const std::vector<std::vector<char>>& strings);

    static void PrintAllPatternOccurences(const std::vector<char>& mask);

private:
    struct Node;
    //typedef std::unique_ptr<Node> unique_ptr;

    std::unique_ptr<Node> root;
    std::vector<std::vector<char>> strings;

    Node* current_state;

    Node* SuffixLink(Node* node);
    Node* CompressedSuffixLink(Node* node);
    Node* NextForTransition(Node* node, char c);

    void AddString(uint32_t string_index);

    bool IsRoot(const Node* node) const { return node == root.get(); }

    static void PrintMasklessPartsOccurences(const std::vector<std::vector<char>>& strings
        , const std::vector<uint32_t>& strings_indexes
        , uint32_t mask_size);

    static void PrintPatternOccurences(const std::vector<uint32_t>& numbers_of_corresp_strings
        , uint32_t text_size
        , uint32_t mask_size
        , uint32_t patterns_without_mask_number);
};

const uint32_t k_alphabet_size = 26;

struct Trie::Node {
    std::vector<std::unique_ptr<Node>> sons;
    bool is_leaf;
    std::vector<uint32_t> corresp_string;
    std::vector<Node*> next_for_transition;

    Node* parent;
    Node* suff_link;
    Node* up_link;
    int32_t parent_edge_char;

    Node(Node* parent
        , int32_t parent_edge_char = INT32_MAX) : parent(parent)
        , corresp_string(std::vector<uint32_t>())
        , suff_link(nullptr)
        , up_link(nullptr)
        , parent_edge_char(parent_edge_char)
        , is_leaf(false) {
        for (size_t i = 0; i < k_alphabet_size; ++i)
            sons.emplace_back(nullptr);
        next_for_transition = std::vector<Node*>(k_alphabet_size, nullptr);
    }

    Node* Son(char c) const { return sons[index_from_char(c)].get(); }
    Node* NextForTransition(char c) const { return next_for_transition[c]; }

    void MakeSon(char c, Node* node) {
        Node* son = new Node(node, c);
        sons[index_from_char(c)].reset(son);
    }
    void AddCorrespString(uint32_t string) { corresp_string.push_back(string); }
    void AddCorrespStrings(std::vector<uint32_t>& strings) {
        for (const auto& str : corresp_string)
            strings.push_back(str);
    }
};

Trie::Trie(const std::vector<std::vector<char>>& strings_) {
    strings = strings_;
    root = std::make_unique<Node>(nullptr);
    current_state = root.get();

    for (uint32_t strs_index = 0; strs_index < strings.size(); ++strs_index)
        AddString(strs_index);
}

void Trie::AddString(uint32_t string_index) {
    Node* current = root.get();
    for (const auto& c : strings[string_index]) {
        if (!current->Son(c)) {
            current->MakeSon(c, current);
        }

        current = current->Son(c);
    }

    current->AddCorrespString(string_index);
    current->is_leaf = true;
}

Trie::Node* Trie::SuffixLink(Node* node) {
    if (!node->suff_link) {

        if (node == root.get() || node->parent == root.get())
            node->suff_link = root.get();

        else
            node->suff_link = NextForTransition(SuffixLink(node->parent), node->parent_edge_char);
    }

    return node->suff_link;
}

Trie::Node* Trie::NextForTransition(Node* node, char c) {
    char c_index = index_from_char(c);

    if (!node->next_for_transition[c_index]) {

        if (node->Son(c))
            node->next_for_transition[c_index] = node->Son(c);

        else if (node == root.get())
            node->next_for_transition[c_index] = root.get();

        else
            node->next_for_transition[c_index] = NextForTransition(SuffixLink(node), c);
    }

    return node->next_for_transition[c_index];
}

Trie::Node* Trie::CompressedSuffixLink(Node* node) {

    if (!node->up_link) {
        Node* suff_link = SuffixLink(node);

        if (suff_link->is_leaf)
            node->up_link = suff_link;

        else if (SuffixLink(node) == root.get())
            node->up_link = root.get();

        else
            node->up_link = CompressedSuffixLink(suff_link);
    }
    return node->up_link;
}

void Trie::PrintPatternOccurences(const std::vector<uint32_t>& numbers_of_corresp_strings
    , uint32_t text_size
    , uint32_t mask_size
    , uint32_t patterns_without_mask_number) {
    uint32_t index = 0;
    for (const uint32_t& num : numbers_of_corresp_strings) {
        if (index + mask_size > text_size)
            break;
        if (num == patterns_without_mask_number)
            std::cout << index << ' ';
        ++index;
    }
}

void Trie::PrintMasklessPartsOccurences(const std::vector<std::vector<char>>& strings
    , const std::vector<uint32_t>& strings_indexes
    , uint32_t mask_size) {
    Trie trie(strings);
    std::vector<uint32_t> numbers_of_corresp_strings;

    const char k_terminate_symbol = '|';
    uint32_t index = 0;
    for (; ; ++index) {
        char c;
        std::cin >> c;
        if (std::cin.eof() || c == k_terminate_symbol)
            break;

        numbers_of_corresp_strings.push_back(0);

        trie.current_state = trie.NextForTransition(trie.current_state, c);

        if (!(trie.CompressedSuffixLink(trie.current_state) == trie.root.get())) {
            Node* current = trie.current_state;

            while (!trie.IsRoot(current)) {
                current = trie.CompressedSuffixLink(current);

                for (const uint32_t& occur_index : current->corresp_string) {
                    const int32_t begin_index = index + 1 - strings_indexes[occur_index];
                    if (begin_index >= 0)
                        ++numbers_of_corresp_strings[begin_index];
                }
            }
        }

        if (trie.current_state->is_leaf) {
            for (const uint32_t& occur_index : trie.current_state->corresp_string) {
                const int32_t begin_index = index + 1 - strings_indexes[occur_index];
                if (begin_index >= 0)
                    ++numbers_of_corresp_strings[begin_index];
            }
        }

    }

    PrintPatternOccurences(numbers_of_corresp_strings, index, mask_size, strings.size());
}

void Trie::PrintAllPatternOccurences(const std::vector<char>& mask) {
    const char k_mask_delimiter = '?';

    std::vector<std::vector<char>> strings(1, std::vector<char>());
    std::vector<uint32_t> strings_indexes;

    uint32_t index = 0;
    for (const char& c : mask) {
        if (c == k_mask_delimiter) {
            if (!strings.back().empty()) {
                strings_indexes.push_back(index);
                strings.push_back(std::vector<char>());
            }
        }
        else
            strings.back().push_back(c);

        ++index;
    }

    if (strings.back().empty())
        strings.pop_back();
    else
        strings_indexes.push_back(index);

    PrintMasklessPartsOccurences(strings, strings_indexes, mask.size());
}

//-------------------------------------------------------------------------------------

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<char> mask;
    std::string mask_str;

    std::cin >> mask_str;
    for (const auto& symb : mask_str)
        mask.push_back(symb);

    Trie::PrintAllPatternOccurences(mask);

    return 0;
}