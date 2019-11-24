#include <iostream>
#include <cstdint>
#include <vector>
#include <limits>



uint32_t PrefixFunc(const std::vector<uint32_t>& prefix_funcs, const std::vector<char>& pattern,
	uint32_t prev_prefix_func, char curr_symbol) {
	if (prev_prefix_func == std::numeric_limits<uint32_t>::max())
		return 0;

	uint32_t j = prev_prefix_func;
	while (j > 0 && curr_symbol != pattern[j])
		j = prefix_funcs[j - 1];

	if (curr_symbol == pattern[j])
		++j;

	return j;
}

std::vector<uint32_t> AllStringOccurences(const std::vector<char>& pattern) {
	const char str_delimiter = '#';

	std::vector<char> pattern_with_delimiter = pattern;
	pattern_with_delimiter.push_back(str_delimiter);

	std::vector<uint32_t> occurences;
	std::vector<uint32_t> prefix_funcs(pattern_with_delimiter.size(), 0);
	uint32_t prev_prefix_func = std::numeric_limits<uint32_t>::max();

	uint32_t index = 0;
	for (const char& symbol : pattern_with_delimiter) {
		uint32_t curr_prefix_func = PrefixFunc(prefix_funcs, pattern_with_delimiter, prev_prefix_func, symbol);

		prev_prefix_func = curr_prefix_func;
		prefix_funcs[index] = curr_prefix_func;

		++index;
	}

	char curr_symbol = ' ';
	while (std::cin.get(curr_symbol) && curr_symbol != '\n') {
		uint32_t curr_prefix_func = PrefixFunc(prefix_funcs, pattern_with_delimiter, prev_prefix_func, curr_symbol);
		if (curr_prefix_func == pattern.size())
			occurences.push_back(index - 2 * pattern.size());
		prev_prefix_func = curr_prefix_func;

		++index;
	}

	return occurences;
}

int main(int argc, char* argv[]) {
	std::vector<char> pattern;
	char curr_symbol = ' ';
	while (std::cin.get(curr_symbol) && curr_symbol != '\n') {
		pattern.push_back(curr_symbol);
	}

	std::vector<uint32_t> occurences = AllStringOccurences(pattern);
	for (const uint32_t& elem : occurences)
		std::cout << elem << ' ';

	return 0;
}