#include "menukeyboard.h"
#include <map>

const int sKeysSize = 255;
const int sNowPeriod = 100;
const int sMaxDown = 30000;

const std::unordered_map<std::string, int> sKeyMap = {
    // CTRL/SHIFT already have their left and right variants mapped
    //keymap["SHIFT"] = VK_SHIFT;
    //keymap["CTRL"] = VK_CONTROL;
    { "XB1" , VK_XBUTTON1 },
    { "XB2" , VK_XBUTTON2 },
    { "LMB" , VK_LBUTTON },
    { "RMB" , VK_RBUTTON },
    { "CANCEL" , VK_CANCEL },
    { "MMB" , VK_MBUTTON },
    { "BACKSPACE" , VK_BACK },
    { "TAB" , VK_TAB },
    { "CLEAR" , VK_CLEAR },
    { "RETURN" , VK_RETURN },
    { "ALT" , VK_MENU },
    { "PAUSE" , VK_PAUSE },
    { "CAPSLOCK" , VK_CAPITAL },
    { "ESCAPE" , VK_ESCAPE },
    { "SPACE" , VK_SPACE },
    { "PAGEUP" , VK_PRIOR },
    { "PAGEDOWN" , VK_NEXT },
    { "END" , VK_END },
    { "HOME" , VK_HOME },
    { "LEFT" , VK_LEFT },
    { "UP" , VK_UP },
    { "RIGHT" , VK_RIGHT },
    { "DOWN" , VK_DOWN },
    { "SELECT" , VK_SELECT },
    { "PRINT" , VK_PRINT },
    { "EXECUTE" , VK_EXECUTE },
    { "PRINTSCREEN" , VK_SNAPSHOT },
    { "INSERT" , VK_INSERT },
    { "DELETE" , VK_DELETE },
    { "HELP" , VK_HELP },
    { "LWIN" , VK_LWIN },
    { "RWIN" , VK_RWIN },
    { "APPS" , VK_APPS },
    { "SLEEP" , VK_SLEEP },
    { "NUM0" , VK_NUMPAD0 },
    { "NUM1" , VK_NUMPAD1 },
    { "NUM2" , VK_NUMPAD2 },
    { "NUM3" , VK_NUMPAD3 },
    { "NUM4" , VK_NUMPAD4 },
    { "NUM5" , VK_NUMPAD5 },
    { "NUM6" , VK_NUMPAD6 },
    { "NUM7" , VK_NUMPAD7 },
    { "NUM8" , VK_NUMPAD8 },
    { "NUM9" , VK_NUMPAD9 },
    { "*" , VK_MULTIPLY },
    { "PLUS" , VK_ADD },
    { "," , VK_SEPARATOR },
    { "MINUS" , VK_SUBTRACT },
    { "." , VK_DECIMAL },
    { "/" , VK_DIVIDE },
    { "F1" , VK_F1 },
    { "F2" , VK_F2 },
    { "F3" , VK_F3 },
    { "F4" , VK_F4 },
    { "F5" , VK_F5 },
    { "F6" , VK_F6 },
    { "F7" , VK_F7 },
    { "F8" , VK_F8 },
    { "F9" , VK_F9 },
    { "F10" , VK_F10 },
    { "F11" , VK_F11 },
    { "F12" , VK_F12 },
    { "F13" , VK_F13 },
    { "F14" , VK_F14 },
    { "F15" , VK_F15 },
    { "F16" , VK_F16 },
    { "F17" , VK_F17 },
    { "F18" , VK_F18 },
    { "F19" , VK_F19 },
    { "F20" , VK_F20 },
    { "F21" , VK_F21 },
    { "F22" , VK_F22 },
    { "F23" , VK_F23 },
    { "F24" , VK_F24 },
    { "VK_F1" , VK_F1 },
    { "VK_F2" , VK_F2 },
    { "VK_F3" , VK_F3 },
    { "VK_F4" , VK_F4 },
    { "VK_F5" , VK_F5 },
    { "VK_F6" , VK_F6 },
    { "VK_F7" , VK_F7 },
    { "VK_F8" , VK_F8 },
    { "VK_F9" , VK_F9 },
    { "VK_F10" , VK_F10 },
    { "VK_F11" , VK_F11 },
    { "VK_F12" , VK_F12 },
    { "VK_F13" , VK_F13 },
    { "VK_F14" , VK_F14 },
    { "VK_F15" , VK_F15 },
    { "VK_F16" , VK_F16 },
    { "VK_F17" , VK_F17 },
    { "VK_F18" , VK_F18 },
    { "VK_F19" , VK_F19 },
    { "VK_F20" , VK_F20 },
    { "VK_F21" , VK_F21 },
    { "VK_F22" , VK_F22 },
    { "VK_F23" , VK_F23 },
    { "VK_F24" , VK_F24 },
    { "NUMLOCK" , VK_NUMLOCK },
    { "SCROLL" , VK_SCROLL },
    { "LSHIFT" , VK_LSHIFT },
    { "RSHIFT" , VK_RSHIFT },
    { "LCTRL" , VK_LCONTROL },
    { "RCTRL" , VK_RCONTROL },
    { "LMENU" , VK_LMENU },
    { "RMENU" , VK_RMENU },
    { "BROWSER_BACK" , VK_BROWSER_BACK },
    { "BROWSER_FORWARD" , VK_BROWSER_FORWARD },
    { "BROWSER_REFRESH" , VK_BROWSER_REFRESH },
    { "BROWSER_STOP" , VK_BROWSER_STOP },
    { "BROWSER_SEARCH" , VK_BROWSER_SEARCH },
    { "BROWSER_FAVORITES" , VK_BROWSER_FAVORITES },
    { "BROWSER_HOME" , VK_BROWSER_HOME },
    { "VOLUME_MUTE" , VK_VOLUME_MUTE },
    { "VOLUME_DOWN" , VK_VOLUME_DOWN },
    { "VOLUME_UP" , VK_VOLUME_UP },
    { "MEDIA_NEXT_TRACK" , VK_MEDIA_NEXT_TRACK },
    { "MEDIA_PREV_TRACK" , VK_MEDIA_PREV_TRACK },
    { "MEDIA_STOP" , VK_MEDIA_STOP },
    { "MEDIA_PLAY_PAUSE" , VK_MEDIA_PLAY_PAUSE },
    { "LAUNCH_MAIL" , VK_LAUNCH_MAIL },
    { "LAUNCH_MEDIA_SELECT" , VK_LAUNCH_MEDIA_SELECT },
    { "LAUNCH_APP1" , VK_LAUNCH_APP1 },
    { "LAUNCH_APP2" , VK_LAUNCH_APP2 },
    { "PLAY" , VK_PLAY },
    { "ZOOM" , VK_ZOOM },
    { "VK_OEM_1" , VK_OEM_1 },              // ';:' for US
    { "VK_OEM_PLUS" , VK_OEM_PLUS },        // '+' any country
    { "VK_OEM_COMMA" , VK_OEM_COMMA },      // ',' any country
    { "VK_OEM_MINUS" , VK_OEM_MINUS },      // '-' any country
    { "VK_OEM_PERIOD" , VK_OEM_PERIOD },    // '.' any country
    { "VK_OEM_2" , VK_OEM_2 },              // '/?' for US
    { "VK_OEM_3" , VK_OEM_3 },              // '`~' for US
    { "VK_OEM_4" , VK_OEM_4 },              // '{' for US
    { "VK_OEM_5" , VK_OEM_5 },              // '\|' for US
    { "VK_OEM_6" , VK_OEM_6 },              // '}' for US
    { "VK_OEM_7" , VK_OEM_7 },              // ''"' for US
    { "VK_OEM_8" , VK_OEM_8 },              // � !
    { "VK_OEM_102" , VK_OEM_102 },          // > <
};

