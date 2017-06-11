/* MenuClass.cpp */

#include "menu.h"
#include <functional>

#include "inc/natives.h"
#include "inc/enums.h"
#include "menucontrols.h"

namespace NativeMenu {

Menu::Menu() { }

Menu::~Menu() { }

bool Menu::CurrentMenu(std::string menuname) {
	if (menuname == actualmenu) return true;
	return false;
}

void Menu::changeMenu(std::string menuname) {
	currentmenu[menulevel] = actualmenu;
	lastoption[menulevel] = currentoption;
	menulevel++;
	actualmenu = menuname;
	currentoption = 1;
	menuBeep();
	resetButtonStates();
}

void Menu::nextOption() {
	if (currentoption < optioncount)
		currentoption++;
	else
		currentoption = 1;
	if (menulevel > 0) {
		menuBeep();
	}
	resetButtonStates();
}

void Menu::previousOption() {
	if (currentoption > 1)
		currentoption--;
	else
		currentoption = optioncount;
	if (menulevel > 0) {
		menuBeep();
	}
	resetButtonStates();
}

void Menu::backMenu() {
	if (menulevel > 0) {
		menuBeep();
	}
	menulevel--;
	actualmenu = currentmenu[menulevel];
	currentoption = lastoption[menulevel];

}

void Menu::drawText(const std::string text, int font, float x, float y, float pUnknown, float scale, rgba rgba, int justify) {
	// justify: 0 - center, 1 - left, 2 - right
	if (justify == 2) {
		UI::SET_TEXT_WRAP(menux - menuWidth / 2, menux + menuWidth / 2 - optionRightMargin / 2.0f);
	}
	UI::SET_TEXT_JUSTIFICATION(justify);
	
	UI::SET_TEXT_FONT(font);
	if (font == 0) { // big-ass Chalet London
		scale *= 0.75f;
		y += 0.003f;
	}
	UI::SET_TEXT_SCALE(0.0f, scale);
	UI::SET_TEXT_COLOUR(rgba.r, rgba.g, rgba.b, rgba.a);
	UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(CharAdapter(text));
	UI::END_TEXT_COMMAND_DISPLAY_TEXT(x, y);
}

void Menu::drawRect(float x, float y, float width, float height, rgba rgba) {
	GRAPHICS::DRAW_RECT(x, y, width, height, rgba.r, rgba.g, rgba.b, rgba.a);
}

void Menu::drawSprite(std::string textureDict, std::string textureName, float x, float y, float width, float height, float rotation, rgba rgba) {
	if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(CharAdapter(textureDict))) GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(CharAdapter(textureDict), false);
	else GRAPHICS::DRAW_SPRITE(CharAdapter(textureDict), CharAdapter(textureName), x, y, width, height, rotation, rgba.r, rgba.g, rgba.b, rgba.a);
}

void Menu::Title(std::string title) {
	optioncount = 0;
	drawText(title, titleFont, menux, menuy - 0.03f, 0.85f, 0.85f, titleText, 0);
	if (TitleTextureIndex < 1 || TitleTextureIndex >= TextureDicts.size()) {
		drawRect(menux, menuy - 0.0075f, menuWidth, titleHeight, titleRect);
	}
	else {
		backgroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, TextureDicts[TitleTextureIndex], TextureNames[TitleTextureIndex], 
			menux, menuy - 0.0075f, menuWidth, titleHeight, 0.0f, titleRect)
		);
	}
}

