/* MenuClass.cpp */

#include "menu.h"


#include "inc/natives.h"
#include "inc/enums.h"
#include "menucontrols.h"
#include "menuutils.h"
#include "Util/Versions.h"
#include <locale>

namespace NativeMenu {

Menu::Menu() { }

Menu::~Menu() { }

void Menu::SetFiles(const std::string &fileName) {
	settings.SetFiles(fileName);
}

void Menu::ReadSettings() {
	settings.ReadSettings(&controls, this);
}

void Menu::RegisterOnMain(std::function<void()> onMain) {
	this->onMain = onMain;
}

void Menu::RegisterOnExit(std::function<void()> onExit) {
	this->onExit = onExit;
}

bool Menu::CurrentMenu(std::string menuname) {
	if (menuname == actualmenu) return true;
	return false;
}

void Menu::Title(std::string title) {
	optioncount = 0;
	totalHeight = 0.0f;
	float titletexty = menuY + totalHeight + titleTextOffset;
	float titley = menuY + totalHeight + titleTextureOffset;

	drawText(title, titleFont, menuX, titletexty, titleTextSize, titleTextSize, titleTextColor, 0);
	backgroundDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, textureDicts[titleTextureIndex], textureNames[titleTextureIndex], 
		menuX, titley, menuWidth, titleHeight, 0.0f, titleBackgroundColor)
	);
	totalHeight = titleHeight;
	headerHeight = titleHeight;
}

void Menu::Subtitle(std::string subtitle, bool allcaps) {
	if (allcaps)
		subtitle = makeCaps(subtitle);
	
	float subtitleY = subtitleTextureOffset + menuY + totalHeight;
	float subtitleTextY = menuY + totalHeight;
	drawRect(menuX, subtitleY, menuWidth, subtitleHeight, { 0, 0, 0, 255 } );
	drawText(subtitle, optionsFont, (menuX - menuWidth / 2.0f) + menuTextMargin, subtitleTextY, subtitleTextSize, subtitleTextSize, titleTextColor);
	totalHeight += subtitleHeight;
	headerHeight += subtitleHeight;
}

bool Menu::Option(std::string option, std::vector<std::string> details) {
	optioncount++;

	bool highlighted = currentoption == optioncount;
	

	bool visible = false;
	float optiony;
	float optiontexty;

	if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
		visible = true;
		optiontexty = menuY + totalHeight;
		optiony = optiontexty + optionTextureOffset;
	}
	else if (optioncount > currentoption - maxDisplay && optioncount <= currentoption) {
		visible = true;
		optiontexty = menuY + headerHeight + (optioncount - (currentoption - maxDisplay + 1)) * optionHeight;
		optiony = optiontexty + optionTextureOffset;
	}

	if (visible) {
		drawText(option, optionsFont, (menuX - menuWidth / 2.0f) + menuTextMargin, optiontexty, optionTextSize, optionTextSize, highlighted ? optionsTextSelectColor : optionsTextColor);
		if (highlighted) {
			highlightsDrawCalls.push_back(
				std::bind(&Menu::drawSprite, this, textureDicts[highlTextureIndex], textureNames[highlTextureIndex],
				menuX, optiony, menuWidth, optionHeight, 0.0f, optionsBackgroundSelectColor)
			);
			
			if (details.size() > 0) {
				this->details = details;
			}
		}
	}
	
	totalHeight += optionHeight;
	if (optionpress && currentoption == optioncount) return true;
	return false;
}

bool Menu::MenuOption(std::string option, std::string menu, std::vector<std::string> details) {
	Option(option, details);
	float indicatorHeight = totalHeight - optionHeight; // why the hell was this menu designed like *this*?
	bool highlighted = currentoption == optioncount;

	if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
		drawText(">>", optionsFont, 
				 menuX + menuWidth / 2.0f - optionRightMargin, 
				 indicatorHeight + menuY, 
				 optionTextSize, optionTextSize, 
				 highlighted ? optionsTextSelectColor : optionsTextColor, 2);
	}
	else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption) {
		drawText(">>", optionsFont,
				 menuX + menuWidth / 2.0f - optionRightMargin, 
				 menuY + headerHeight + (optioncount - (currentoption - maxDisplay + 1)) * optionHeight,
				 optionTextSize, optionTextSize, 
				 highlighted ? optionsTextSelectColor : optionsTextColor, 2);
	}

	if (optionpress && currentoption == optioncount) {
		optionpress = false;
		changeMenu(menu);
		return true;
	}
	return false;
}

