#include "menusettings.h"
#include "menucontrols.h"
#include "thirdparty/simpleini/SimpleIni.h"
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
#pragma warning(push)
#pragma warning(disable: 4244)
		menuOpts->menux = settingsMenu.GetDoubleValue("MENU", "MenuX", 0.2);
		menuOpts->menuy = settingsMenu.GetDoubleValue("MENU", "MenuY", 0.125);
#pragma warning(pop)

		// Title Text
		menuOpts->titleText.r =   settingsMenu.GetLongValue("Title Text",   "Red"	, 255);
		menuOpts->titleText.g =   settingsMenu.GetLongValue("Title Text",   "Green", 255);
		menuOpts->titleText.b =   settingsMenu.GetLongValue("Title Text",   "Blue"	, 255);
		menuOpts->titleText.a =   settingsMenu.GetLongValue("Title Text",   "Alpha", 255);
		menuOpts->titleFont =	  settingsMenu.GetLongValue("Title Text",   "Font"	, 1);

		menuOpts->titleRect.r =   settingsMenu.GetLongValue("Title Rect",   "Red"	, 255);
		menuOpts->titleRect.g =   settingsMenu.GetLongValue("Title Rect",   "Green", 255);
		menuOpts->titleRect.b =   settingsMenu.GetLongValue("Title Rect",   "Blue"	, 255);
		menuOpts->titleRect.a =   settingsMenu.GetLongValue("Title Rect",   "Alpha", 255);
		
		menuOpts->scroller.r =	  settingsMenu.GetLongValue("Scroller",     "Red"	, 255);
		menuOpts->scroller.g =	  settingsMenu.GetLongValue("Scroller",     "Green", 255);
		menuOpts->scroller.b =	  settingsMenu.GetLongValue("Scroller",     "Blue"	, 255);
		menuOpts->scroller.a =	  settingsMenu.GetLongValue("Scroller",     "Alpha", 255);
		
		menuOpts->options.r =	  settingsMenu.GetLongValue("Options Text", "Red"	, 255);
		menuOpts->options.g =	  settingsMenu.GetLongValue("Options Text", "Green", 255);
		menuOpts->options.b =	  settingsMenu.GetLongValue("Options Text", "Blue"	, 255);
		menuOpts->options.a =	  settingsMenu.GetLongValue("Options Text", "Alpha", 255);
		menuOpts->optionsFont =   settingsMenu.GetLongValue("Options Text", "Font"	, 0);
		
		menuOpts->optionsrect.r = settingsMenu.GetLongValue("Options Rect", "Red"	, 0);
		menuOpts->optionsrect.g = settingsMenu.GetLongValue("Options Rect", "Green", 0);
		menuOpts->optionsrect.b = settingsMenu.GetLongValue("Options Rect", "Blue"	, 0);
		menuOpts->optionsrect.a = settingsMenu.GetLongValue("Options Rect", "Alpha", 0);
	}

	void MenuSettings::SaveSettings() {

	}
}