bool Menu::Option(std::string option, std::vector<std::string> details) {
	optioncount++;

	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;

	bool doDraw = false;
	float optiony;
	float optiontexty;

	if (currentoption <= 16 && optioncount <= 16)
	{
		doDraw = true;
		optiony = optioncount * optionHeight + (menuy + textureTextOffset);
		optiontexty = (optioncount * optionHeight + menuy);
	}
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
	{
		doDraw = true;
		optiony = (optioncount - (currentoption - 16)) * optionHeight + (menuy + textureTextOffset);
		optiontexty = ((optioncount - (currentoption - 16)) * optionHeight + menuy);
	}

	if (doDraw) {
		drawRect(menux, optiony, menuWidth, optionHeight, optionsrect);
		if (thisOption) {
			if (HighlTextureIndex < 1 || HighlTextureIndex >= TextureDicts.size()) {
				drawRect(menux, optiony, menuWidth, optionHeight, scroller);
			}
			else {
				highlightsDrawCalls.push_back(
					std::bind(&Menu::drawSprite, this, TextureDicts[HighlTextureIndex], TextureNames[HighlTextureIndex],
					menux, optiony, menuWidth, optionHeight, 0.0f, scroller)
				);
			}
			if (details.size() > 0) {
				this->details = details;
			}
		}
		drawText(option, optionsFont, (menux - menuWidth/2.0f) + menuTextMargin, optiontexty, optionTextSize, optionTextSize, thisOption ? optionsBlack : options);
	}

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

void Menu::drawAdditionalInfoBoxTitle(std::string title) {
	float extrax = menux + menuWidth;

	drawText(title, titleFont, extrax, menuy - 0.03f, 0.85f, 0.85f, titleText, 0);
	if (TitleTextureIndex < 1 || TitleTextureIndex >= TextureDicts.size()) {
		drawRect(extrax, menuy - 0.0075f, menuWidth, titleHeight, titleRect);
	}
	else {
		backgroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, TextureDicts[TitleTextureIndex], TextureNames[TitleTextureIndex],
			extrax, menuy - 0.0075f, menuWidth, titleHeight, 180.0f, titleRect)
		);
	}
}

void Menu::drawAdditionalInfoBox(std::vector<std::string> &extra, size_t infoLines, std::string title) {
	float extrax = menux + menuWidth;

	drawAdditionalInfoBoxTitle(title);
	float extray;
	for (int i = 0; i < infoLines; i++) {
		extray = i * optionHeight + (menuy + 0.0515f);
		drawText(extra[i], optionsFont, menux + 0.125f, i * optionHeight + (menuy + optionHeight), optionTextSize, optionTextSize, options);
		if (HighlTextureIndex < 1 || HighlTextureIndex >= TextureDicts.size()) {
			drawRect(extrax, extray, menuWidth, optionHeight, scroller);
		}
		else {
			optionsrect.a = 0;
		}
	}
	auto tempoptions = optionsrect;
	tempoptions.a = 255;
	highlightsDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, TextureDicts[BackgTextureIndex], TextureNames[BackgTextureIndex],
		extrax, (menuy + optionHeight) + (infoLines * optionHeight) / 2, menuWidth, optionHeight * infoLines, 0.0f, tempoptions)
	);
}

