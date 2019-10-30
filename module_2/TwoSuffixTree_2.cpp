#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <memory>


class SuffixArray {
public:
	explicit SuffixArray(const std::string& str);
	std::vector<uint64_t> lcp();

	std::vector<uint64_t> Suffixes() const { return suffixes; }

private:
	const uint64_t k_alphabet_size = 256;

	std::string str;
	std::vector<uint64_t> suffixes;
	std::vector<uint64_t> eq_classes;

	void Prepearing();
	// CycledValue fixes negative values of suffix position
	uint64_t CycledValue(int64_t value) const { return (value + str.size()) % str.size(); }

};

void SuffixArray::Prepearing() {
	//str.push_back('%');
	suffixes.resize(str.size());
	uint64_t sz = str.size();
	eq_classes.resize(std::max(k_alphabet_size, sz));

	std::vector<uint64_t> counts(std::max(k_alphabet_size, sz)); // count symbols number in string
	for (const auto& ch : str)
		++counts.at(ch);

	// count position of beginnings of same value blocks
	uint64_t carry = 0;
	for (uint64_t i = 0; i < counts.size() - 1; ++i) {
		uint64_t tmp = counts.at(i);
		counts.at(i) = carry;
		carry += tmp;
	}

	uint64_t suff_beginning = 0;
	for (const auto& ch : str) {
		suffixes.at(counts.at(ch)) = suff_beginning;
		++counts.at(ch);
		++suff_beginning;
	}

	uint64_t curr_eq_class = 0;
	eq_classes.at(0) = 0;
	for (uint64_t i = 1; i < suffixes.size(); ++i) {
		if (str.at(suffixes.at(i)) != str.at(suffixes.at(i - 1)))
			++curr_eq_class;

		eq_classes.at(suffixes.at(i)) = curr_eq_class;
	}
}

SuffixArray::SuffixArray(const std::string& str) : str(str) {

	Prepearing();

	uint64_t curr_len = 1;
	while (curr_len <= str.size()) {
		// second elems of pair was sorted on previous step -> just
		// move beginning of ALL new 2^k substring in beginning of SECOND (2^(k-1))
		// in suffixes (beginnings) array
		std::vector<uint64_t> sorted_by_second_part(suffixes.size());

		for (uint64_t i = 0; i < sorted_by_second_part.size(); ++i)
			sorted_by_second_part.at(i) = CycledValue(suffixes.at(i) - curr_len);

		// next we can sort it using counting sort
		std::vector<uint64_t> counts(suffixes.size());

		for (const auto& suff : sorted_by_second_part)
			++counts.at(eq_classes.at(suff));

		uint64_t carry = 0;
		for (uint64_t i = 0; i < counts.size(); ++i) {
			uint64_t tmp = counts.at(i);
			counts.at(i) = carry;
			carry += tmp;
		}

		//uint32_t pref_beginning = 0;
		for (const auto& suff : sorted_by_second_part) {
			suffixes.at(counts.at(eq_classes.at(suff))) = suff;
			++counts.at(eq_classes.at(suff));
			//++pref_beginning;
		}

		// change classes
		std::vector<uint64_t> new_eq_classes(eq_classes.size());
		uint64_t curr_eq_class = 0;
		new_eq_classes.at(0) = 0;
		for (uint64_t i = 1; i < suffixes.size(); ++i) {
			bool is_first_part_classes_equal =
				eq_classes.at(suffixes.at(i)) == eq_classes.at(suffixes.at(i - 1));
			bool is_second_part_classes_equal =
				eq_classes.at(CycledValue(suffixes.at(i) + curr_len))
				== eq_classes.at(CycledValue(suffixes.at(i - 1) + curr_len));

			if (!(is_first_part_classes_equal
				&& is_second_part_classes_equal)) {
				++curr_eq_class;
			}

			new_eq_classes.at(suffixes.at(i)) = curr_eq_class;
		}

		eq_classes = std::move(new_eq_classes);

		curr_len <<= 1;
	}
}

std::vector<uint64_t> SuffixArray::lcp() {
	// calculate suf^(-1) == suff_positions
	std::vector<uint64_t> suff_positions(suffixes.size());

	for (uint64_t i = 0; i < suff_positions.size(); ++i) {
		suff_positions.at(suffixes.at(i)) = i;
	}

	// calculating lcp
	std::vector<uint64_t> lcp(suffixes.size());
	lcp.at(0) = 0; // first value undefined
	uint64_t prev_lcp = 0;

	for (uint64_t i = 0; i < suff_positions.size(); ++i) {
		// lcp for zero element in suffix array is undefined...
		// ...so, we just skip it
		if (suff_positions.at(i) == 0)
			continue;

		// lcp for current = lcp for previous suffix in string - 1
		// we have matching theorem
		uint64_t curr_lcp = 0;
		if (i > 0 && suff_positions.at(i - 1) != 0 && prev_lcp > 0) // if previous lcp exists (not lcp[0])
			curr_lcp = prev_lcp - 1;

		// find lcp
		uint64_t j = suffixes.at(suff_positions.at(i) - 1); // previous suffix in suffix array

		for (; i + curr_lcp < str.size()
			&& j + curr_lcp < str.size()
			&& str.at(i + curr_lcp) == str.at(j + curr_lcp); ++curr_lcp);
		lcp.at(suff_positions.at(i)) = curr_lcp;

		prev_lcp = lcp.at(suff_positions.at(i));
	}

	return lcp;
}

