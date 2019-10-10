#include <iostream>
#include <vector>
#include <string>



// This function considers case when we need to run through all prefixes to
// check that there is no prefixes that will be continued by adding new symbol to our result line
void FindMinStringCasePrefixFuncIsNull(const std::vector<uint32_t>& prefix_funcs
									, std::vector<uint32_t>& result_arr
									, uint32_t index) {
	const uint32_t alphabet_size = 26;

	std::vector<bool> does_character_continue_string(alphabet_size, false);

	int32_t current = prefix_funcs.at(index - 1);
	while (current > 0) {
		does_character_continue_string.at(result_arr.at(current)) = true;

		current = prefix_funcs.at(current - 1);
	}
	does_character_continue_string.at(result_arr.at(current)) = true; // in case current == 0

	uint32_t i = 0;
	for (const auto& val : does_character_continue_string) {
		if (!val) {
			result_arr.push_back(i);
			break;
		}

		++i;
	}
}

std::string MininmalCorrespondingString(const std::vector<uint32_t>& prefix_funcs) {
	const uint32_t alphabet_size = 26;

	std::vector<uint32_t> result_arr(1, 0);

	uint32_t index = 0;
	for (const auto& prefix_func : prefix_funcs) {
		if (!index) {
			++index;
			continue;
		}

		else if (prefix_func != 0)
			result_arr.push_back(result_arr.at(prefix_func - 1));
		
		else
			FindMinStringCasePrefixFuncIsNull(prefix_funcs, result_arr, index);

		++index;
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