bool Menu::OptionPlus(std::string option, std::vector<std::string> &extra, bool *highlighted, std::function<void() > onRight, std::function<void() > onLeft, std::string title) {
	Option(option);

	size_t infoLines = extra.size();
	bool thisOption = false;
	if (currentoption == optioncount) {
		thisOption = true;
		if (onLeft && leftpress) {
			onLeft();
			leftpress = false;
			return false;
		}
		if (onRight && rightpress) {
			onRight();
			rightpress = false;
			return false;
		}
	}

	if (thisOption && ((currentoption <= 16 && optioncount <= 16) ||
		((optioncount > (currentoption - 16)) && optioncount <= currentoption))) {
		drawAdditionalInfoBox(extra, infoLines, title);
	}

	if (highlighted != nullptr) {
		*highlighted = thisOption;
	}

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

bool Menu::MenuOption(std::string option, std::string menu, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	if (currentoption <= 16 && optioncount <= 16)
		drawText(">>", optionsFont, menux + menuWidth/2.0f - optionRightMargin, (optioncount * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText(">>", optionsFont, menux + menuWidth/2.0f - optionRightMargin, ((optioncount - (currentoption - 16)) * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);

	if (optionpress && currentoption == optioncount) {
		optionpress = false;
		changeMenu(menu);
		return true;
	}
	return false;
}

bool Menu::IntOption(std::string option, int *var, int min, int max, int step, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	if (currentoption <= 16 && optioncount <= 16)
		drawText("< " + std::to_string(*var) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, (optioncount * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText("< " + std::to_string(*var) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, ((optioncount - (currentoption - 16)) * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);

	if (currentoption == optioncount) {
		if (leftpress) {
			if (*var <= min) *var = max;
			else *var -= step;
			leftpress = false;
			return true;
		}
		if (*var < min) *var = max;
		if (rightpress) {
			if (*var >= max) *var = min;
			else *var += step;
			rightpress = false;
			return true;
		}
		if (*var > max) *var = min;
	}

	if (optionpress && currentoption == optioncount)
		return true;
	return false;
}

bool Menu::FloatOption(std::string option, float *var, float min, float max, float step, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	char buf[100];
	_snprintf_s(buf, sizeof(buf), "%.2f", *var);

	if (currentoption <= 16 && optioncount <= 16)
		drawText("< " + std::string(buf) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, optioncount * optionHeight + menuy, optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);
	else if (optioncount > currentoption - 16 && optioncount <= currentoption)
		drawText("< " + std::string(buf) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, (optioncount - (currentoption - 16)) * optionHeight + menuy, optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);

	if (currentoption == optioncount) {
		if (leftpress) {
			if (*var <= min) *var = max;
			else *var -= step;
			leftpress = false;
			return true;
		}
		if (*var < min) *var = max;
		if (rightpress) {
			if (*var >= max) *var = min;
			else *var += step;
			rightpress = false;
			return true;
		}
		if (*var > max) *var = min;
	}

	if (optionpress && currentoption == optioncount)
		return true;
	return false;
}

bool Menu::BoolOption(std::string option, bool *b00l, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	char * tickBoxTexture;
	rgba optionColors;
	optionColors = options;

	if (thisOption) {
		tickBoxTexture = *b00l ? "shop_box_tickb" : "shop_box_blankb";
	}
	else {
		tickBoxTexture = *b00l ? "shop_box_tick" : "shop_box_blank";
	}

	bool doDraw = false;
	float textureY;
	
	if (currentoption <= 16 && optioncount <= 16) {
		doDraw = true;
		textureY = (optioncount * optionHeight + (menuy + 0.016f));
	}
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption) {
		doDraw = true;
		textureY = ((optioncount - (currentoption - 16)) * optionHeight + (menuy + 0.016f));
	}

	if (doDraw) {
		int resX, resY;
		GRAPHICS::_GET_ACTIVE_SCREEN_RESOLUTION(&resX, &resY);
		float ratio = (float)resX / (float)resY;
		float boxSz = 0.05;
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", tickBoxTexture,
			menux + menuWidth/2.0f - optionRightMargin, textureY, boxSz/ratio, boxSz, 0.0f, optionColors)
		);
	}

	if (optionpress && currentoption == optioncount) {
		*b00l ^= 1;
		return true;
	}
	return false;
}

bool Menu::BoolSpriteOption(std::string option, bool b00l, std::string category, std::string spriteOn, std::string spriteOff, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	if (currentoption <= 16 && optioncount <= 16) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, category, b00l ? spriteOn : spriteOff,
			menux + menuWidth/2.0f - optionRightMargin, (optioncount * optionHeight + (menuy + 0.016f)), 0.03f, 0.05f, 0.0f, thisOption ? optionsBlack : options));
	}
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, category, b00l ? spriteOn : spriteOff,
			menux + menuWidth/2.0f - optionRightMargin, ((optioncount - (currentoption - 16)) * optionHeight + (menuy + 0.016f)), 0.03f, 0.05f, 0.0f, thisOption ? optionsBlack : options));
	}
			

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

