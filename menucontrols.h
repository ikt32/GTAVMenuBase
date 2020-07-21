#pragma once
#include <unordered_map>
#include <inc/enums.h>

namespace NativeMenu {
    class Settings;

    class MenuControls
    {
    public:
        friend Settings;

        enum ControlType {
            MenuKey = 0,
            MenuUp,
            MenuDown,
            MenuLeft,
            MenuRight,
            MenuSelect,
            MenuCancel,
            SIZEOF_ControlType
        };

        MenuControls();
        bool IsKeyPressed(ControlType control);
        bool IsKeyJustPressed(ControlType control);
        bool IsKeyJustReleased(ControlType control);
        bool IsKeyDownFor(ControlType control, unsigned long long millis);
        void Update();
        bool IsControlDownFor(eControl control, unsigned long long millis);
        static const int controlSize = SIZEOF_ControlType;
        int ControlKeys[controlSize];
        int ControllerButton1 = -1;
        int ControllerButton2 = -1;
    private:

        bool controlCurr[controlSize];
        bool controlPrev[controlSize];

        unsigned long long pressTime[controlSize];
        unsigned long long releaseTime[controlSize];

        std::unordered_map<eControl, bool> nControlCurr;
        std::unordered_map<eControl, bool> nControlPrev;

        std::unordered_map<eControl, __int64> nPressTime;
        std::unordered_map<eControl, __int64> nReleaseTime;
    };

}