#include "menuutils.h"

unsigned NativeMenu::numZeroes(unsigned number) {
	unsigned digits = 0;
	while (number % 10 == 0) {
		if (number == 0) break;
		number /= 10;
		digits++;
	}
	return digits;
}

unsigned NativeMenu::behindDec(float f) {
	unsigned max = 6;
	unsigned base = 1;
	for (unsigned i = 0; i < max; i++) {
		base *= 10;
	}
	int nums = (max)-numZeroes(unsigned(f*base));
	return nums < 0 ? 1 : nums;
}