bool Menu::OptionPlus(std::string option, std::vector<std::string> &extra,
					  std::function<void() > onRight, std::function<void() > onLeft, 
					  std::string title, std::vector<std::string> details) {
	Option(option, details);
	float indicatorHeight = totalHeight - optionHeight;
	size_t infoLines = extra.size();
	bool highlighted = currentoption == optioncount;
	if (currentoption == optioncount) {
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

	if (highlighted && ((currentoption <= maxDisplay && optioncount <= maxDisplay) ||
		((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption))) {
		drawAdditionalInfoBox(extra, infoLines, title);
	}

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

bool Menu::OptionPlus(std::string option, std::vector<std::string> &extra, bool *_highlighted,
					  std::function<void() > onRight, std::function<void() > onLeft,
					  std::string title, std::vector<std::string> details) {
	bool highlighted = currentoption == optioncount;
	if (_highlighted != nullptr) {
		*_highlighted = highlighted;
	}
	return OptionPlus(option, extra, onRight, onLeft, title, details);
}

bool Menu::IntOption(std::string option, int &var, int min, int max, int step, std::vector<std::string> details) {
	std::string printVar = std::to_string(var);

	Option(option, details);
	bool highlighted = currentoption == optioncount;

	drawOptionValue(printVar, highlighted, max - min);
	return processOptionItemControls(var, min, max, step);
}

bool Menu::FloatOption(std::string option, float &var, float min, float max, float step, std::vector<std::string> details) {
	unsigned precision = behindDec(step);
	if (precision < 2) precision = 2;
	if (precision > 6) precision = 6;

	char buf[100];
	_snprintf_s(buf, sizeof(buf), "%.*f", precision, var);
	std::string printVar = buf;
	int items = min != max ? 1 : 0;

	Option(option, details);
	bool highlighted = currentoption == optioncount;
	
	drawOptionValue(printVar, highlighted, items);
	return processOptionItemControls(var, min, max, step);
}

bool Menu::BoolOption(std::string option, bool &var, std::vector<std::string> details) {
	Option(option, details);
	float indicatorHeight = totalHeight - optionHeight;
	bool highlighted = currentoption == optioncount;
	
	char *tickBoxTexture;
	Color optionColors = optionsTextColor;
	float boxSz = 0.05f;

	if (highlighted) {
		tickBoxTexture = var ? "shop_box_tickb" : "shop_box_blankb";
	}
	else {
		tickBoxTexture = var ? "shop_box_tick" : "shop_box_blank";
	}

	bool doDraw = false;
	float textureY;
	
	if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
		doDraw = true;
		textureY = (indicatorHeight + (menuY + 0.016f));
	}
	else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption) {
		doDraw = true;
		textureY = menuY + headerHeight + (optioncount - (currentoption - maxDisplay + 1)) * optionHeight + 0.016f;
	}

	if (doDraw) {
		int resX, resY;
		GRAPHICS::_GET_ACTIVE_SCREEN_RESOLUTION(&resX, &resY);
		float ratio = (float)resX / (float)resY;
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", tickBoxTexture,
			menuX + menuWidth/2.0f - optionRightMargin, textureY, boxSz/ratio, boxSz, 0.0f, optionColors)
		);
	}

	if (optionpress && currentoption == optioncount) {
		var ^= 1;
		return true;
	}
	return false;
}

