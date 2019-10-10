#include <iostream>
#include <vector>
#include <queue>
#include <cstdint>
#include <string>
#include <memory>



class Trie {
public:
	Trie(const std::vector<std::vector<char>>& strings);

	static void PrintAllPatternOccurences(const std::vector<char>& mask);

private:
	struct Node;
	typedef std::shared_ptr<Node> shared_ptr;

	shared_ptr root;
	std::vector<std::vector<char>> strings;

	shared_ptr current_state; // текущее состояние бора, соответствующее переданной
							// в него ранее последовательности

	shared_ptr SuffixLink(shared_ptr node);
	shared_ptr CompressedSuffixLink(shared_ptr node);
	shared_ptr NextForTransition(shared_ptr node, char c);

	void AddString(uint32_t string_index);

	bool IsRoot(const shared_ptr node) const { return node == root; }

	static void PrintMasklessPartsOccurences(const std::vector<std::vector<char>>& strings
		, const std::vector<uint32_t>& strings_indexes
		, uint32_t mask_size);

	static void PrintPatternOccurences(const std::vector<uint32_t>& numbers_of_corresp_strings
		, uint32_t text_size
		, uint32_t mask_size
		, uint32_t patterns_without_mask_number);
};

struct Trie::Node {
	std::vector<shared_ptr> sons;
	bool is_leaf; // по умолчанию все node НЕ is_leaf
	std::vector<uint32_t> corresp_string; // индексы соответствующих строк
	std::vector<shared_ptr> next_for_transition;

	shared_ptr parent;
	shared_ptr suff_link;
	shared_ptr up_link;
	int32_t parent_edge_char;

	Node(shared_ptr parent
		, int32_t parent_edge_char = INT32_MAX) : parent(parent)
		, corresp_string(std::vector<uint32_t>())
		, suff_link(nullptr)
		, up_link(nullptr)
		, parent_edge_char(parent_edge_char)
		, is_leaf(false) {
		const uint32_t k_alphabet_size = 26;
		sons = std::vector<shared_ptr>(k_alphabet_size, shared_ptr(nullptr));
		next_for_transition = std::vector<shared_ptr>(k_alphabet_size, shared_ptr(nullptr));
	}

	shared_ptr Son(char c) const { return sons[c - 'a']; }
	shared_ptr NextForTransition(char c) const { return next_for_transition[c]; }

	void AddSon(char c, shared_ptr son) { sons[c - 'a'] = son; }
	void AddCorrespString(uint32_t string) { corresp_string.push_back(string); }
	void AddCorrespStrings(std::vector<uint32_t>& strings) {
		for (const auto& str : corresp_string)
			strings.push_back(str);
	}
};

Trie::Trie(const std::vector<std::vector<char>>& strings_) {
	strings = strings_;
	//root = new Node(nullptr);
	root = std::make_shared<Node>(nullptr);
	current_state = root;

	for (uint32_t strs_index = 0; strs_index < strings.size(); ++strs_index)
		AddString(strs_index);
}

void Trie::AddString(uint32_t string_index) {
	shared_ptr current = root;
	for (const auto& c : strings[string_index]) {
		if (!current->Son(c)) {
			shared_ptr son = std::make_shared<Node>(current, c);
			current->AddSon(c, son);
		}

		current = current->Son(c);
	}

	current->AddCorrespString(string_index);
	current->is_leaf = true;
}

Trie::shared_ptr Trie::SuffixLink(shared_ptr node) {
	if (!node->suff_link) {

		if (node == root || node->parent == root)
			node->suff_link = root;

		else
			node->suff_link = NextForTransition(SuffixLink(node->parent), node->parent_edge_char);
	}

	return node->suff_link;
}

Trie::shared_ptr Trie::NextForTransition(shared_ptr node, char c) {
	char c_index = c - 'a';

	if (!node->next_for_transition[c_index]) {

		if (node->Son(c))
			node->next_for_transition[c_index] = node->Son(c);

		else if (node == root)
			node->next_for_transition[c_index] = root;

		else
			node->next_for_transition[c_index] = NextForTransition(SuffixLink(node), c);
	}

	return node->next_for_transition[c_index];
}

Trie::shared_ptr Trie::CompressedSuffixLink(shared_ptr node) {

	if (!node->up_link) {
		shared_ptr suff_link = SuffixLink(node);

		if (suff_link->is_leaf)
			node->up_link = suff_link;

		else if (SuffixLink(node) == root)
			node->up_link = root;

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
	// число безмасочных кусков шаблона, которые стоят на своей позиции в шаблоне, если бы шаблон
	// начинался в данном индексе

	const char k_terminate_symbol = '|';
	uint32_t index = 0;
	for (; ; ++index) {
		char c;
		std::cin >> c;
		if (std::cin.eof() || c == k_terminate_symbol)
			break;

		numbers_of_corresp_strings.push_back(0);

		trie.current_state = trie.NextForTransition(trie.current_state, c);

		if (!(trie.CompressedSuffixLink(trie.current_state) == trie.root)) {
			shared_ptr current = trie.current_state;

			while (!trie.IsRoot(current)) {
				current = trie.CompressedSuffixLink(current);

				for (const uint32_t& occur_index : current->corresp_string) {
					const int32_t begin_index = index + 1 - strings_indexes[occur_index];
					if (begin_index >= 0) // если шаблон не вылезет за текст
						++numbers_of_corresp_strings[begin_index];
				}
			}
		}

		if (trie.current_state->is_leaf) {
			for (const uint32_t& occur_index : trie.current_state->corresp_string) {
				const int32_t begin_index = index + 1 - strings_indexes[occur_index];
				if (begin_index >= 0) // если шаблон не вылезет за текст
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