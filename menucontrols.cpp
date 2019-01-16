#include "menucontrols.h"

#include <inc/enums.h>
#include <inc/natives.h>

#include "menukeyboard.h"

namespace NativeMenu {
	long long milliseconds_now() { // static
		LARGE_INTEGER s_frequency; // static
		BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency); // static
		if (s_use_qpc) {
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			return (1000LL * now.QuadPart) / s_frequency.QuadPart;
		}
		return GetTickCount64();
	}

	MenuControls::MenuControls()
    : ControlKeys{}
    , controlCurr{}
    , controlPrev{}
    , pressTime{}
    , releaseTime{}
    , nControlCurr{}
    , nControlPrev{}
    , nPressTime{}
    , nReleaseTime{}{
    std::fill(controlPrev, std::end(controlPrev), false);
    std::fill(controlCurr, std::end(controlCurr), false);
    std::fill(ControlKeys, std::end(ControlKeys), -1);
}

MenuControls::~MenuControls() = default;

bool MenuControls::IsKeyPressed(ControlType control) {
		return IsKeyDown(ControlKeys[control]);
	}

	bool MenuControls::IsKeyJustPressed(ControlType control) {
	    return controlCurr[control] && !controlPrev[control];
	}

	bool MenuControls::IsKeyJustReleased(ControlType control) {
	    return !controlCurr[control] && controlPrev[control];
	}

	bool MenuControls::IsKeyDownFor(ControlType control, unsigned long long millis) {
		if (IsKeyJustPressed(control)) {
			pressTime[control] = milliseconds_now();
		}

	    return IsKeyPressed(control) && (milliseconds_now() - pressTime[control]) >= millis;
	}

	void MenuControls::Update() {
		for (int i = 0; i < SIZEOF_ControlType; i++) {
			controlPrev[i] = controlCurr[i];
			controlCurr[i] = IsKeyDown(ControlKeys[i]);
		}
		// Size of eControl
		for (unsigned i = 0; i < eControlSize; ++i) {
			nControlPrev[i] = nControlCurr[i];
			nControlCurr[i] = CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, i) != 0;
		}
	}

	bool MenuControls::IsControlDownFor(eControl control, unsigned long long millis) {
		if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, control)) {
			nPressTime[control] = milliseconds_now();
		}

	    return CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, control) &&
            (milliseconds_now() - nPressTime[control]) >= millis;
	}
}