bool Menu::BoolSpriteOption(std::string option, bool enabled, std::string category, std::string spriteOn, std::string spriteOff, std::vector<std::string> details) {
	Option(option, details);
	float indicatorHeight = totalHeight - optionHeight;
	bool highlighted = currentoption == optioncount;
	
	if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, category, enabled ? spriteOn : spriteOff,
			menuX + menuWidth/2.0f - optionRightMargin, 
			(indicatorHeight + (menuY + 0.016f)), 
			0.03f, 0.05f, 0.0f, highlighted ? optionsTextSelectColor : optionsTextColor));
	}
	else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, category, enabled ? spriteOn : spriteOff,
			menuX + menuWidth/2.0f - optionRightMargin, 
			menuY + headerHeight + (optioncount - (currentoption - maxDisplay + 1)) * optionHeight + 0.016f,
			0.03f, 0.05f, 0.0f, highlighted ? optionsTextSelectColor : optionsTextColor));
	}

	if (optionpress && currentoption == optioncount) return true;
	return false;
}

bool Menu::IntArray(std::string option, std::vector<int> display, int &iterator, std::vector<std::string> details) {
	std::string printVar = std::to_string(display[iterator]);
	
	Option(option, details);
	bool highlighted = currentoption == optioncount;
	
	int min = 0;
	int max = static_cast<int>(display.size());
	
	drawOptionValue(printVar, highlighted, max);
	return processOptionItemControls(iterator, min, max, 1);
}

// TODO: Refactor first part since Arrays are similar?
// TODO: Refactor second part since FloatOption is similar?
bool Menu::FloatArray(std::string option, std::vector<float> display, int &iterator, std::vector<std::string> details) {
	Option(option, details);
	bool highlighted = currentoption == optioncount;
	int min = 0;
	int max = static_cast<int>(display.size()) - 1;

	if (iterator > display.size() || iterator < 0) {
		drawOptionValue("error", highlighted, max);
		return false;
	}

	unsigned precision = behindDec(display[iterator]);
	if (precision < 2) precision = 2;
	if (precision > 6) precision = 6;

	char buf[100];
	_snprintf_s(buf, sizeof(buf), "%.*f", precision, display[iterator]);
	std::string printVar = buf;

	drawOptionValue(printVar, highlighted, max);
	return processOptionItemControls(iterator, min, max, 1);
}

bool Menu::StringArray(std::string option, std::vector<std::string>display, int &iterator, std::vector<std::string> details) {
	Option(option, details);
	bool highlighted = currentoption == optioncount; 
	int min = 0;
	int max = static_cast<int>(display.size()) - 1;

	if (iterator > display.size() || iterator < 0) {
		drawOptionValue("error", highlighted, max);
		return false;
	}

	std::string printVar = display[iterator];
	drawOptionValue(printVar, highlighted, max);
	return processOptionItemControls(iterator, min, max, 1);
}

