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

#include "menucontrols.h"
#include "menuutils.h"
#include "menusettings.h"

// TODO: menuutils.h for string utils
namespace NativeMenu {
class Menu {
public:
	/*
	 * c'tor and d'tor. As of current they don't do a lot.
	 */
	Menu();
	~Menu();

	/*
	 * Specify settings file name/location. If not set, it will use default settings.
	 * Calling this is pretty much mandatory. It's advised to call this just once after
	 * figuring out the directory structure.
	 */
	void SetFiles(const std::string & fileName);

	/*
	 * Read settings file. If no specified settings file, it'll use default settings.
	 * Calling this is pretty much mandatory. It's advised to call this whenever new settings
	 * need to be parsed, for example, on re-opening the menu or something.
	 */
	void ReadSettings();

	/*
	 * Registers a function that will be called when the menu is opened.
	 */
	void RegisterOnMain(std::function<void() > onMain);

	/*
	 * Registers a function that will be called when the menu is closed.
	 */
	void RegisterOnExit(std::function<void() > onExit);

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
	 * A menu subtitle.
	 */
	void Subtitle(std::string subtitle);

	/*
	 * Normal option.
	 * Shows nothing special.
	 * Returns true on accept.
	 */
	bool Option(std::string option, std::vector<std::string> details = {});

	/*
	 * Submenu option.
	 * Shows option with menu sign.
	 * Switches menu on action.
	 * Returns true on accept.
	 */
	bool MenuOption(std::string option, std::string menu, std::vector<std::string> details = {});

	/*
	 * Option that shows an extra pane to the right.
	 * Shows text with extra's specifyable in the detail pane.
	 * Function pointers can be passed and are called on right, left press.
	 * Custom pane title can be specified.
	 * Returns true on accept.
	 */
	bool OptionPlus(std::string option, std::vector<std::string> &extra,
					std::function<void()> onRight = nullptr, std::function<void()> onLeft = nullptr, 
					std::string title = "Info", std::vector<std::string> details = {});
	/*
	 * Same as above but with an additional bool ptr for indicating if the option
	 * is being highlighted. Added back because it's needed by some types of options
	 * after all. (Menu-assisted keyboard/text input)
	 */
	bool OptionPlus(std::string option, std::vector<std::string> &extra, bool *highlighted = nullptr, 
					std::function<void()> onRight = nullptr, std::function<void()> onLeft = nullptr ,
					std::string title = "Info", std::vector<std::string> details = {});

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
	 */
	void CheckKeys();

	/*
	 * Closes the menu and calls onExit
	 */
	void CloseMenu();

	const MenuControls &GetControls();

	// TODO: Refactor into Menu.Settings or provide accessors (r/w).
	int optionsFont = 0;
	int titleFont = 1;
	float menux = 0.2f;
	float menuy = 0.125f;
	rgba titleTextColor = { 0, 0, 0, 255 };
	rgba titleRect = { 255, 200, 0, 255 };
	rgba scroller = { 80, 80, 80, 200 };
	rgba options = { 0, 0, 0, 255 };
	rgba optionsrect = { 255, 220, 30, 60 };
	rgba optionsBlack = { 0, 0, 0, 255 };

private:
	MenuControls controls;
	MenuSettings settings;

	std::function<void() > onMain = nullptr;
	std::function<void() > onExit = nullptr;

	/*
	 * Due to how this menu was designed initially, it's expected that 
	 * Menu.End() would be called at the end of the menu tick. At Menu.End(),
	 * all menu options and option counts are known. This information is needed
	 * by the background drawing tasks, so we will store the functions until
	 * the information we need is known. Since we're storing draw calls anyway,
	 * we can split them to draw them in specific "layers". This wasn't a 
	 * problem when the backgrounds were just rects, but with sprites this is
	 * important.
	 */
	typedef std::vector<std::function<void(void)>> functionList;
	functionList backgroundDrawCalls;
	functionList highlightsDrawCalls;
	functionList foregroundDrawCalls;
	
	/*
	 * Detail text also needs to know Y-coordinate to start drawing properly.
	 */
	std::vector<std::string> details;

	/*
	 * These members aren't as modifyable, as they depend on one another. I
	 * wasn't able to find relations between them, so these should not be changed
	 * runtime. They're set to resemble NativeUI / GTA V's UI as much as possible.
	 */
	float menuTextMargin = 0.0075f;
	float optionRightMargin = 0.015f;

	float menuWidth = 0.225f;

	float optionHeight = 0.035f;
	float optionTextSize = 0.45f;
	float optionTextureOffset = optionHeight/2.0f;

	float titleHeight = 0.1f;
	float titleTextSize = 0.85f;
	float titleTextOffset = 0.025f;
	float titleTextureOffset = titleHeight/2.0f;
	
	float subtitleHeight = optionHeight;
	float subtitleTextureOffset = subtitleHeight / 2.0f;
	float subtitleTextSize = 0.45f;

	float detailLineHeight = 0.025f;

	/*
	 * Max items to display. Can be higher or lower, but 12 is a nice
	 * number so the radar doesn't get covered while the description is
	 * readable.
	 */
	int maxDisplay = 12;

	/*
	 * Members for menu state.
	 */
	float totalHeight = 0.0f;
	int optioncount = 0;
	int currentoption = 0;
	bool optionpress = false;
	bool leftpress = false;
	bool rightpress = false;
	bool uppress = false;
	bool downpress = false;
	// Looks like we have 100 menu levels.
	std::array<std::string, 100> currentmenu;
	std::string actualmenu;
	int lastoption[100];
	int menulevel = 0;
	float headerHeight = 0.0f;

	/*
	 * Navigation-related members.
	 */
	unsigned int delay = GetTickCount();
	std::vector<int> menuTimeDelays = {
		240,
		120,
		75,
		40,
		20,
		10,
	};

	const unsigned int menuTimeRepeat = menuTimeDelays[0];

	unsigned int menuTime = menuTimeRepeat;
	bool useNative = true;

	/*
	 * Background textures!
	 */
	const std::vector<std::string> textureNames = {
		"",
		"gradient_nav",
		"interaction_bgd",
		"gradient_bgd",
		"gradient_nav",
	};
	const std::vector<std::string> textureDicts = {
		"",
		"commonmenu",
		"commonmenu",
		"commonmenu",
		"commonmenu",
	};

	int titleTextureIndex = 2;
	int backgTextureIndex = 3;
	int highlTextureIndex = 4;

	float getStringWidth(std::string text);
	std::vector<std::string> splitString(float maxWidth, std::string &details);

	void drawText(const std::string text, int font, float x, float y, float pUnknown, float scale, rgba rgba, int justify = 1);
	void drawRect(float x, float y, float width, float height, rgba rgba);
	void drawSprite(std::string textureDict, std::string textureName, float x, float y, float width, float height, float rotation, rgba rgba);
	void drawAdditionalInfoBoxTitle(std::string title);
	void drawAdditionalInfoBox(std::vector<std::string> &extra, size_t infoLines, std::string title = "Info");
	void drawMenuDetails(std::vector<std::string> details, float y);
	void drawOptionValue(std::string printVar, bool highlighted, int max = 0);

	void changeMenu(std::string menuname);
	void nextOption();
	void previousOption();
	void backMenu();
	void menuBeep();
	void resetButtonStates();
	void disableKeysOnce();
	void enableKeysOnce();
	void disableKeys();
	void processMenuNav(std::function<void()> onMain, std::function<void()> onExit);

	


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
};

}
