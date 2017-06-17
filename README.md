# GTA V Menu   

This menu base was taken from SudoMod base once, but I changed things here and there and now I want to use it in multiple projects, so here's a repo. Full credits to the original authors!

![pic](Menus.png)

## Building

### Requirements
* [ScriptHookV SDK by Alexander Blade](http://www.dev-c.com/gtav/scripthookv/)
* [simpleini (included as submodule)](https://github.com/brofield/simpleini)

Clone this repository recursively, such that simpleini is cloned with it:  
`git clone --recursive https://github.com/E66666666/GTAVMenuBase`

You'll want to have this repo cloned to your git folder along with where you store your projects. All of my GTA V projects that use this menu, use it in the same way, by having something like `-I../../GTAVMenuBase/` and `-I../../ScriptHookV_SDK/` in their VS Project files.

Just make sure this repo folder is in your additional include files.

Mods using this menu:
* [Manual Transmission](https://github.com/E66666666/GTAVManualTransmission)
* [Addon Spawner](https://github.com/E66666666/GTAVAddonLoader)
* [VStancer](https://github.com/E66666666/GTAVStancer)

Full menu example:
* [GTAVMenuExample](https://github.com/E66666666/GTAVMenuExample)

Since native functions are used, ScriptHookV is needed. ScriptHookV should be extracted into a
`ScriptHookV_SDK` folder parallel to this repo's folder and your mod folder that uses this.

Directory structure example:

```
git
└───ScriptHookV_SDK
│	└───inc
│	└───lib
└───GTAVMenuBase
│	└───thirdparty
│		└───simpleini
└───YourProject
```

## Usage

A menu example:

```c++
/*
 * update_menu() should be called each tick.
 */
void update_menu() {
	/*
	 * Each tick, the controls are checked. If the key is hit to open
	 * or close the menu, the binded functions are called.
	 * This function has to be called for navigation to work.
	 */
	menu.CheckKeys();

	/*
	 *  You can define a menu like this. 
	 *  The main menu should always be called "mainmenu".
	 */ 
	if (menu.CurrentMenu("mainmenu")) {
		// The title is NOT optional.
		menu.Title("Menu example");

		// This is a normal option. It'll return true when "select" is presed.
		if (menu.Option("Click me!", { "This will log something to " + Paths::GetModuleNameWithoutExtension() + ".log" })) {
			showNotification("Check the logfile!");
			logger.Write("\"Click me!\" was selected!");
		}

		// This will open a submenu with the name "submenu"
		menu.MenuOption("Look, a submenu!", "submenu", { "This submenu demonstrates a few settings."});

		// Showing static information is also possible if a string vector only contains one element.
		int nothing = 0;
		menu.StringArray("Version", { DISPLAY_VERSION }, nothing, 
						 { "Thanks for checking out this menu!", "-ikt",  eGameVersionToString(getGameVersion())});
	}

	// Any submenus can have any titles. They should only need to match
	// the name used to call them.
	if (menu.CurrentMenu("submenu")) {
		menu.Title("I'm a submenu!");

		menu.BoolOption("Here's a checkbox", checkBoxStatus, { std::string("Boolean is ") + (checkBoxStatus ? "checked" : "not checked") + "." });
		menu.IntOption("Ints!", someInt, -100, 100, intStep, { "Stepsize can be changed!" });
		menu.IntOption("Int step size", intStep, 1, 100, 1, { "Stepsize can be changed!" });
		menu.FloatOption("Floats?", someFloat, -100.0f, 100.0f, floatSteps[stepChoice], { "Try holding left/right, things should speed up." });
		menu.FloatArray("Float step size", floatSteps, stepChoice, { "Something something magic!" });
		menu.StringArray("String arrays", strings, stringsPos, { "You can also show different strings" });

		menu.Option("Description info",
		{ "You can put arbitarily long texts in the description. "
		"Word wrapping magic should work! "
		"That's why this subtext is so big ;)",
		"Newlines",
		"like so."});

		// Some extra information can be shown on the right of the the menu.
		// You do need to manage newlines yourself.
		std::vector<std::string> extraInfo = {
			"There's also some additional info",
			"You can put descriptions or info here",
			"Each string is a new line",
			"The box expands by itself"
		};
		menu.OptionPlus("Look to the right!", extraInfo, std::bind(onLeft), std::bind(onRight), "Something", 
		{"You do need to manage the line splitting yourself, as it's meant for short pieces of info."});
	}

	// Finally, draw all textures.
	menu.EndMenu();
}

void main() {
	menu.SetFiles(settingsMenuFile);
	menu.RegisterOnMain(std::bind(onMain));
	menu.RegisterOnExit(std::bind(onExit));

	while (true) {
		update_game();
		update_menu();
		WAIT(0);
	}
}
```

In which `update_menu()` is in the main update loop This should be called every tick.

Required methods inside `update_menu()`:
* `CheckKeys`
  * accepts a `MenuControls` pointer and a void `std::function`, so a function is called 
  when the menu gets opened. The function can be `nullptr` if no functionality is desired.
* `EndMenu`
  * Draws all sprites that should be drawn, like backgrounds and check boxes, and draws any
  other additional information.

Required menus and items:
* Main menu
  * Check the description how to do this
* Title
  * You'll need to specify a menu title

For a more complete example (working build), check [GTAVMenuExample](https://github.com/E66666666/GTAVMenuExample).

## Input handling
`MenuControls` does input checking, so it can distinguish between key press, key being pressed, key being released. This applies both to keyboard input (`GetAsyncKeyState`) and native inputs (`IS_DISABLED_CONTROL_PRESSED`). Conflicts/delays between these two shouldn't occur as that's handled in `CheckKeys`: if a GetAsyncKeyState is detected, native controls are temporarily 
blocked. This should prevent ghosting when a menu key is assigned to the same key the game 
navigation controls are binded to.

## Details
There are two detail panes available:
* Info pane to the right
* Details pane below the menu

A menu option with the info pane on the right can be created with `OptionPlus`. You can bind
functions to left/right press, and the original `return-true-when-option-pressed` is also
still available. The pane can be filled with a string vector, where each item represents a new
line. As of now these aren't split automatically yet, so you'll need to manually manage the
line length. It's originally made to show short pieces of information.

Detail panes are available for all option types. It accepts a string vector. Each item in this
vector is automatically split up so it fits nicely within the detail pane. A newline can be
forced by adding more string items to the vector. For normal use a vector with a simple string
should be enough.

## Remarks
If you're also not using [ScriptHookVDotNet](https://github.com/crosire/scripthookvdotnet) with [NativeUI](https://github.com/Guad/NativeUI) and just want something less painful than the mess that happens in the ScriptHookV Simple Trainer example, I hope this is of some use for you.

This thing started out as something I needed for [VStancer](https://github.com/E66666666/GTAVStancer) 
where unknown modder on GTA5-Mods linked me the SudoMod menu sources. 

Good luck modding! 