void Menu::EndMenu() {
	if (menulevel < 1)
		return;

	int maxOptionCount = optioncount >  maxDisplay ?  maxDisplay : optioncount;

	float footerTextY;
	float footerBackY;

	if (optioncount > maxDisplay) {
		footerTextY = maxDisplay * optionHeight + menuY + headerHeight;
	}
	else {
		footerTextY = totalHeight + menuY;
	}
	footerBackY = footerTextY + optionTextureOffset;


	// Footer
	backgroundDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, textureDicts[titleTextureIndex], textureNames[titleTextureIndex],
		menuX, footerBackY, menuWidth, optionHeight, 0.0f, titleBackgroundColor)
	);
	

	drawText(std::to_string(currentoption) + " / " + std::to_string(optioncount),
			 optionsFont, menuX - 0.1f, footerTextY, optionTextSize, optionTextSize, titleTextColor, 2);

	// Options background
	backgroundDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, textureDicts[backgTextureIndex], textureNames[backgTextureIndex], 
		menuX, 
		menuY + headerHeight + maxOptionCount * optionHeight / 2,
		menuWidth, 
		optionHeight * maxOptionCount, 0.0f, optionsBackgroundColor)
	);

	// Menu detail box
	if (details.size() > 0) {
		drawMenuDetails(details, footerBackY + optionHeight / 1.5f);
	}

	// Indicators
	if (currentoption == 1) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menuX, 
			(footerTextY + 0.0175f),
			0.02f, 0.02f, 90.0f, titleTextColor)
		);
	}
	else if (currentoption == optioncount) {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menuX, 
			(footerTextY + 0.0175f),
			0.02f, 0.02f, 270.0f, titleTextColor)
		);
	}
	else {
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menuX, 
			(footerTextY + 0.0125f),
			0.02f, 0.02f, 270.0f, titleTextColor)
		);
		foregroundDrawCalls.push_back(
			std::bind(&Menu::drawSprite, this, "commonmenu", "arrowright",
			menuX, 
			(footerTextY + 0.0225f),
			0.02f, 0.02f, 90.0f, titleTextColor)
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

void Menu::CheckKeys() {
	controls.Update();
	optionpress = false;

	if (GetTickCount() - delay > menuTime ||
		controls.IsKeyJustPressed(MenuControls::MenuKey) ||
		controls.IsKeyJustPressed(MenuControls::MenuSelect) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendAccept) ||
		controls.IsKeyJustPressed(MenuControls::MenuCancel) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendCancel) ||
		controls.IsKeyJustPressed(MenuControls::MenuUp) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendUp) ||
		controls.IsKeyJustPressed(MenuControls::MenuDown) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendDown) ||
		controls.IsKeyJustPressed(MenuControls::MenuLeft) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneLeft) ||
		controls.IsKeyJustPressed(MenuControls::MenuRight) || useNative && CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneRight)) {

		processMenuNav(onMain, onExit);
	}

	if (controls.IsKeyJustReleased(MenuControls::MenuKey) || controls.IsKeyJustPressed(MenuControls::MenuKey) ||
		controls.IsKeyJustReleased(MenuControls::MenuSelect) || controls.IsKeyJustPressed(MenuControls::MenuSelect) ||
		controls.IsKeyJustReleased(MenuControls::MenuCancel) || controls.IsKeyJustPressed(MenuControls::MenuCancel) ||
		controls.IsKeyJustReleased(MenuControls::MenuUp) || controls.IsKeyJustPressed(MenuControls::MenuUp) ||
		controls.IsKeyJustReleased(MenuControls::MenuDown) || controls.IsKeyJustPressed(MenuControls::MenuDown) ||
		controls.IsKeyJustReleased(MenuControls::MenuLeft) || controls.IsKeyJustPressed(MenuControls::MenuLeft) ||
		controls.IsKeyJustReleased(MenuControls::MenuRight) || controls.IsKeyJustPressed(MenuControls::MenuRight) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendAccept) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendAccept) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendCancel) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendCancel) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendUp) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendUp) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendDown) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendDown) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneLeft) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneLeft) ||
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight) || CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneRight)) {
		menuTime = menuTimeRepeat;
	}

	for (int i = 1; i < menuTimeDelays.size(); i++) {
		if (controls.IsKeyDownFor(MenuControls::MenuUp, (i+1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendUp, (i + 1) * menuTimeRepeat) ||
			controls.IsKeyDownFor(MenuControls::MenuDown, (i + 1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendDown, (i + 1) * menuTimeRepeat) ||
			controls.IsKeyDownFor(MenuControls::MenuLeft, (i + 1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendLeft, (i + 1) * menuTimeRepeat) ||
			controls.IsKeyDownFor(MenuControls::MenuRight, (i + 1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendRight, (i + 1) * menuTimeRepeat)) {
			menuTime = menuTimeDelays[i]; 
		}
	}


	if (!useNative && GetTickCount() - delay > 1000) {
		useNative = true;
	}

}

void Menu::CloseMenu() {
	while (menulevel > 0) {
		backMenu();
	}
	if (onExit) onExit();
}

const MenuControls &Menu::GetControls() {
	return controls;
}

float Menu::getStringWidth(std::string text) {
	float scale = optionTextSize;
	if (optionsFont == 0) { // big-ass Chalet London
		scale *= 0.75f;
	}

	UI::_SET_TEXT_ENTRY_FOR_WIDTH("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(CharAdapter(text));
	return UI::_GET_TEXT_SCREEN_WIDTH(optionsFont) * scale;
}

std::vector<std::string> Menu::splitString(float maxWidth, std::string &details) {
	std::vector<std::string> splitLines;

	std::vector<std::string> words = split(details, ' ');

	std::string line;
	for (std::string word : words) {
		float lineWidth = getStringWidth(line);
		float wordWidth = getStringWidth(word);
		if (lineWidth + wordWidth > maxWidth) {
			splitLines.push_back(line);
			line.clear();
		}
		line += word + ' ';
		if (word == words.back()) {
			splitLines.push_back(line);
		}
	}

	return splitLines;
}

void Menu::drawText(const std::string text, int font, float x, float y, float pUnknown, float scale, Color color, int justify) {
	// justify: 0 - center, 1 - left, 2 - right
	if (justify == 2) {
		UI::SET_TEXT_WRAP(menuX - menuWidth / 2, menuX + menuWidth / 2 - optionRightMargin / 2.0f);
	}
	UI::SET_TEXT_JUSTIFICATION(justify);

	UI::SET_TEXT_FONT(font);
	if (font == 0) { // big-ass Chalet London
		scale *= 0.75f;
		y += 0.003f;
	}
	UI::SET_TEXT_SCALE(0.0f, scale);
	UI::SET_TEXT_COLOUR(color.R, color.G, color.B, color.A);
	UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(CharAdapter(text));
	UI::END_TEXT_COMMAND_DISPLAY_TEXT(x, y);
}

void Menu::drawRect(float x, float y, float width, float height, Color color) {
	GRAPHICS::DRAW_RECT(x, y, width, height, color.R, color.G, color.B, color.A);
}

void Menu::drawSprite(std::string textureDict, std::string textureName, float x, float y, float width, float height, float rotation, Color color) {
	if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(CharAdapter(textureDict))) GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(CharAdapter(textureDict), false);
	else GRAPHICS::DRAW_SPRITE(CharAdapter(textureDict), CharAdapter(textureName), x, y, width, height, rotation, color.R, color.G, color.B, color.A);
}

void Menu::drawAdditionalInfoBoxTitle(std::string title) {
	float extrax = menuX + menuWidth;

	float titletexty = menuY + titleTextOffset;
	float titley = menuY + titleTextureOffset;

	drawText(title, titleFont, extrax, titletexty, titleTextSize, titleTextSize, titleTextColor, 0);

	backgroundDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, textureDicts[titleTextureIndex], textureNames[titleTextureIndex],
		extrax, titley, menuWidth, titleHeight, 180.0f, titleBackgroundColor)
	);
	
}

void Menu::drawAdditionalInfoBox(std::vector<std::string> &extra, size_t infoLines, std::string title) {
	float extrax = menuX + menuWidth;
	drawAdditionalInfoBoxTitle(title);

	if (headerHeight == titleHeight + subtitleHeight) {
		float subtitleY = subtitleTextureOffset + menuY + titleHeight;
		drawRect(extrax, subtitleY, menuWidth, subtitleHeight, { 0,0,0,255 });
	}


	for (int i = 0; i < infoLines; i++) {
		drawText(extra[i], optionsFont, menuX + menuWidth / 2.0f + menuTextMargin, i * optionHeight + (menuY + headerHeight), optionTextSize, optionTextSize, optionsTextColor);
	}

	highlightsDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, textureDicts[backgTextureIndex], textureNames[backgTextureIndex],
		extrax, (menuY + headerHeight) + (infoLines * optionHeight) / 2, menuWidth, optionHeight * infoLines, 0.0f, optionsBackgroundColor)
	);
}

