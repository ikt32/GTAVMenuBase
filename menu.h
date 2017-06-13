#pragma once

/*
 * Menu system that was originally from sudomod, but with a bunch of
 * changes to make working with it easier.
 */

#include <string>
#include <windows.h>
#include <vector>
#include <functional>
#include <array>
#include <sstream>

// TODO: menuutils.h for string utils
// TODO: menusettings class
// TODO: MenuControls owned by Menu
namespace NativeMenu {

// TODO: Refactor into some Utils class
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

class MenuControls;

struct rgba {
	int r, g, b, a;
};

class Menu {
public:
	/*
	 * c'tor and d'tor. As of current they don't do a lot.
	 */
	Menu();
	~Menu();

	/*
	 * Main menu is always called "mainmenu".
	 * Otherwise menuname is a submenu name.
	 * Returns true when inside the submenu menuname.
	 */
	bool CurrentMenu(std::string menuname);

	/*
	 * Always assign a title to a submenu!
	 */
	void Title(std::string title);

	/*
	 * Submenu option.
	 * Shows option with menu sign.
	 * Switches menu on action.
	 * Returns true on accept.
	 */
	bool MenuOption(std::string option, std::string menu, std::vector<std::string> details = {});

	/*
	 * Normal option.
	 * Shows nothing special.
	 * Returns true on accept.
	 */
	bool Option(std::string option, std::vector<std::string> details = {});

	/*
	 * Option that shows an extra pane to the right.
	 * Shows text with extra's specifyable in the detail pane.
	 * Function pointers can be passed and are called on right, left press.
	 * Custom pane title can be specified.
	 * Returns true on accept.
	 */
	bool OptionPlus(std::string option, std::vector<std::string> &extra,
					std::function<void()> onRight = nullptr, std::function<void()> onLeft = nullptr, 
					std::string title = "Info", std::vector<std::string> details = {}); // help

	/*
	 * Option that changes an int value with optional custom-sized steps.
	 * Shows option with the value inside < > brackets.
	 * Returns true on accept, left and right.
	 */
	bool IntOption(		std::string option, int &var,	int min,	int max,	int step = 1,		std::vector<std::string> details = {});

	/*
	 * Option that changes a float value with optional custom-sized steps.
	 * Shows option with the value inside < > brackets.
	 * Returns true on accept, left and right.
	 */
	bool FloatOption(	std::string option, float &var, float min,	float max,	float step = 0.1f,	std::vector<std::string> details = {});

	/*
	 * Option that toggles a boolean.
	 * Shows option with a checkbox, which is checked when the passed var is "true".
	 * Returns true on accept.
	 */
	bool BoolOption(std::string option, bool &var, std::vector<std::string> details = {});

	/*
	 * Option that displays a boolean with a specifyable texture.
	 * Shows option with a checkbox, which is checked when the passed var is "true".
	 * Returns true on accept.
	 */
	bool BoolSpriteOption(std::string option, bool var, std::string category, std::string spriteOn, std::string spriteOff, std::vector<std::string> details = {});

	/*
	 * Option that shows a scrollable list of supplied ints.
	 * Shows option with the current value inside < > brackets.
	 * Value shown is display[iterator].
	 * On left or right press, iterator's value is incremented or decremented.
	 * Returns true on accept, left and right.
	 */
	bool IntArray(std::string option, std::vector<int> display, int &iterator, std::vector<std::string> details = {});

	/*
	 * Option that shows a scrollable list of supplied floats.
	 * Shows option with the current value inside < > brackets.
	 * Value shown is display[iterator].
	 * On left or right press, iterator's value is incremented or decremented.
	 * Returns true on accept, left and right.
	 */
	bool FloatArray(std::string option, std::vector<float> display, int &iterator, std::vector<std::string> details = {});
	
	/*
	 * Option that shows a scrollable list of supplied strings.
	 * Shows option with the current value inside < > brackets.
	 * Value shown is display[iterator].
	 * On left or right press, iterator's value is incremented or decremented.
	 * Returns true on accept, left and right.
	 */
	bool StringArray(std::string option, std::vector<std::string> display, int &iterator, std::vector<std::string> details = {});

	/*
	 * Draws the menu backgrounds and processes menu navigation key inputs.
	 */
	void EndMenu();

	/*
	 * Use at the beginning of the menu update loop!
	 * Checks input keys and processes them for navigation in the menu with MenuControls
	 * Function pointers can be passed. 
	 * onMain is executed on main menu open. 
	 * onExit is executed on main menu close.
	 */
	void CheckKeys(MenuControls* controls, std::function<void() > onMain = nullptr, std::function<void() > onExit = nullptr);

