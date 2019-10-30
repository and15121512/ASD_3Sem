#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>



class SuffixArray {
public:
	explicit SuffixArray(const std::string& str);
	std::vector<uint32_t> lcp();

	uint32_t SubstringsCount();

private:
	const uint32_t k_alphabet_size = 256;

	std::string str;
	std::vector<uint32_t> suffixes;
	std::vector<uint32_t> eq_classes;

	void Prepearing();
	// CycledValue fixes negative values of suffix position
	uint32_t CycledValue(int32_t value) const { return (value + str.size()) % str.size(); }

};

void SuffixArray::Prepearing() {
	str.push_back('$');
	suffixes.resize(str.size());
	uint32_t sz = str.size();
	eq_classes.resize(std::max(k_alphabet_size, sz));

	std::vector<uint32_t> counts(std::max(k_alphabet_size, sz)); // count symbols number in string
	for (const auto& ch : str)
		++counts.at(ch);

	// count position of beginnings of same value blocks
	uint32_t carry = 0;
	for (uint32_t i = 0; i < counts.size() - 1; ++i) {
		uint32_t tmp = counts.at(i);
		counts.at(i) = carry;
		carry += tmp;
	}

	uint32_t suff_beginning = 0;
	for (const auto& ch : str) {
		suffixes.at(counts.at(ch)) = suff_beginning;
		++counts.at(ch);
		++suff_beginning;
	}

	uint32_t curr_eq_class = 0;
	eq_classes.at(0) = 0;
	for (uint32_t i = 1; i < suffixes.size(); ++i) {
		if (str.at(suffixes.at(i)) != str.at(suffixes.at(i - 1)))
			++curr_eq_class;

		eq_classes.at(suffixes.at(i)) = curr_eq_class;
	}
}

SuffixArray::SuffixArray(const std::string& str) : str(str) {

	Prepearing();

	uint32_t curr_len = 1;
	while (curr_len <= str.size()) {
		// second elems of pair was sorted on previous step -> just
		// move beginning of ALL new 2^k substring in beginning of SECOND (2^(k-1))
		// in suffixes (beginnings) array
		std::vector<uint32_t> sorted_by_second_part(suffixes.size());

		for (uint32_t i = 0; i < sorted_by_second_part.size(); ++i)
			sorted_by_second_part.at(i) = CycledValue(suffixes.at(i) - curr_len);

		// next we can sort it using counting sort
		std::vector<uint32_t> counts(suffixes.size());

		for (const auto& suff : sorted_by_second_part)
			++counts.at(eq_classes.at(suff));

		uint32_t carry = 0;
		for (uint32_t i = 0; i < counts.size(); ++i) {
			uint32_t tmp = counts.at(i);
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
		std::vector<uint32_t> new_eq_classes(eq_classes.size());
		uint32_t curr_eq_class = 0;
		new_eq_classes.at(0) = 0;
		for (uint32_t i = 1; i < suffixes.size(); ++i) {
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

std::vector<uint32_t> SuffixArray::lcp() {
	// calculate suf^(-1) == suff_positions
	std::vector<uint32_t> suff_positions(suffixes.size());

	for (uint32_t i = 0; i < suff_positions.size(); ++i) {
		suff_positions.at(suffixes.at(i)) = i;
	}

	// calculating lcp
	std::vector<uint32_t> lcp(suffixes.size());
	lcp.at(0) = 0; // first value undefined
	uint32_t prev_lcp = 0;

	for (uint32_t i = 0; i < suff_positions.size(); ++i) {
		// lcp for zero element in suffix array is undefined...
		// ...so, we just skip it
		if (suff_positions.at(i) == 0)
			continue;

		// lcp for current = lcp for previous suffix in string - 1
		// we have matching theorem
		uint32_t curr_lcp = 0;
		if (i > 0 && suff_positions.at(i - 1) != 0 && prev_lcp > 0) // if previous lcp exists (not lcp[0])
			curr_lcp = prev_lcp - 1;

		// find lcp
		uint32_t j = suffixes.at(suff_positions.at(i) - 1); // previous suffix in suffix array

		for (; i + curr_lcp < str.size()
			&& j + curr_lcp < str.size()
			&& str.at(i + curr_lcp) == str.at(j + curr_lcp); ++curr_lcp);
		lcp.at(suff_positions.at(i)) = curr_lcp;

		prev_lcp = lcp.at(suff_positions.at(i));
	}

	return lcp;
}

uint32_t SuffixArray::SubstringsCount() {
	std::vector<uint32_t> lcp_arr = lcp();
	uint32_t result = 0;

	for (uint32_t i = 1; i < str.size(); ++i) {
		result += str.size() - suffixes.at(i) - lcp_arr.at(i) - 1;
	}

	// we count 5 substrings with $
	// and also we count 1 empty string
	return result;
}

int main() {
	std::string str;
	std::cin >> str;

	SuffixArray suf_arr(str);
	std::cout << suf_arr.SubstringsCount() << std::endl;

	return 0;
}