void Menu::drawMenuDetails(std::vector<std::string> details, float y) {
	std::vector<std::string> splitDetails;
	for (auto detailLine : details) {
		auto splitLines = splitString(menuWidth, detailLine);
		splitDetails.insert(std::end(splitDetails), std::begin(splitLines), std::end(splitLines));
	}

	for (auto i = 0; i < splitDetails.size(); i++) {
		drawText(splitDetails[i], optionsFont, (menuX - menuWidth / 2.0f) + menuTextMargin, i * detailLineHeight + y, optionTextSize, optionTextSize, optionsTextColor);
	}

	// The thin line
	drawRect(menuX, y, menuWidth, optionHeight / 16.0f, { 0,0,0,255 });

	float boxHeight = (splitDetails.size() * detailLineHeight) + (optionHeight - detailLineHeight);

	backgroundDrawCalls.push_back(
		std::bind(&Menu::drawSprite, this, textureDicts[backgTextureIndex], textureNames[backgTextureIndex],
		menuX, y + boxHeight / 2, menuWidth, boxHeight, 0.0f, optionsBackgroundColor));
}

void Menu::drawOptionValue(std::string printVar, bool highlighted, int items) {
	float indicatorHeight = totalHeight - optionHeight;

	std::string leftArrow = "< ";
	std::string rightArrow = " >";
	if (items == 0) {
		leftArrow = rightArrow = "";
	}
	if (currentoption <= maxDisplay && optioncount <= maxDisplay)
		drawText(leftArrow + printVar + rightArrow, optionsFont,
				 menuX + menuWidth / 2.0f - optionRightMargin,
				 indicatorHeight + menuY,
				 optionTextSize, optionTextSize,
				 highlighted ? optionsTextSelectColor : optionsTextColor, 2);
	else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption)
		drawText(leftArrow + printVar + rightArrow, optionsFont,
				 menuX + menuWidth / 2.0f - optionRightMargin,
				 menuY + headerHeight + (optioncount - (currentoption - maxDisplay + 1)) * optionHeight,
				 optionTextSize, optionTextSize,
				 highlighted ? optionsTextSelectColor : optionsTextColor, 2);
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