bool Menu::IntArray(std::string option, int display[], int *PlaceHolderInt, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	int min = 0;
	int max = sizeof(display) / sizeof(*display);

	if (currentoption == optioncount) {
		if (leftpress) {
			if (*PlaceHolderInt <= min) *PlaceHolderInt = max;
			else PlaceHolderInt -= 1;
			leftpress = false;
			return true;
		}
		if (*PlaceHolderInt < min) *PlaceHolderInt = max;
		if (rightpress) {
			if (*PlaceHolderInt >= max) *PlaceHolderInt = min;
			else *PlaceHolderInt += 1;
			rightpress = false;
			return true;
		}
		if (*PlaceHolderInt > max) *PlaceHolderInt = min;
	}
	if (currentoption <= 16 && optioncount <= 16)
		drawText("< " + std::to_string(display[*PlaceHolderInt]) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, (optioncount * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText("< " + std::to_string(display[*PlaceHolderInt]) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, ((optioncount - (currentoption - 16)) * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);

	if (optionpress && currentoption == optioncount)
		return true;
	return false;
}

bool Menu::FloatArray(std::string option, float display[], int *PlaceHolderInt, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	int min = 0;
	int max = sizeof(display) / sizeof(*display);

	if (currentoption == optioncount) {
		if (leftpress) {
			if (*PlaceHolderInt <= min) *PlaceHolderInt = max;
			else *PlaceHolderInt -= 1;
			leftpress = false;
			return true;
		}
		if (*PlaceHolderInt < min) *PlaceHolderInt = max;
		if (rightpress) {
			if (*PlaceHolderInt >= max) *PlaceHolderInt = min;
			else *PlaceHolderInt += 1;
			rightpress = false;
			return true;
		}
		if (*PlaceHolderInt > max) *PlaceHolderInt = min;
	}

	char buf[30];
	_snprintf_s(buf, sizeof(buf), "%.2f", display[*PlaceHolderInt]);

	if (currentoption <= 16 && optioncount <= 16)
		drawText("< " + std::string(buf) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, (optioncount * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText("< " + std::string(buf) + " >", optionsFont, menux + menuWidth/2.0f - optionRightMargin, ((optioncount - (currentoption - 16)) * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);

	if (optionpress && currentoption == optioncount)
		return true;
	return false;
}

bool Menu::StringArray(std::string option, std::vector<std::string>display, int *PlaceHolderInt, std::vector<std::string> details) {
	Option(option, details);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	int min = 0;
	int max = static_cast<int>(display.size()) - 1;

	if (currentoption == optioncount) {
		if (leftpress) {
			if (*PlaceHolderInt <= min) *PlaceHolderInt = max;
			else *PlaceHolderInt -= 1;
			leftpress = false;
		}
		if (*PlaceHolderInt < min) *PlaceHolderInt = max;
		if (rightpress) {
			if (*PlaceHolderInt >= max) *PlaceHolderInt = min;
			else *PlaceHolderInt += 1;
			rightpress = false;
		}
		if (*PlaceHolderInt > max) *PlaceHolderInt = min;
	}
	std::string leftArrow = "< ";
	std::string rightArrow = " >";
	if (max == 0) {
		leftArrow = rightArrow = "";
	}
	if (currentoption <= 16 && optioncount <= 16)
		drawText(leftArrow + std::string(display[*PlaceHolderInt]) + rightArrow, optionsFont, menux + menuWidth/2.0f - optionRightMargin, (optioncount * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText(leftArrow + std::string(display[*PlaceHolderInt]) + rightArrow, optionsFont, menux + menuWidth/2.0f - optionRightMargin, ((optioncount - (currentoption - 16)) * optionHeight + menuy), optionTextSize, optionTextSize, thisOption ? optionsBlack : options, 2);

	if (optionpress && currentoption == optioncount)
		return true;
	return false;
}

void Menu::TeleportOption(std::string option, float x, float y, float z) {
	Option(option);

	if (currentoption == optioncount && optionpress) {
		Entity handle = PLAYER::PLAYER_PED_ID();
		if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false)) handle = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(handle, x, y, z, false, false, false);
	}
}

void Menu::IniWriteInt(LPCWSTR file, LPCWSTR section, LPCWSTR key, int value) {
	wchar_t newValue[256];
	wsprintfW(newValue, L"%d", value);
	WritePrivateProfileStringW(section, key, newValue, file);
}

int Menu::IniReadInt(LPCWSTR file, LPCWSTR section, LPCWSTR key) {
	int returning = GetPrivateProfileIntW(section, key, NULL, file);
	return returning;
}

void Menu::LoadMenuTheme(LPCWSTR file) {
	// Title Text
	titleText.r = IniReadInt(file, L"Title Text", L"Red");
	titleText.g = IniReadInt(file, L"Title Text", L"Green");
	titleText.b = IniReadInt(file, L"Title Text", L"Blue");
	titleText.a = IniReadInt(file, L"Title Text", L"Alpha");
	titleFont = IniReadInt(file, L"Title Text", L"Font");
	// Title Rect
	titleRect.r = IniReadInt(file, L"Title Rect", L"Red");
	titleRect.g = IniReadInt(file, L"Title Rect", L"Green");
	titleRect.b = IniReadInt(file, L"Title Rect", L"Blue");
	titleRect.a = IniReadInt(file, L"Title Rect", L"Alpha");

	// Scroller
	scroller.r = IniReadInt(file, L"Scroller", L"Red");
	scroller.g = IniReadInt(file, L"Scroller", L"Green");
	scroller.b = IniReadInt(file, L"Scroller", L"Blue");
	scroller.a = IniReadInt(file, L"Scroller", L"Alpha");

	// Option Text
	options.r = IniReadInt(file, L"Options Text", L"Red");
	options.g = IniReadInt(file, L"Options Text", L"Green");
	options.b = IniReadInt(file, L"Options Text", L"Blue");
	options.a = IniReadInt(file, L"Options Text", L"Alpha");
	optionsFont = IniReadInt(file, L"Options Text", L"Font");

	// Option Rect
	optionsrect.r = IniReadInt(file, L"Options Rect", L"Red");
	optionsrect.g = IniReadInt(file, L"Options Rect", L"Green");
	optionsrect.b = IniReadInt(file, L"Options Rect", L"Blue");
	optionsrect.a = IniReadInt(file, L"Options Rect", L"Alpha");
	optionsrectAlpha = optionsrect.a;
}

void Menu::SaveMenuTheme(LPCWSTR file) {
	// Title Text
	IniWriteInt(file, L"Title Text", L"Red", titleText.r);
	IniWriteInt(file, L"Title Text", L"Green", titleText.g);
	IniWriteInt(file, L"Title Text", L"Blue", titleText.b);
	IniWriteInt(file, L"Title Text", L"Alpha", titleText.a);
	IniWriteInt(file, L"Title Text", L"Font", titleFont);

	// Title Rect
	IniWriteInt(file, L"Title Rect", L"Red", titleRect.r);
	IniWriteInt(file, L"Title Rect", L"Green", titleRect.g);
	IniWriteInt(file, L"Title Rect", L"Blue", titleRect.b);
	IniWriteInt(file, L"Title Rect", L"Alpha", titleRect.a);

	// Scroller 
	IniWriteInt(file, L"Scroller", L"Red", scroller.r);
	IniWriteInt(file, L"Scroller", L"Green", scroller.g);
	IniWriteInt(file, L"Scroller", L"Blue", scroller.b);
	IniWriteInt(file, L"Scroller", L"Alpha", scroller.a);

	// Options Text
	IniWriteInt(file, L"Options Text", L"Red", options.r);
	IniWriteInt(file, L"Options Text", L"Green", options.g);
	IniWriteInt(file, L"Options Text", L"Blue", options.b);
	IniWriteInt(file, L"Options Text", L"Alpha", options.a);
	IniWriteInt(file, L"Options Text", L"Font", optionsFont);

	// Options Rect
	IniWriteInt(file, L"Options Rect", L"Red", optionsrect.r);
	IniWriteInt(file, L"Options Rect", L"Green", optionsrect.g);
	IniWriteInt(file, L"Options Rect", L"Blue", optionsrect.b);
	IniWriteInt(file, L"Options Rect", L"Alpha", optionsrect.a);
}

void Menu::disableKeys() {
	UI::HIDE_HELP_TEXT_THIS_FRAME();
	CAM::SET_CINEMATIC_BUTTON_ACTIVE(0);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(10);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(6);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(7);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(9);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(8);

	for (int i = 0; i <= 2; i++) {
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlNextCamera, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleCinCam, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlCinematicSlowMo, true);

		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlPhone, true);

		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSelectCharacterMichael, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSelectCharacterFranklin, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSelectCharacterTrevor, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSelectCharacterMultiplayer, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlCharacterWheel, true);
										 
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlMeleeAttackLight, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlMeleeAttackHeavy, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlMeleeAttackAlternate, true);
										 
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlMap, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlMultiplayerInfo, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlMapPointOfInterest, true);
										 
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlRadioWheelLeftRight, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleNextRadio, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehiclePrevRadio, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlRadioWheelUpDown, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleNextRadioTrack, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehiclePrevRadioTrack, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleRadioWheel, true);
										 
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleDuck, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleSelectNextWeapon, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleSelectPrevWeapon, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleAttack, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleAttack2, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleExit, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSelectWeapon, true);

		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlWeaponWheelNext, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlWeaponWheelPrev, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlNextWeapon, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlPrevWeapon, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSelectWeapon, true);

		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlContext, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlContextSecondary, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSelectWeapon, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleHeadlight, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleRoof, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleHorn, true);
										 
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehicleAim, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlVehiclePassengerAim, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlFrontendSocialClub, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlFrontendSocialClubSecondary, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlReplayStartStopRecording, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlInteractionMenu, true);
		CONTROLS::DISABLE_CONTROL_ACTION(i, ControlSaveReplayClip, true);
	}
}

