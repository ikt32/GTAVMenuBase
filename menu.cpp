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

void Menu::drawText(const std::string text, int font, float x, float y, float scalex, float scaley, rgba rgba, bool center) {
	UI::SET_TEXT_FONT(font);
	if (font == 0) { // big-ass Chalet London
		scaley *= 0.75f;
		y += 0.0025f;
	}
	UI::SET_TEXT_SCALE(scalex, scaley);
	UI::SET_TEXT_COLOUR(rgba.r, rgba.g, rgba.b, rgba.a);
	UI::SET_TEXT_CENTRE(center);
	UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(CharAdapter(text.c_str()));
	UI::END_TEXT_COMMAND_DISPLAY_TEXT(x, y);
}

void Menu::drawRect(float x, float y, float width, float height, rgba rgba) {
	GRAPHICS::DRAW_RECT(x, y, width, height, rgba.r, rgba.g, rgba.b, rgba.a);
}

void Menu::drawSprite(std::string textureDict, std::string textureName, float x, float y, float width, float height, float rotation, rgba rgba) {
	if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(CharAdapter(textureDict.c_str()))) GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(CharAdapter(textureDict.c_str()), false);
	else GRAPHICS::DRAW_SPRITE(CharAdapter(textureDict.c_str()), CharAdapter(textureName.c_str()), x, y, width, height, rotation, rgba.r, rgba.g, rgba.b, rgba.a);
}

void Menu::Title(std::string title) {
	optioncount = 0;
	drawText(title, titleFont, menux, menuy - 0.03f, 0.85f, 0.85f, titleText, true);
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

// todo; inheritance shit

bool Menu::Option(std::string option) {
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
		}
		drawText(option, optionsFont, menux - 0.1f, optiontexty, 0.5f, 0.5f, thisOption ? optionsBlack : options, false);
	}

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

void Menu::drawAdditionalInfoBox(std::vector<std::string> &extra, size_t infoLines) {
	for (int i = 0; i < infoLines; i++) {
		drawText(extra[i].c_str(), optionsFont, menux + 0.125f, i * optionHeight + (menuy + optionHeight), 0.5f, 0.5f, options, false);
		drawRect(menux + menuWidth, i * optionHeight + (menuy + 0.0515f), menuWidth, optionHeight, optionsrect);
	}
}

bool Menu::OptionPlus(std::string option, std::vector<std::string> &extra, bool *highlighted, std::function<void() > onRight, std::function<void() > onLeft) {
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
		drawAdditionalInfoBox(extra, infoLines);
	}

	if (highlighted != nullptr) {
		*highlighted = thisOption;
	}

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

