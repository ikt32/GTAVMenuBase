#pragma once
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

/*
 * Misc utilities that don't use natives and would clutter the main
 * classes.
 */

namespace NativeMenu {
	struct rgba {
		int r, g, b, a;
	};

	unsigned numZeroes(unsigned number);
	unsigned behindDec(float f);

	// http://stackoverflow.com/questions/36789380/how-to-store-a-const-char-to-a-char
	class CharAdapter {
	public:
		explicit CharAdapter(const char* s) : m_s(::_strdup(s)) { }
		explicit CharAdapter(std::string str) : m_s(::_strdup(str.c_str())) { }

		CharAdapter(const CharAdapter& other) = delete; // non construction-copyable
		CharAdapter& operator=(const CharAdapter&) = delete; // non copyable

		~CharAdapter() /*free memory on destruction*/ {
			::free(m_s); /*use free to release strdup memory*/
		}
		operator char*() /*implicit cast to char* */ {
			return m_s;
		}

	private:
		char* m_s;
	};

	// https://stackoverflow.com/questions/236129/split-a-string-in-c
	template<typename Out>
	void split(const std::string &s, char delim, Out result) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

	std::vector<std::string> split(const std::string &s, char delim);
}
