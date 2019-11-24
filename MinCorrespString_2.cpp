#include <iostream>
#include <vector>
#include <string>
#include <bitset>



constexpr uint32_t alphabet_size = 26;
// This function considers case when we need to run through all prefixes to
// check that there is no prefixes that will be continued by adding new symbol to our result line
void FindMinStringCasePrefixFuncIsNull(const std::vector<uint32_t>& prefix_func
														, std::vector<uint32_t>& result_arr
														, uint32_t index) {
	std::bitset<alphabet_size> does_character_continue_string;
	int32_t current = prefix_func[index - 1];
	for (; current > 0; current = prefix_func[current - 1])
		does_character_continue_string.set(result_arr[current]);
	does_character_continue_string.set(result_arr[current]); // in case current == 0

	uint32_t i = 0;
	for (size_t i = 0; i < does_character_continue_string.size(); ++i) {
		if (!does_character_continue_string[i]) {
			result_arr.push_back(i);
			break;
		}
		++i;
	}
}

std::string MininmalCorrespondingString(const std::vector<uint32_t>& prefix_funcs) {
	std::vector<uint32_t> result_arr(1, 0);
	for (uint32_t i = 1; i < prefix_funcs.size(); ++i) {
		if (prefix_funcs[i] != 0)
			result_arr.push_back(result_arr[prefix_funcs[i] - 1]);
		else
			FindMinStringCasePrefixFuncIsNull(prefix_funcs, result_arr, i);
	}

	std::string result_str;
	for (const auto& i : result_arr)
		result_str.push_back(i + 'a');
	return result_str;
}

int main(int argc, char* argv[]) {
	std::vector<uint32_t> prefix_funcs;

	char c = ' ';
	while (std::cin.get(c) && c != '\n') {
		if (c == ' ')
			continue;
		prefix_funcs.push_back(c - '0');
	}
	std::cout << MininmalCorrespondingString(prefix_funcs);

	return 0;
}