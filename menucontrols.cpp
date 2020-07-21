#include "menucontrols.h"

#include <inc/enums.h>
#include <inc/natives.h>

#include "menukeyboard.h"

namespace NativeMenu {
    MenuControls::MenuControls()
    : ControlKeys{}
    , controlCurr{}
    , controlPrev{}
    , pressTime{}
    , releaseTime{} {
    std::fill(controlPrev, std::end(controlPrev), false);
    std::fill(controlCurr, std::end(controlCurr), false);
    std::fill(ControlKeys, std::end(ControlKeys), -1);
}

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
        auto tNow = GetTickCount64();
        if (IsKeyJustPressed(control)) {
            pressTime[control] = tNow;
        }

        return IsKeyPressed(control) && (tNow - pressTime[control]) >= millis;
    }

    void MenuControls::Update() {
        for (int i = 0; i < SIZEOF_ControlType; i++) {
            controlPrev[i] = controlCurr[i];
            controlCurr[i] = IsKeyDown(ControlKeys[i]);
        }

        for (const auto& input : nControlCurr) {
            nControlPrev[input.first] = nControlCurr[input.first];
            nControlCurr[input.first] = PAD::IS_DISABLED_CONTROL_PRESSED(0, input.first);
        }
    }

    bool MenuControls::IsControlDownFor(eControl control, unsigned long long millis) {
        auto tNow = GetTickCount64();
        if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, control)) {
            nPressTime[control] = tNow;
        }

        return PAD::IS_DISABLED_CONTROL_PRESSED(0, control) &&
            (tNow - nPressTime[control]) >= millis;
    }
}
