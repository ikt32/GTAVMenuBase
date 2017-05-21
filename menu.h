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

//http://stackoverflow.com/questions/36789380/how-to-store-a-const-char-to-a-char
namespace NativeMenu {

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
	Menu();
	~Menu();

	void Title(std::string title);
	bool Option(std::string option, std::vector<std::string> details = {});
	bool OptionPlus(std::string option, std::vector<std::string> &extra, bool *highlighted = nullptr, 
					std::function<void()> onRight = nullptr, std::function<void()> onLeft = nullptr, std::string title = "Info");
	bool MenuOption(std::string option, std::string menu);
	bool IntOption(std::string option, int *var, int min, int max, int step = 1, std::vector<std::string> details = {});
	bool FloatOption(std::string option, float *var, float min, float max, float step = 0.1, std::vector<std::string> details = {});
	bool BoolOption(std::string option, bool *b00l, std::vector<std::string> details = {});
	bool BoolSpriteOption(std::string option, bool b00l, std::string category, std::string spriteOn, std::string spriteOff, std::vector<std::string> details = {});
	bool IntArray(std::string option, int display[], int *PlaceHolderInt, std::vector<std::string> details = {});
	bool FloatArray(std::string option, float display[], int *PlaceHolderInt, std::vector<std::string> details = {});
	bool StringArray(std::string option, std::vector<std::string> display, int *PlaceHolderInt, std::vector<std::string> details = {});
	void TeleportOption(std::string option, float x, float y, float z);

	bool CurrentMenu(std::string menuname);

	void IniWriteInt(LPCWSTR file, LPCWSTR section, LPCWSTR key, int value);
	int IniReadInt(LPCWSTR file, LPCWSTR section, LPCWSTR key);

	void LoadMenuTheme(LPCWSTR file);
	void SaveMenuTheme(LPCWSTR file);
	void disableKeys();
	void drawMenuDetails(std::vector<std::string> details, float y);

	void EndMenu();
	void ProcessMenuNav(MenuControls *controls, std::function<void()> onMain, std::function<void()> onExit);
	void CheckKeys(MenuControls* controls, std::function<void(void) > onMain, std::function<void(void) > onExit);
	void CloseMenu();

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

	// probably keep this grouped like this
	std::vector<std::string> TextureNames = {
		"",
		"gradient_nav",
		"interaction_bgd",
		"gradient_bgd",
		"gradient_nav",
	};
	std::vector<std::string> TextureDicts = {
		"",
		"commonmenu",
		"commonmenu",
		"commonmenu",
		"commonmenu",
	};

	int TitleTextureIndex = 2;
	int BackgTextureIndex = 3;
	int HighlTextureIndex = 4;

private:
	/*
	 * yes hello this hurts me
	 * ok so in this menu the optioncount is final at menu.end()
	 * but if we draw sprites earlier, shit we draw in end() will overlap
	 * so we just need to save the draw calls in order to execute at the
	 * end of end() or at least after we're done drawing the background
	 */

	typedef std::vector<std::function<void(void)>> functionList;
	functionList backgroundDrawCalls;
	functionList highlightsDrawCalls;
	functionList foregroundDrawCalls;
	std::vector<std::string> details;

	float optionHeight = 0.035f;
	float menuWidth = 0.23f;
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

	void drawText(const std::string text, int font, float x, float y, float pUnknown, float scale, rgba rgba, int justify = 1);
	void drawRect(float x, float y, float width, float height, rgba rgba);
	void drawSprite(std::string textureDict, std::string textureName, float x, float y, float width, float height, float rotation, rgba rgba);
	void changeMenu(std::string menuname);
	void nextOption();
	void previousOption();
	void backMenu();
	void menuBeep();
	void resetButtonStates();
	bool useNative = true;
	void drawAdditionalInfoBoxTitle(std::string title);
	void drawAdditionalInfoBox(std::vector<std::string> &extra, size_t infoLines, std::string title = "Info");
};

}