	/*
	 * Closes the menu and calls onExit
	 */
	void CloseMenu();


	// TODO: Refactor into Menu.Settings
	void IniWriteInt(LPCWSTR file, LPCWSTR section, LPCWSTR key, int value);
	int IniReadInt(LPCWSTR file, LPCWSTR section, LPCWSTR key);
	void LoadMenuTheme(LPCWSTR file);
	void SaveMenuTheme(LPCWSTR file);

	// TODO: Refactor into Menu.Settings or provide accessors.
	int optionsFont = 0;
	int titleFont = 1;
	float menux = 0.2f;
	float menuy = 0.125f;
	rgba titleText = { 0, 0, 0, 255 };
	rgba titleRect = { 255, 200, 0, 255 };
	rgba scroller = { 80, 80, 80, 200 };
	rgba options = { 0, 0, 0, 255 };
	rgba optionsrect = { 255, 220, 30, 60 };
	int optionsrectAlpha = 0;
	rgba optionsBlack = { 0, 0, 0, 255 };

private:
	std::function<void() > onExit = nullptr;

	/*
	 * ok so in this menu the optioncount is final at menu.end()
	 * but if we draw sprites earlier, what we draw in end() will overlap
	 * so we just need to save the draw calls in order to execute at the
	 * end of end() or at least after we're done drawing the backgrounds
	 */
	typedef std::vector<std::function<void(void)>> functionList;
	functionList backgroundDrawCalls;
	functionList highlightsDrawCalls;
	functionList foregroundDrawCalls;
	std::vector<std::string> details;

	float detailLineHeight = 0.025f;
	float optionHeight = 0.035f;
	float menuWidth = 0.23f;
	float titleTextSize = 0.85f;
	float titleHeight = 0.085f;
	float textureTextOffset = 0.0165f;
	float menuTextMargin = 0.0075f;
	float optionTextSize = 0.45f;
	float optionRightMargin = 0.015f;
	

	int optioncount = 0;
	int currentoption = 0;
	bool optionpress = false;
	bool leftpress = false;
	bool rightpress = false;
	bool uppress = false;
	bool downpress = false;

	std::array<std::string, 100> currentmenu;
	std::string actualmenu;
	int lastoption[100];
	int menulevel = 0;
	int infocount = 0;
	unsigned int delay = GetTickCount();

	const unsigned int menuTimeRepeat = 240;
	const unsigned int menuTimeSlow = 120;
	const unsigned int menuTimeMedium = 75;
	const unsigned int menuTimeFast = 40;
	unsigned int menuTime = menuTimeRepeat;
	bool useNative = true;

	std::vector<std::string> textureNames = {
		"",
		"gradient_nav",
		"interaction_bgd",
		"gradient_bgd",
		"gradient_nav",
	};
	std::vector<std::string> textureDicts = {
		"",
		"commonmenu",
		"commonmenu",
		"commonmenu",
		"commonmenu",
	};

	int titleTextureIndex = 2;
	int backgTextureIndex = 3;
	int highlTextureIndex = 4;

	void drawText(const std::string text, int font, float x, float y, float pUnknown, float scale, rgba rgba, int justify = 1);
	void drawRect(float x, float y, float width, float height, rgba rgba);
	void drawSprite(std::string textureDict, std::string textureName, float x, float y, float width, float height, float rotation, rgba rgba);
	void changeMenu(std::string menuname);
	void nextOption();
	void previousOption();
	void backMenu();
	void menuBeep();
	void resetButtonStates();
	void drawAdditionalInfoBoxTitle(std::string title);
	void drawAdditionalInfoBox(std::vector<std::string> &extra, size_t infoLines, std::string title = "Info");
	void disableKeys();
	float getStringWidth(std::string text);
	std::vector<std::string> splitString(float maxWidth, std::string &details);
	void processMenuNav(MenuControls *controls, std::function<void()> onMain, std::function<void()> onExit);

	
	void drawMenuDetails(std::vector<std::string> details, float y);
	void drawOptionValue(std::string printVar, bool highlighted, int max = 0);

	template <typename T>
	bool processOptionItemControls(T &var, T min, T max, T step) {
		if (currentoption == optioncount) {
			if (leftpress) {
				if (var <= min) var = max;
				else var -= step;
				leftpress = false;
				return true;
			}
			if (var < min) var = max;
			if (rightpress) {
				if (var >= max) var = min;
				else var += step;
				rightpress = false;
				return true;
			}
			if (var > max) var = min;
		}

		if (optionpress && currentoption == optioncount)
			return true;
		return false;
	}

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

	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}
};

}
