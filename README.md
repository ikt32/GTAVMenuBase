# ScriptHookV Menu Base

This is intended as a simple menu for ScriptHookV scripts,
which mimics the looks of the game's menus.

![Comparison](MenuCompare.png)

## Building

Requirements:

* [ScriptHookV SDK by Alexander Blade](http://www.dev-c.com/gtav/scripthookv/)
  * Added such that `#include <inc/main.h>` can be found
  * Uses `natives.h` generated from [Dot.'s Native DB](https://nativedb.dotindustries.dev/natives/) (Compact vectors)
* [simpleini](https://github.com/brofield/simpleini) 4.x
  * Added such that `#include <simpleini/SimpleIni.h` can be found

Add this repository as submodule:  

```sh
git submodule add https://github.com/E66666666/GTAVMenuBase
```

Example implementations

* [Ghost Replay](https://github.com/E66666666/GTAVGhostReplay)
* [Manual Transmission](https://github.com/E66666666/GTAVManualTransmission)
* [Addon Spawner](https://github.com/E66666666/GTAVAddonLoader)
* [VStancer](https://github.com/E66666666/GTAVStancer)

Full menu example:

* [GTAVMenuExample](https://github.com/E66666666/GTAVMenuExample)

## Usage

Check [GTAVMenuExample's implementation](https://github.com/E66666666/GTAVMenuExample/blob/master/GTAVMenuExample/script.cpp) to see the source code.

`update_menu()` is the main update loop. This should be called every tick.

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

## Changes

* [commit 173a499](https://github.com/E66666666/GTAVMenuBase/commit/173a499c7b77242aeafd58d610a6bfa209571588) moves the menu to the top left. This makes `(0.0f, 0.0f)` sit flush with the safe zone
* [commit 3b37182](https://github.com/E66666666/GTAVMenuBase/commit/3b37182181e73c28439838b6107eae53a2844e03) restores the selected bool pointer requirement in OptionPlus
* [commit ec9477b](https://github.com/E66666666/GTAVMenuBase/commit/ec9477b0b203efc2fcc83e7dcce33045d2198917) removes the automatic uppercase menu subtitle argument
* [commit d7a1e72](https://github.com/E66666666/GTAVMenuBase/commit/d7a1e72756919c2499a7f7446e40bd6083c2e928) updates to C++17  for `std::any` (changed to `std::variant` in [commit a61461d](https://github.com/E66666666/GTAVMenuBase/commit/a61461d0e0506469507b6d09b7a0faff253b9d26))
* [commit d88ec66](https://github.com/E66666666/GTAVMenuBase/commit/d88ec66b30c12b07b00afaa193c3470c49c6512f) requires the implementing project to include simpleini 4.x
* [commit 3b165f0](https://github.com/E66666666/GTAVMenuBase/commit/3b165f004265b1ee79a06cb9fdacacb7806f14a4) uses compact vectors as generated by Dot.'s Native DB

## Questions/Suggestions/Discussion

Feel free to use the issues, or post in the [thread on the GTA5-Mods forums](https://forums.gta5-mods.com/topic/12090/tool-wip-c-shv-lightweight-menu-library).

## Remarks

If you're also not using [ScriptHookVDotNet](https://github.com/crosire/scripthookvdotnet) with [NativeUI](https://github.com/Guad/NativeUI) and just want something less painful than the mess that happens in the ScriptHookV Simple Trainer example, I hope this is of some use for you.

This thing started out as something I needed for [VStancer](https://github.com/E66666666/GTAVStancer)
where Unknown Modder on GTA5-Mods linked me the SudoMod menu sources.

Good luck modding!

## Credits

* DireDan for the [original implementation for sudomod](https://www.unknowncheats.me/forum/grand-theft-auto-v/200692-originbase-sudomod-1-37-a.html)
* Sudomod authors
* NativeUI authors
* alloc8or
