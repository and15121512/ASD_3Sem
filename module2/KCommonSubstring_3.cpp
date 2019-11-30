#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <limits>



class SuffixArray {
public:
	explicit SuffixArray(const std::string& str);
	std::vector<uint64_t> lcp();

	std::pair<uint64_t, uint64_t> KNumberedCommonString(const std::string& str
		, uint64_t substr_number
		, uint64_t delimiter_position);

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
	str.push_back('$');
	suffixes.resize(str.size());
	uint64_t sz = str.size();
	eq_classes.resize(std::max(k_alphabet_size, sz));

	std::vector<uint64_t> counts(std::max(k_alphabet_size, sz)); // count symbols number in string
	for (const auto& ch : str)
		++counts[ch];

	// count position of beginnings of same value blocks
	uint64_t carry = 0;
	for (uint64_t i = 0; i < counts.size() - 1; ++i) {
		uint64_t tmp = counts[i];
		counts[i] = carry;
		carry += tmp;
	}

	uint64_t suff_beginning = 0;
	for (const auto& ch : str) {
		suffixes[counts[ch]] = suff_beginning;
		++counts[ch];
		++suff_beginning;
	}

	uint64_t curr_eq_class = 0;
	eq_classes[0] = 0;
	for (uint64_t i = 1; i < suffixes.size(); ++i) {
		if (str[suffixes[i]] != str[suffixes[i - 1]])
			++curr_eq_class;

		eq_classes[suffixes[i]] = curr_eq_class;
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
			sorted_by_second_part[i] = CycledValue(suffixes[i] - curr_len);

		// next we can sort it using counting sort
		std::vector<uint64_t> counts(suffixes.size());

		for (const auto& suff : sorted_by_second_part)
			++counts[eq_classes[suff]];

		uint64_t carry = 0;
		for (uint64_t i = 0; i < counts.size(); ++i) {
			uint64_t tmp = counts[i];
			counts[i] = carry;
			carry += tmp;
		}

		//uint32_t pref_beginning = 0;
		for (const auto& suff : sorted_by_second_part) {
			suffixes[counts[eq_classes[suff]]] = suff;
			++counts[eq_classes[suff]];
		}

		// change classes
		std::vector<uint64_t> new_eq_classes(eq_classes.size());
		uint64_t curr_eq_class = 0;
		new_eq_classes[0] = 0;
		for (uint64_t i = 1; i < suffixes.size(); ++i) {
			bool is_first_part_classes_equal =
				eq_classes[suffixes[i]] == eq_classes[suffixes[i - 1]];
			bool is_second_part_classes_equal =
				eq_classes[CycledValue(suffixes[i] + curr_len)]
				== eq_classes[CycledValue(suffixes[i - 1] + curr_len)];

			if (!(is_first_part_classes_equal
				&& is_second_part_classes_equal)) {
				++curr_eq_class;
			}

			new_eq_classes[suffixes[i]] = curr_eq_class;
		}

		eq_classes = std::move(new_eq_classes);

		curr_len <<= 1;
	}
}

std::vector<uint64_t> SuffixArray::lcp() {
	// calculate suf^(-1) == suff_positions
	std::vector<uint64_t> suff_positions(suffixes.size());

	for (uint64_t i = 0; i < suff_positions.size(); ++i) {
		suff_positions[suffixes[i]] = i;
	}

	// calculating lcp
	std::vector<uint64_t> lcp(suffixes.size());
	lcp[0] = 0; // first value undefined
	uint64_t prev_lcp = 0;

	for (uint64_t i = 0; i < suff_positions.size(); ++i) {
		// lcp for zero element in suffix array is undefined...
		// ...so, we just skip it
		if (suff_positions[i] == 0)
			continue;

		// lcp for current = lcp for previous suffix in string - 1
		// we have matching theorem
		uint64_t curr_lcp = 0;
		if (i > 0 && suff_positions[i - 1] != 0 && prev_lcp > 0) // if previous lcp exists (not lcp[0])
			curr_lcp = prev_lcp - 1;

		// find lcp
		uint64_t j = suffixes[suff_positions[i] - 1]; // previous suffix in suffix array

		for (; i + curr_lcp < str.size()
			&& j + curr_lcp < str.size()
			&& str[i + curr_lcp] == str[j + curr_lcp]; ++curr_lcp);
		lcp[suff_positions[i]] = curr_lcp;

		prev_lcp = lcp[suff_positions[i]];
	}

	return lcp;
}

// idea of this algorithm:
// we keep the "frame" of already found common substrings
// that we can calculate twice later ---
// it`s number of common symbols for current suffix and
// LAST suffix where we found new substrings
std::pair<uint64_t, uint64_t> SuffixArray::KNumberedCommonString(const std::string& str
	, uint64_t substr_number
	, uint64_t delimiter_position) {
	// strings must be written through delimiter '%'!
	uint64_t common_part = 0; // we don`t have any common symbols
	uint64_t remaining_to_find = substr_number; // how many common substrings
												// we need to find now

	std::vector<uint64_t> lcp_arr = lcp();
	uint64_t k_substr_start = std::numeric_limits<uint64_t>::max();
	uint64_t k_substr_length = std::numeric_limits<uint64_t>::max();
	for (uint64_t i = 1; i < suffixes.size(); ++i) {
		// if our suffixes starts in one string
		// we can`t find new common substring here
		// also check if largest common substring in suffix < common_part
		// because in this case we are to do noting
		int64_t value = (suffixes[i - 1] - delimiter_position)
			* (delimiter_position - suffixes[i]);
		if (!(value > 0)
			|| lcp_arr[i] < common_part) {
			// just change "frame" value
			common_part = std::min(common_part, lcp_arr[i]);
			continue;
		}

		// next our suffixes starts in different strings
		if (remaining_to_find > lcp_arr[i] - common_part) {
			// add new common substrs to number of all common substrs
			remaining_to_find -= lcp_arr[i] - common_part;
			// refresh "frame" value
			common_part = lcp_arr[i];
			continue;
		}
		else {
			// we already found suffix with
			// k-numbered common substring
			k_substr_start = suffixes[i];
			// if we want to get last symbol of substring
			// we need to do ... - 1 also but we want position after last symbol
			k_substr_length = common_part + remaining_to_find;
			break;
		}
	}

	return std::make_pair(k_substr_start, k_substr_length);
}

int main() {
	std::string first_str;
	std::cin >> first_str;
	std::string second_str;
	std::cin >> second_str;
	uint64_t k;
	std::cin >> k;

	first_str.push_back('%'); // write strings in a row through the delimiter '%'
	uint64_t delimiter_position = first_str.size() - 1;
	first_str.insert(first_str.end(), second_str.begin(), second_str.end());

	SuffixArray suf_arr(first_str);
	std::pair<int64_t, int64_t> substr_beginning = suf_arr.KNumberedCommonString(first_str, k, delimiter_position);

	std::string result;
	if (substr_beginning.first != std::numeric_limits<uint64_t>::max()) { // if k-numbered string exists
		result = first_str.substr(substr_beginning.first
			, substr_beginning.second);
	}
	else {
		result = "-1";
	}

	std::cout << result;

	return 0;
}