void Menu::disableKeysOnce() {
	CAM::SET_CINEMATIC_BUTTON_ACTIVE(0);
	if (getGameVersion() > G_VER_1_0_877_1_NOSTEAM) {
		*getGlobalPtr(0x42FF + 0x82) = 1;
	}
}

void Menu::enableKeysOnce() {
	CAM::SET_CINEMATIC_BUTTON_ACTIVE(1);
	if (getGameVersion() > G_VER_1_0_877_1_NOSTEAM) {
		*getGlobalPtr(0x42FF + 0x82) = 0;
	}
}

void Menu::disableKeys() {
	disableKeysOnce();

	UI::HIDE_HELP_TEXT_THIS_FRAME();
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

void Menu::processMenuNav(std::function<void()> onMain, std::function<void()> onExit) {
	if (controls.IsKeyJustPressed(MenuControls::MenuSelect) ||
		controls.IsKeyJustPressed(MenuControls::MenuCancel) ||
		controls.IsKeyJustPressed(MenuControls::MenuUp) ||
		controls.IsKeyJustPressed(MenuControls::MenuDown) ||
		controls.IsKeyJustPressed(MenuControls::MenuLeft) ||
		controls.IsKeyJustPressed(MenuControls::MenuRight)) {
		useNative = false;
	}

	if (controls.IsKeyJustPressed(MenuControls::MenuKey) || useNative &&
		CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, controls.ControllerButton1) &&
		CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, controls.ControllerButton2)) {
		if (menulevel == 0) {
			changeMenu("mainmenu");
			if (onMain) onMain();
		}
		else {
			CloseMenu();
			enableKeysOnce();
			if (onExit) {
				onExit();
			}
		}
		delay = GetTickCount();
		return;
	}
	if (controls.IsKeyJustPressed(MenuControls::MenuCancel) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendCancel)) {
		if (menulevel > 0) {
			if (menulevel == 1) {
				enableKeysOnce();
				if (onExit) {
					onExit();
				}
			}
			backMenu();

		}
		delay = GetTickCount();
	}
	if (controls.IsKeyJustPressed(MenuControls::MenuSelect) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendAccept)) {
		if (menulevel > 0) {
			menuBeep();
		}
		optionpress = true;
		delay = GetTickCount();
	}
	if (controls.IsKeyPressed(MenuControls::MenuDown) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendDown)) {
		nextOption();
		delay = GetTickCount();
		downpress = true;
	}
	if (controls.IsKeyPressed(MenuControls::MenuUp) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendUp)) {
		previousOption();
		delay = GetTickCount();
		uppress = true;
	}
	if (controls.IsKeyPressed(MenuControls::MenuLeft) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneLeft)) {
		if (menulevel > 0) {
			menuBeep();
		}
		leftpress = true;
		delay = GetTickCount();
	}
	if (controls.IsKeyPressed(MenuControls::MenuRight) || useNative && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight)) {
		if (menulevel > 0) {
			menuBeep();
		}
		rightpress = true;
		delay = GetTickCount();
	}
}




}
