#include "menusettings.h"

#include <thirdparty/simpleini/SimpleIni.h>

#include "menucontrols.h"
#include "menukeyboard.h"
#include "menu.h"

namespace NativeMenu {


	MenuSettings::MenuSettings() { }


	MenuSettings::~MenuSettings() { }

	void MenuSettings::SetFiles(const std::string &menu) {
		settingsMenuFile = menu;
	}

	void MenuSettings::ReadSettings(MenuControls *control, Menu *menuOpts) {
		CSimpleIniA settingsMenu;
		settingsMenu.SetUnicode();
		settingsMenu.LoadFile(settingsMenuFile.c_str());
		control->ControlKeys[MenuControls::ControlType::MenuKey] = str2key(settingsMenu.GetValue("MENU", "MenuKey", "VK_OEM_4"));
		control->ControlKeys[MenuControls::ControlType::MenuUp] = str2key(settingsMenu.GetValue("MENU", "MenuUp", "UP"));
		control->ControlKeys[MenuControls::ControlType::MenuDown] = str2key(settingsMenu.GetValue("MENU", "MenuDown", "DOWN"));
		control->ControlKeys[MenuControls::ControlType::MenuLeft] = str2key(settingsMenu.GetValue("MENU", "MenuLeft", "LEFT"));
		control->ControlKeys[MenuControls::ControlType::MenuRight] = str2key(settingsMenu.GetValue("MENU", "MenuRight", "RIGHT"));
		control->ControlKeys[MenuControls::ControlType::MenuSelect] = str2key(settingsMenu.GetValue("MENU", "MenuSelect", "RETURN"));
		control->ControlKeys[MenuControls::ControlType::MenuCancel] = str2key(settingsMenu.GetValue("MENU", "MenuCancel", "BACKSPACE"));

		control->ControllerButton1 = settingsMenu.GetLongValue("MENU", "ControllerButton1", -1);
		control->ControllerButton2 = settingsMenu.GetLongValue("MENU", "ControllerButton2", -1);

        menuOpts->cheatString = settingsMenu.GetValue("MENU", "CheatString", "");

#pragma warning(push)
#pragma warning(disable: 4244)
		menuOpts->menuX = settingsMenu.GetDoubleValue("MENU", "MenuX", 0.0);
		menuOpts->menuY = settingsMenu.GetDoubleValue("MENU", "MenuY", 0.0);
#pragma warning(pop)

		// Title Text
		menuOpts->titleTextColor.R =   settingsMenu.GetLongValue("Title Text",   "Red"	, 255);
		menuOpts->titleTextColor.G =   settingsMenu.GetLongValue("Title Text",   "Green", 255);
		menuOpts->titleTextColor.B =   settingsMenu.GetLongValue("Title Text",   "Blue"	, 255);
		menuOpts->titleTextColor.A =   settingsMenu.GetLongValue("Title Text",   "Alpha", 255);
		menuOpts->titleFont =	  settingsMenu.GetLongValue("Title Text",   "Font"	, 1);

		menuOpts->titleBackgroundColor.R =   settingsMenu.GetLongValue("Title Rect",   "Red"	, 255);
		menuOpts->titleBackgroundColor.G =   settingsMenu.GetLongValue("Title Rect",   "Green", 255);
		menuOpts->titleBackgroundColor.B =   settingsMenu.GetLongValue("Title Rect",   "Blue"	, 255);
		menuOpts->titleBackgroundColor.A =   settingsMenu.GetLongValue("Title Rect",   "Alpha", 255);
		
		menuOpts->optionsBackgroundSelectColor.R =	  settingsMenu.GetLongValue("Scroller",     "Red"	, 255);
		menuOpts->optionsBackgroundSelectColor.G =	  settingsMenu.GetLongValue("Scroller",     "Green", 255);
		menuOpts->optionsBackgroundSelectColor.B =	  settingsMenu.GetLongValue("Scroller",     "Blue"	, 255);
		menuOpts->optionsBackgroundSelectColor.A =	  settingsMenu.GetLongValue("Scroller",     "Alpha", 255);
		
		menuOpts->optionsTextColor.R =	  settingsMenu.GetLongValue("Options Text", "Red"	, 255);
		menuOpts->optionsTextColor.G =	  settingsMenu.GetLongValue("Options Text", "Green", 255);
		menuOpts->optionsTextColor.B =	  settingsMenu.GetLongValue("Options Text", "Blue"	, 255);
		menuOpts->optionsTextColor.A =	  settingsMenu.GetLongValue("Options Text", "Alpha", 255);
		menuOpts->optionsFont =   settingsMenu.GetLongValue("Options Text", "Font"	, 0);
		
		menuOpts->optionsBackgroundColor.R = settingsMenu.GetLongValue("Options Rect", "Red"	, 0);
		menuOpts->optionsBackgroundColor.G = settingsMenu.GetLongValue("Options Rect", "Green", 0);
		menuOpts->optionsBackgroundColor.B = settingsMenu.GetLongValue("Options Rect", "Blue"	, 0);
		menuOpts->optionsBackgroundColor.A = settingsMenu.GetLongValue("Options Rect", "Alpha", 255);
	}

	void MenuSettings::SaveSettings() {
		// Make an issue or do a PR or something when you want to have this implemented...
	}
}