bool Menu::MenuOption(std::string option, std::string menu) {
	Option(option);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	if (currentoption <= 16 && optioncount <= 16)
		drawText(">>", optionsFont, menux + 0.068f, (optioncount * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText(">>", optionsFont, menux + 0.068f, ((optioncount - (currentoption - 16)) * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);

	if (optionpress && currentoption == optioncount) {
		optionpress = false;
		changeMenu(menu);
		return true;
	}
	return false;
}

bool Menu::IntOption(std::string option, int *var, int min, int max, int step) {
	Option(option);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	if (currentoption <= 16 && optioncount <= 16)
		drawText(("<" + std::to_string(*var) + ">").c_str(), optionsFont, menux + 0.068f, (optioncount * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText(("<" + std::to_string(*var) + ">").c_str(), optionsFont, menux + 0.068f, ((optioncount - (currentoption - 16)) * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);

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

bool Menu::FloatOption(std::string option, float *var, float min, float max, float step) {
	Option(option);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	char buf[100];
	_snprintf_s(buf, sizeof(buf), "%.2f", *var);

	if (currentoption <= 16 && optioncount <= 16)
		drawText(("<" + std::string(buf) + ">").c_str(), optionsFont, menux + 0.068f, (optioncount * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText(("<" + std::string(buf) + ">").c_str(), optionsFont, menux + 0.068f, ((optioncount - (currentoption - 16)) * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);

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

bool Menu::BoolOption(std::string option, bool *b00l) {
	Option(option);
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
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", tickBoxTexture,
			menux + 0.068f, textureY, 0.03f, 0.05f, 0.0f, optionColors)
		);
	}

	if (optionpress && currentoption == optioncount) {
		*b00l ^= 1;
		return true;
	}
	return false;
}

bool Menu::BoolSpriteOption(std::string option, bool b00l, std::string category, std::string spriteOn, std::string spriteOff) {
	Option(option);
	bool thisOption = false;
	if (currentoption == optioncount) thisOption = true;
	if (currentoption <= 16 && optioncount <= 16) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, category, b00l ? spriteOn : spriteOff,
			menux + 0.068f, (optioncount * optionHeight + (menuy + 0.016f)), 0.03f, 0.05f, 0.0f, thisOption ? optionsBlack : options));
	}
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, category, b00l ? spriteOn : spriteOff,
			menux + 0.068f, ((optioncount - (currentoption - 16)) * optionHeight + (menuy + 0.016f)), 0.03f, 0.05f, 0.0f, thisOption ? optionsBlack : options));
	}
			

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

bool Menu::IntArray(std::string option, int display[], int *PlaceHolderInt) {
	Option(option);
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
		drawText(("<" + std::to_string(display[*PlaceHolderInt]) + ">").c_str(), optionsFont, menux + 0.068f, (optioncount * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText(("<" + std::to_string(display[*PlaceHolderInt]) + ">").c_str(), optionsFont, menux + 0.068f, ((optioncount - (currentoption - 16)) * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);

	if (optionpress && currentoption == optioncount)
		return true;
	return false;
}

bool Menu::FloatArray(std::string option, float display[], int *PlaceHolderInt) {
	Option(option);
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
		drawText(("<" + std::string(buf) + ">").c_str(), optionsFont, menux + 0.068f, (optioncount * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText(("<" + std::string(buf) + ">").c_str(), optionsFont, menux + 0.068f, ((optioncount - (currentoption - 16)) * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);

	if (optionpress && currentoption == optioncount)
		return true;
	return false;
}

bool Menu::StringArray(std::string option, std::vector<std::string>display, int *PlaceHolderInt) {
	Option(option);
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
	std::string leftArrow = "<";
	std::string rightArrow = ">";
	if (max == 0) {
		leftArrow = rightArrow = "";
	}
	if (currentoption <= 16 && optioncount <= 16)
		drawText((leftArrow + std::string(display[*PlaceHolderInt]) + rightArrow).c_str(), optionsFont, menux + 0.068f, (optioncount * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);
	else if ((optioncount > (currentoption - 16)) && optioncount <= currentoption)
		drawText((leftArrow + std::string(display[*PlaceHolderInt]) + rightArrow).c_str(), optionsFont, menux + 0.068f, ((optioncount - (currentoption - 16)) * optionHeight + menuy), 0.5f, 0.5f, thisOption ? optionsBlack : options, true);

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

	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlNextCamera, true);

	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlPhone, true);

	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlVehicleCinCam, true);

	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlSelectCharacterMichael, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlSelectCharacterFranklin, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlSelectCharacterTrevor, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlSelectCharacterMultiplayer, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlCharacterWheel, true);

	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlMeleeAttackLight, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlMeleeAttackHeavy, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlMeleeAttackAlternate, true);

	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlMultiplayerInfo, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlMapPointOfInterest, true);

	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlRadioWheelLeftRight, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlVehicleNextRadio, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlVehiclePrevRadio, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlVehicleDuck, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlVehicleSelectNextWeapon, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, ControlVehicleSelectPrevWeapon, true);
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

	drawText((std::to_string(currentoption) + "/" + std::to_string(optioncount)).c_str(),
				optionsFont, menux - 0.1f, footerTextY, 0.5f, 0.5f, titleText, false);

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

	disableKeys();

	if (currentoption > optioncount) currentoption = optioncount;
	if (currentoption < 1) currentoption = 1;
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

		if (controls->IsKeyJustPressed(MenuControls::MenuSelect) ||
			controls->IsKeyJustPressed(MenuControls::MenuCancel) ||
			controls->IsKeyJustPressed(MenuControls::MenuUp)     ||
			controls->IsKeyJustPressed(MenuControls::MenuDown)   ||
			controls->IsKeyJustPressed(MenuControls::MenuLeft)   ||
			controls->IsKeyJustPressed(MenuControls::MenuRight)) {
			useNative = false;
		}

		if (controls->IsKeyJustPressed(MenuControls::MenuKey)) {
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