class SuffixTree {
public:
	SuffixTree(const std::vector<uint64_t>& suffixes
			, const std::vector<uint64_t>& lcp_arr
			, uint64_t first_str_length);

	void PrintTreeDescription();
private:
	struct Node {

		Node(std::shared_ptr<Node> parent, uint64_t depth) : parent(parent)
															, depth(depth)
															, max_list_depth(UINT64_MAX) { 
			++nodes_count; 
		}
		void AddChild(std::shared_ptr<Node> node) { children.push_back(node); }
		void RemoveLastChild() { children.pop_back(); }

		std::shared_ptr<Node> parent;
		std::vector<std::shared_ptr<Node>> children;
		uint64_t depth;
		uint64_t max_list_depth;
		static uint64_t nodes_count;
	};

	typedef std::shared_ptr<Node> shared_ptr;

	shared_ptr root;
	uint64_t str_length;
	uint64_t first_str_length;
	shared_ptr AddNextSuffix(shared_ptr prev_suffix_node, uint64_t length, uint64_t lcp);

	uint64_t MaxListDepth(shared_ptr curr_node);
	uint64_t DFS(shared_ptr parent_node, shared_ptr curr_node, uint64_t parent_number, uint64_t current_number);
};

uint64_t SuffixTree::Node::nodes_count = 0;

SuffixTree::shared_ptr SuffixTree::AddNextSuffix(shared_ptr prev_suffix_node, uint64_t length, uint64_t lcp) {
	// here length is suffix length

	// if we are in root or new suffix includes previous
	// or we just cut the edge and want to add residue of suffix tree
	if (prev_suffix_node->depth == 0 || prev_suffix_node->depth == lcp) {
		// add new node and save it as child
		shared_ptr added = std::make_shared<Node>(prev_suffix_node, length);
		prev_suffix_node->AddChild(added);
		return added;
	}
	else
	{
		if (prev_suffix_node->parent->depth < lcp) {
			// create new node on edge
			shared_ptr inserted = std::make_shared<Node>(prev_suffix_node->parent, lcp);
			// we put strings in lecsicographic order so we 
			// can just remove last child that we added
			prev_suffix_node->parent->RemoveLastChild();
			prev_suffix_node->parent->AddChild(inserted);
			inserted->AddChild(prev_suffix_node);
			prev_suffix_node->parent = inserted;
		}
		return AddNextSuffix(prev_suffix_node->parent, length, lcp);
	}
}


uint64_t SuffixTree::MaxListDepth(shared_ptr curr_node) {
	// if curr_node is list
	if (curr_node->children.empty()) {
		curr_node->max_list_depth = curr_node->depth;
		return curr_node->depth;
	}

	uint64_t max_list_depth = 0;
	for (const auto& child : curr_node->children) {
		max_list_depth = std::max(MaxListDepth(child), max_list_depth);
	}

	curr_node->max_list_depth = max_list_depth;
	return max_list_depth;
}

uint64_t SuffixTree::DFS(shared_ptr parent_node, shared_ptr curr_node, uint64_t parent_number, uint64_t current_number) {
	// returns first node number that wasn`t used
	if (curr_node != root) {
		std::cout << parent_number << ' ';
		uint64_t start = str_length - curr_node->max_list_depth + parent_node->depth;
		uint64_t end = start + curr_node->depth - parent_node->depth;

		if (start < first_str_length) {
			std::cout << 0 << ' ';
			std::cout << start << ' ' << std::min(end, first_str_length) << '\n';
		}
		else {
			std::cout << 1 << ' ';
			std::cout << start - first_str_length << ' '
				<< end - first_str_length << '\n';
		}
	}

	uint64_t child_number = current_number + 1;
	for (const auto& child : curr_node->children) {
		child_number = DFS(curr_node, child, current_number, child_number);
	}

	return child_number;
}

void SuffixTree::PrintTreeDescription() {
	std::cout << Node::nodes_count << '\n';

	MaxListDepth(root);
	DFS(nullptr, root, UINT64_MAX, 0);
}

SuffixTree::SuffixTree(const std::vector<uint64_t>& suffixes
					, const std::vector<uint64_t>& lcp_arr
					, uint64_t first_str_length) : str_length(suffixes.size())
												, first_str_length(first_str_length) {

	root = std::make_shared<Node>(nullptr, 0);

	shared_ptr prev_suffix_node = root;
	for (uint64_t i = 0; i < lcp_arr.size(); ++i) {
		// we get node corresponding to last added suffix
		prev_suffix_node = AddNextSuffix(prev_suffix_node, lcp_arr.size() - suffixes[i], lcp_arr[i]);
	}
}

int main() {
	std::string first_str;
	std::cin >> first_str;
	std::string second_str;
	std::cin >> second_str;
	
	uint64_t first_str_length = first_str.size();
	first_str.insert(first_str.end(), second_str.begin(), second_str.end());

	SuffixArray suf_arr(first_str);
	std::vector<uint64_t> lcp = suf_arr.lcp();

	SuffixTree suf_tree = SuffixTree(suf_arr.Suffixes(), lcp, first_str_length);
	suf_tree.PrintTreeDescription();

	return 0;
}