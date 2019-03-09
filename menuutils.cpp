#include "menuutils.h"
#include <locale>
#include <sstream>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

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
		::split(s, delim, std::back_inserter(elems));
		return elems;
	}

float lerp(float a, float b, float f) {
    return a + f * (b - a);
}
}