void Menu::drawMenuDetails(std::vector<std::string> details, float y) {
	for (auto i = 0; i < details.size(); i++) {
		drawText(details[i], optionsFont, (menux - menuWidth / 2.0f) + menuTextMargin, i * optionHeight + y, optionTextSize, optionTextSize, options);
	}
	auto tempoptions = optionsrect;
	tempoptions.a = 255;
	drawRect(menux, y, menuWidth, optionHeight/8, {0,0,0,255});

	backgroundDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, TextureDicts[BackgTextureIndex], TextureNames[BackgTextureIndex],
		menux, y + (splitDetails.size() * optionHeight) / 2, menuWidth, optionHeight * splitDetails.size(), 0.0f, tempoptions));
}

void Menu::EndMenu() {
	if (menulevel < 1)
		return;

	int maxOptionCount = optioncount > 16 ? 16 : optioncount;

	float footerTextY;
	float footerBackY;

	if (optioncount > 16) {
		footerTextY = 17 * optionHeight + menuy;
	}
	else {
		footerTextY = (optioncount + 1) * optionHeight + menuy;
	}
	footerBackY = footerTextY + textureTextOffset;


	// Footer
	if (TitleTextureIndex < 1 || TitleTextureIndex >= TextureDicts.size()) {
		drawRect(menux, footerBackY, menuWidth, optionHeight, titleRect);
	}
	else {
		backgroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, TextureDicts[TitleTextureIndex], TextureNames[TitleTextureIndex],
			menux, footerBackY, menuWidth, optionHeight, 0.0f, titleRect)
		);
	}

	drawText(std::to_string(currentoption) + " / " + std::to_string(optioncount),
	         optionsFont, menux - 0.1f, footerTextY, optionTextSize, optionTextSize, titleText, 2);

	// Options background
	if (BackgTextureIndex < 1 || BackgTextureIndex >= TextureDicts.size()) {
		optionsrect.a = optionsrectAlpha;
	}
	else {
		optionsrect.a = 0;
		auto tempoptions = optionsrect;
		tempoptions.a = 255;
		backgroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, TextureDicts[BackgTextureIndex], TextureNames[BackgTextureIndex],
			menux, (menuy + optionHeight) + (maxOptionCount * optionHeight) / 2, menuWidth, optionHeight * maxOptionCount, 0.0f, tempoptions)
		);

		// That footer thing you also see in the native menu
		if (details.size() > 0) {
			drawMenuDetails(details, footerBackY + optionHeight / 1.5f);
		}
		
	}

	// Indicators
	if (currentoption == 1) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menux, ((maxOptionCount + 1) * optionHeight + (menuy + 0.0175f)), 0.02f, 0.02f, 90.0f, titleText)
		);
	}
	else if (currentoption == optioncount) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menux, ((maxOptionCount + 1) * optionHeight + (menuy + 0.0175f)), 0.02f, 0.02f, 270.0f, titleText)
		);
	}
	else {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menux, ((maxOptionCount + 1) * optionHeight + (menuy + 0.0125f)), 0.02f, 0.02f, 270.0f, titleText)
		);
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menux, ((maxOptionCount + 1) * optionHeight + (menuy + 0.0225f)), 0.02f, 0.02f, 90.0f, titleText)
		);
	}

	for (auto f : backgroundDrawCalls) { f(); }
	for (auto f : highlightsDrawCalls) { f(); }
	for (auto f : foregroundDrawCalls) { f(); }
	backgroundDrawCalls.clear();
	highlightsDrawCalls.clear();
	foregroundDrawCalls.clear();
	details.clear();

	disableKeys();

	if (currentoption > optioncount) currentoption = optioncount;
	if (currentoption < 1) currentoption = 1;
}

