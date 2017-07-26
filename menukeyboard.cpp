#include "menukeyboard.h"
#include <map>

namespace NativeMenu {

	const int KEYS_SIZE = 255;

	struct {
		BOOL curr;
		BOOL prev;
	} _keyStates[KEYS_SIZE];

	bool IsWindowFocused() {
		auto foregroundHwnd = GetForegroundWindow();
		DWORD foregroundProcId;
		GetWindowThreadProcessId(foregroundHwnd, &foregroundProcId);
		auto currentProcId = GetCurrentProcessId();
		if (foregroundProcId == currentProcId) {
			return true;
		}
		return false;
	}

	bool IsKeyDown(DWORD key) {
		if (!IsWindowFocused()) return false;
		if (GetAsyncKeyState(key) & 0x8000) return true;
		return false;
	}

	bool IsKeyJustUp(DWORD key, bool exclusive) {
		_keyStates[key].curr = IsKeyDown(key);
		if (!_keyStates[key].curr && _keyStates[key].prev) {
			_keyStates[key].prev = _keyStates[key].curr;
			return true;
		}
		_keyStates[key].prev = _keyStates[key].curr;
		return false;
	}

	DWORD str2key(std::string humanReadableKey) {
		if (humanReadableKey.length() == 1) {
			char letter = humanReadableKey.c_str()[0];
			if ((letter >= 0x30 && letter <= 0x39) || (letter >= 0x41 && letter <= 0x5A)) {
				return static_cast<int>(letter);
			}
		}
		return GetWithDef(KeyMap, humanReadableKey, -1);
	}

	std::string key2str(DWORD key) {
		if (key == -1) return "UNKNOWN";
		if ((key >= 0x30 && key <= 0x39) || (key >= 0x41 && key <= 0x5A)) {
			std::string letter;
			letter = (char)key;
			return std::string(letter);
		}
		for (auto k : KeyMap) {
			if (k.second == key) return k.first;
		}
		return "UNKNOWN";
	}
}
