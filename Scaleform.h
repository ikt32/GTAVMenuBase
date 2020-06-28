// From ScriptHookVDotNet v3
#pragma once
#include <string>
#include <vector>
#include <variant>
#include <inc/natives.h>

class ScaleformArgumentTXD {
public:
    ScaleformArgumentTXD(std::string s) {
        m_txd = s;
    }
    std::string Txd() {
        return m_txd;
    }
private:
    std::string m_txd;
};

class Scaleform
{
    using ScaleArg = std::variant<std::string, int, float, double, bool, ScaleformArgumentTXD>;
public:
    Scaleform(std::string scaleformID) {
        m_handle = GRAPHICS::REQUEST_SCALEFORM_MOVIE((char *)scaleformID.c_str());
    }

    ~Scaleform() {
        if (IsLoaded()) {
            GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&m_handle);
        }
    }

    int Handle() {
        return m_handle;
    }

    bool IsValid() {
        return m_handle != 0;
    }

    bool IsLoaded() {
        return GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(m_handle);
    }

    void CallFunction(std::string function, std::vector<ScaleArg> args = {}) {
        GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_handle, (char *)function.c_str());
        for (auto arg : args) {
            if (std::holds_alternative<std::string>(arg)) {
                GRAPHICS::BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
                HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)std::get<std::string>(arg).c_str());
                GRAPHICS::END_TEXT_COMMAND_SCALEFORM_STRING();
            }
            else if (std::holds_alternative<int>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(std::get<int>(arg));
            }
            else if (std::holds_alternative<float>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(std::get<float>(arg));

            }
            else if (std::holds_alternative<double>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT((float)std::get<double>(arg));

            }
            else if (std::holds_alternative<bool>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(std::get<bool>(arg));

            }
            else if (std::holds_alternative<ScaleformArgumentTXD>(arg)) {
                GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_TEXTURE_NAME_STRING((char *)std::get<ScaleformArgumentTXD>(arg).Txd().c_str());
            }
            else {
            }
        }
        GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
    }

    void Render2D() {
        GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(m_handle, 255, 255, 255, 255, 0);
    }

    void Render2DScreenSpace(float x, float y, float width, float height) {
        GRAPHICS::DRAW_SCALEFORM_MOVIE(m_handle, x + (width / 2.0f), y + (height / 2.0f), width, height, 255, 255, 255, 255, 0);
    }

private:
    int m_handle;

};

