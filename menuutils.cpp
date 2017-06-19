#include "menuutils.h"
#include <locale>

namespace NativeMenu {
	unsigned numZeroes(unsigned number) {
		unsigned digits = 0;
		while (number % 10 == 0) {
			if (number == 0) break;
			number /= 10;
			digits++;
		}
		return digits;
	}

	unsigned behindDec(float f) {
		unsigned max = 6;
		unsigned base = 1;
		for (unsigned i = 0; i < max; i++) {
			base *= 10;
		}
		int nums = (max)-numZeroes(unsigned(f*base));
		return nums < 0 ? 1 : nums;
	}

	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

std::string makeCaps(std::string input) {
		std::locale loc;
		std::stringstream caps;
		for (std::string::size_type i = 0; i < input.length(); ++i)
			caps << std::toupper(input[i], loc);
		return caps.str();
	}
}
