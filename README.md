# GTA V Menu   

This menu base was taken from SudoMod base once, but I changed things here and there and now I want to use it in multiple projects so it'll just exist here or whatever. Full credits to those people!

![pic](Menus.png)

## Building

### Requirements
* [ScriptHookV SDK by Alexander Blade](http://www.dev-c.com/gtav/scripthookv/)

You'll wanna have this repo cloned to your git folder along with where you store your projects. All of my GTA V projects that use a menu (this menu) consume it in the same way, by having something like `-I../../GTAVMenuBase/` and `-I../../ScriptHookV_SDK/` in their VS Project files.

* [Manual Transmission](https://github.com/E66666666/GTAVManualTransmission)
* [Addon Spawner](https://github.com/E66666666/GTAVAddonLoader)
* [VStancer](https://github.com/E66666666/GTAVStancer)
* [Real Time Handling Editor](https://github.com/E66666666/GTAVHandlingEditor)

Since native functions are used, ScriptHookV is needed.

## Usage

Nifty guys, who made this SudoMod menu base. The menu is (imo) rather easy to use. Just start with checking your keys, then your menus and submenus, and finally, tell the menu to draw stuff.

A simple example would be this:

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
    // You do need to manage newlines yourself.
    std::vector<std::string> extraInfo = {
      "There's also some additional info",
      "You can put descriptions or info here",
      "Each string is a new line",
      "The box expands by itself"
    };
    menu.OptionPlus("Look to the right!", extraInfo);
  }

  // Finally, draw all textures.
  menu.EndMenu();
}
```

In which update_menu() is in the main update loopy thing. `CheckKeys` accepts a `MenuControls` pointer and a void `std::function` so stuff can happen when the menu thinks it gets opened.

`MenuControls` does input check things so it can distinguish between key press, key being pressed, key being released. This applies both to keyboard input (`GetAsyncKeyState`) and native inputs (`IS_DISABLED_CONTROL_PRESSED`). Conflicts/delays between these two shouldn't occur as that's handled in `CheckKeys`, but I don't trust myself.

If you're also not using [ScriptHookVDotNet](https://github.com/crosire/scripthookvdotnet) with [NativeUI](https://github.com/Guad/NativeUI) and just want something less painful than the mess that happens in the ScriptHookV Simple Trainer example, I hope this is of some use for you.

Good luck! 
