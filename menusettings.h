#pragma once
#include <vector>

namespace NativeMenu {
class MenuControls;
class Menu;

class MenuSettings
{
public:
	MenuSettings();
	~MenuSettings();
	void ReadSettings(MenuControls *control, Menu *menuOpts);
	void SaveSettings();
	void SetFiles(const std::string &menu);

private:
	std::string settingsMenuFile;
};
}
