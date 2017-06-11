# GTA V Menu   

This menu base was taken from SudoMod base once, but I changed things here and there and now I want to use it in multiple projects, so here's a repo. Full credits to the original authors!

![pic](Menus.png)

## Building

### Requirements
* [ScriptHookV SDK by Alexander Blade](http://www.dev-c.com/gtav/scripthookv/)

You'll wanna have this repo cloned to your git folder along with where you store your projects. All of my GTA V projects that use a menu (this menu) consume it in the same way, by having something like `-I../../GTAVMenuBase/` and `-I../../ScriptHookV_SDK/` in their VS Project files.

Just make sure this repo folder is in your additional include files.

Mods using this:
* [Manual Transmission](https://github.com/E66666666/GTAVManualTransmission)
* [Addon Spawner](https://github.com/E66666666/GTAVAddonLoader)
* [VStancer](https://github.com/E66666666/GTAVStancer)

Since native functions are used, ScriptHookV is needed. ScriptHookV should be extracted into a
`ScriptHookV_SDK` folder parallel to this repo's folder and your mod folder that uses this.

## Usage

A simple menu example:

```c++
/*
 * We need to have a controls object. Another function can fill in the public
 * members of this object, to specify custom controls.
 */
NativeMenu::MenuControls controls;

/*
 * This simple function is executed when the menu opens.
 */
void onMain() {
  logger.Write("Menu was opened");
}

/*
 * This simple function is executed when the menu closes. You can handle things
 * you temporarily stored in the menu, for example.
 */
 void onExit() {
  logger.Write("Menu was closed");
}

/*
 * update_menu() should be called each tick.
 */
void update_menu() {
  // Each tick, the controls are checked. If the key is hit to open
  // or close the menu, the binded functions are called.
  menu.CheckKeys(&controls, std::bind(onMain), std::bind(onExit));

  // You can define a menu like this. The main menu should always be
  // called "mainmenu".
  if (menu.CurrentMenu("mainmenu")) {
    // The title is NOT optional.
    menu.Title("Whoopie!");
    
    if (menu.Option("Click me!")) {
      logger.Write("Option was chosen");
    }
    // This will open a submenu with the name "submenu"
    menu.MenuOption("Look, a submenu!", "submenu");
  }
  
  // Any submenus can have any titles. They should only need to match
  // the name used to call them.
  if (menu.CurrentMenu("submenu")) {
    menu.Title("I'm a submenu!");
    
    // Some extra information can be shown on the right of the the menu.
    std::vector<std::string> extraInfo = {
      "There's also some additional info! You can put descriptions"
      " or info here. This automatically splits the lines so they"
      " fit in the menu."
    };
    menu.OptionPlus("Look to the right!", extraInfo);
  }

  // Finally, draw all textures.
  menu.EndMenu();
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

## Input handling
`MenuControls` does input checking, so it can distinguish between key press, key being pressed, key being released. This applies both to keyboard input (`GetAsyncKeyState`) and native inputs (`IS_DISABLED_CONTROL_PRESSED`). Conflicts/delays between these two shouldn't occur as that's handled in `CheckKeys`: if a GetAsyncKeyState is detected, native controls are temporarily 
blocked. This should prevent ghosting when a menu key is assigned to the same key the game 
navigation controls are binded to.

## Details
There are two detail panes available (mutually exclusive until whenever):
* Info pane to the right
* Details pane below the menu

A menu option with the info pane on the right can be created with `OptionPlus`. You can bind
functions to left/right press, and the original `return-true-when-option-pressed` is also
still available. The pane can be filled with a string vector, where each item represents a new
line. As of now these aren't split automatically yet, so you'll need to manually manage the
line length.

Detail panes are available for all option types except `OptionPlus`. The example features an
example of this: It also accepts a string vector. Due to less-than-optimal decisions in the past
and legacy reasons, it's not a string. You don't need to manage the length of your text here
though, as all items are concatenated and split into lines automatically.

If you still need to have manual line management, making the first vector item `"RAW"` will make
the menu use your string vector as it is originally (with each item on a new line).

## Remarks
If you're also not using [ScriptHookVDotNet](https://github.com/crosire/scripthookvdotnet) with [NativeUI](https://github.com/Guad/NativeUI) and just want something less painful than the mess that happens in the ScriptHookV Simple Trainer example, I hope this is of some use for you.

This thing started out as something I needed for [VStancer](https://github.com/E66666666/GTAVStancer) 
where unknown modder on GTA5-Mods linked me the SudoMod menu sources. 

Good luck! 