void Menu::ProcessMenuNav(MenuControls *controls, std::function<void()> onMain, std::function<void()> onExit) {
	if (controls->IsKeyJustPressed(MenuControls::MenuSelect) ||
		controls->IsKeyJustPressed(MenuControls::MenuCancel) ||
		controls->IsKeyJustPressed(MenuControls::MenuUp)     ||
		controls->IsKeyJustPressed(MenuControls::MenuDown)   ||
		controls->IsKeyJustPressed(MenuControls::MenuLeft)   ||
		controls->IsKeyJustPressed(MenuControls::MenuRight)) {
		useNative = false;
	}

	if (controls->IsKeyJustPressed(MenuControls::MenuKey) || useNative && 
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, controls->ControllerButton1) &&
		CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, controls->ControllerButton2)) {
		if (menulevel == 0) {
			changeMenu("mainmenu");
			if (onMain) onMain();
		}
		else {
			CloseMenu();
			CAM::SET_CINEMATIC_BUTTON_ACTIVE(1);
			if (onExit) {
				onExit();
			}
		}
		delay = GetTickCount();
		return;
	}
	if (controls->IsKeyJustPressed(MenuControls::MenuCancel) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendCancel)) {
		if (menulevel > 0) {
			if (menulevel == 1) {
				CAM::SET_CINEMATIC_BUTTON_ACTIVE(1);
				if (onExit) {
					onExit();
				}
			}
			backMenu();

		}
		delay = GetTickCount();
	}
	if (controls->IsKeyJustPressed(MenuControls::MenuSelect) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendAccept)) {
		if (menulevel > 0) {
			menuBeep();
		}
		optionpress = true;
		delay = GetTickCount();
	}
	if (controls->IsKeyPressed(MenuControls::MenuDown) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendDown)) {
		nextOption();
		delay = GetTickCount();
		downpress = true;
	}
	if (controls->IsKeyPressed(MenuControls::MenuUp) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendUp)) {
		previousOption();
		delay = GetTickCount();
		uppress = true;
	}
	if (controls->IsKeyPressed(MenuControls::MenuLeft) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneLeft)) {
		if (menulevel > 0) {
			menuBeep();
		}
		leftpress = true;
		delay = GetTickCount();
	}
	if (controls->IsKeyPressed(MenuControls::MenuRight) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight)) {
		if (menulevel > 0) {
			menuBeep();
		}
		rightpress = true;
		delay = GetTickCount();
	}
}

