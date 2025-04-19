// From ScriptHookVDotNet v3
#pragma once
#include <string>
#include <vector>
#include <variant>
#include <inc/natives.h>

class CScaleformArgumentTxd {
public:
    CScaleformArgumentTxd(const std::string& s)
        : mTxd(s)
    {
    }
    std::string Txd() const {
        return mTxd;
    }
private:
    std::string mTxd;
};

class CScaleform
{
    using TScaleArg = std::variant<std::string, int, float, double, bool, CScaleformArgumentTxd>;
public:
    CScaleform(const std::string& scaleformId)
        : mId(scaleformId)
    {
    }

    ~CScaleform() {
        Deinit();
    }

    void Init() {
        if (!mHandle)
            mHandle = GRAPHICS::REQUEST_SCALEFORM_MOVIE(mId.c_str());
    }

    void Deinit() {
        if (mHandle) {
            GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&mHandle);
            mHandle = 0;
        }
    }

    int Handle() const {
        return mHandle;
    }

    bool Initialized() {
        return GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(mHandle);
    }

    void CallFunction(std::string function, const std::vector<TScaleArg>& args = {}) {
        GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(mHandle, function.c_str());
        for (const auto& arg : args) {
            if (std::holds_alternative<std::string>(arg)) {
                GRAPHICS::BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
                HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(std::get<std::string>(arg).c_str());
                GRAPHICS::END_TEXT_COMMAND_SCALEFORM_STRING();
            }
            else if (std::holds_alternative<int>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(std::get<int>(arg));
            }
            else if (std::holds_alternative<float>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(std::get<float>(arg));
            }
            else if (std::holds_alternative<double>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(static_cast<float>(std::get<double>(arg)));
            }
            else if (std::holds_alternative<bool>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(std::get<bool>(arg));
            }
            else if (std::holds_alternative<CScaleformArgumentTxd>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_TEXTURE_NAME_STRING(std::get<CScaleformArgumentTxd>(arg).Txd().c_str());
            }
            else {
            }
        }
        GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
    }

    void Render2D() {
        GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(mHandle, 255, 255, 255, 255, 0);
    }

    void Render2DScreenSpace(float x, float y, float width, float height) {
        GRAPHICS::DRAW_SCALEFORM_MOVIE(mHandle, { x + (width / 2.0f), y + (height / 2.0f) }, width, height, 255, 255, 255, 255, 0);
    }

private:
    std::string mId;
    int mHandle = 0;
};