struct {
    DWORD time;
    BOOL isWithAlt;
    BOOL wasDownBefore;
    BOOL isUpNow;
} keyStates[sKeysSize];

// http://stackoverflow.com/questions/2333728/stdmap-default-value
template <template<class, class, class...> class C, typename K, typename V, typename... Args>
V GetWithDef(const C<K, V, Args...>& m, K const& key, const V& defval) {
    auto it = m.find(key);
    if (it == m.end())
        return defval;
    return it->second;
}

void ResetKeyState(DWORD key) {
    if (key < sKeysSize)
        memset(&keyStates[key], 0, sizeof(keyStates[0]));
}

void NativeMenu::OnKeyboardMessage(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow) {
    if (key < sKeysSize) {
        keyStates[key].time = GetTickCount();
        keyStates[key].isWithAlt = isWithAlt;
        keyStates[key].wasDownBefore = wasDownBefore;
        keyStates[key].isUpNow = isUpNow;
    }
}

bool NativeMenu::IsKeyDown(DWORD key) {
    return (key < sKeysSize) ?
        ((GetTickCount() < keyStates[key].time + sMaxDown) && !keyStates[key].isUpNow) :
        false;
}

bool NativeMenu::IsKeyJustUp(DWORD key, bool exclusive) {
    bool b = (key < sKeysSize) ?
        (GetTickCount() < keyStates[key].time + sMaxDown && keyStates[key].isUpNow) :
        false;
    if (b && exclusive)
        ResetKeyState(key);
    return b;
}

DWORD NativeMenu::GetKeyFromName(const std::string& name) {
    if (name.length() == 1) {
        char letter = name.c_str()[0];
        if ((letter >= 0x30 && letter <= 0x39) || (letter >= 0x41 && letter <= 0x5A)) {
            return static_cast<int>(letter);
        }
    }
    return GetWithDef(sKeyMap, name, -1);
}

std::string NativeMenu::GetNameFromKey(DWORD key) {
    if (key == -1)
        return "UNKNOWN";
    if ((key >= 0x30 && key <= 0x39) || (key >= 0x41 && key <= 0x5A)) {
        return { static_cast<char>(key) };
    }
    for (const auto& k : sKeyMap) {
        if (k.second == key)
            return k.first;
    }
    return "UNKNOWN";
}