void Menu::CheckKeys(MenuControls* controls, std::function<void() > onMain, std::function<void() > onExit) {
	controls->Update();
	optionpress = false;

	if (GetTickCount() - delay > menuTime ||
		controls->IsKeyJustPressed(MenuControls::MenuKey)    ||
		controls->IsKeyJustPressed(MenuControls::MenuSelect) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendAccept) ||
		controls->IsKeyJustPressed(MenuControls::MenuCancel) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendCancel) ||
		controls->IsKeyJustPressed(MenuControls::MenuUp)     || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendUp)     ||
		controls->IsKeyJustPressed(MenuControls::MenuDown)   || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendDown)   ||
		controls->IsKeyJustPressed(MenuControls::MenuLeft)   || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneLeft)      ||
		controls->IsKeyJustPressed(MenuControls::MenuRight)  || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneRight)) {

		ProcessMenuNav(controls, onMain, onExit);
	}

	if (controls->IsKeyJustReleased(MenuControls::MenuKey) || controls->IsKeyJustPressed(MenuControls::MenuKey) ||
		controls->IsKeyJustReleased(MenuControls::MenuSelect) || controls->IsKeyJustPressed(MenuControls::MenuSelect) ||
		controls->IsKeyJustReleased(MenuControls::MenuCancel) || controls->IsKeyJustPressed(MenuControls::MenuCancel) ||
		controls->IsKeyJustReleased(MenuControls::MenuUp) || controls->IsKeyJustPressed(MenuControls::MenuUp) ||
		controls->IsKeyJustReleased(MenuControls::MenuDown) || controls->IsKeyJustPressed(MenuControls::MenuDown) ||
		controls->IsKeyJustReleased(MenuControls::MenuLeft) || controls->IsKeyJustPressed(MenuControls::MenuLeft) ||
		controls->IsKeyJustReleased(MenuControls::MenuRight) || controls->IsKeyJustPressed(MenuControls::MenuRight) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendAccept) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendAccept) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendCancel) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendCancel) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendUp) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendUp) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendDown) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendDown) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneLeft) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneLeft) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneRight)) {
		menuTime = menuTimeRepeat;
	}

	if (controls->IsKeyDownFor(MenuControls::MenuUp, 2 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendUp, 2 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuDown, 2 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendDown, 2 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuLeft, 2 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendLeft, 2 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuRight, 2 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendRight, 2 * menuTimeRepeat)) {
		menuTime = menuTimeSlow;
	}
	if (controls->IsKeyDownFor(MenuControls::MenuUp, 3 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendUp, 3 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuDown, 3 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendDown, 3 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuLeft, 3 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendLeft, 3 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuRight, 3 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendRight, 3 * menuTimeRepeat)) {
		menuTime = menuTimeMedium;
	}
	if (controls->IsKeyDownFor(MenuControls::MenuUp, 4 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendUp, 4 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuDown, 4 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendDown, 4 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuLeft, 4 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendLeft, 4 * menuTimeRepeat) ||
		controls->IsKeyDownFor(MenuControls::MenuRight, 4 * menuTimeRepeat) || controls->IsControlDownFor(ControlFrontendRight, 4 * menuTimeRepeat)) {
		menuTime = menuTimeFast;
	}

	if (!useNative && GetTickCount() - delay > 1000) {
		useNative = true;
	}

}

void Menu::menuBeep() {
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
}

void Menu::resetButtonStates() {
	optionpress = false;
	leftpress = false;
	rightpress = false;
	uppress = false;
	downpress = false;
}

void Menu::CloseMenu() {
	while (menulevel > 0) {
		backMenu();
	}
}
}
