/* MenuClass.cpp */

#include "menu.h"

#include <locale>
#include <utility>

#include <inc/main.h>
#include <inc/natives.h>
#include <inc/enums.h>
#include "menucontrols.h"
#include "menuutils.h"
#include "menumemutils.hpp"
#include "Scaleform.h"
#include "menukeyboard.h"

// TODO: Fixes:
//      - Reduce code duplication (titles, OptionPlus title)
//      - Handle Chalet London scaling

// TODO: Improvements:
//      - Mouse support
//      - Badges?

// TODO: Never:
//      - Re-write to OO

namespace NativeMenu {
Menu::Menu()
    : mInstructionalButtonsScaleform("instructional_buttons") {
}

void Menu::Initialize() {
    mRecordGlobal = FindRecordGlobal();
    currentmenu[0] = "reserved_nomenu";
    actualmenu = "reserved_nomenu";
}

void Menu::SetFiles(const std::string &fileName) {
    settings.SetFiles(fileName);
}

void Menu::ReadSettings() {
    settings.ReadSettings(&controls, this);
    menuX = menuX + menuWidth / 2.0f;
}

void Menu::RegisterOnMain(std::function<void()> onMain) {
    this->onMain = std::move(onMain);
}

void Menu::RegisterOnExit(std::function<void()> onExit) {
    this->onExit = std::move(onExit);
}

bool Menu::CurrentMenu(const std::string& menuname) {
    return menuname == actualmenu;
}

/*
 * Section Headers/Footers
 */
void Menu::fitTitle(std::string &title, float &newSize, float titleSize) {
    float width = getStringWidth(title, titleSize, titleFont);
    float maxWidth = menuWidth - 2.0f*menuTextMargin;
    int maxTries = 50;
    int tries = 0;
    newSize = titleSize;
    while (width > maxWidth && newSize > titleTextSize * 0.5f && tries < maxTries) {
        newSize -= 0.01f;
        width = getStringWidth(title, newSize, titleFont);
        tries++;
    }
    if (width > maxWidth) {
        auto titleLines = splitString(maxWidth, title, newSize, titleFont);
        title = "";
        for (const auto& line : titleLines) {
            if (line != titleLines.back())
                title += line + '\n';
            else
                title += line;
        }
    }
}

void Menu::Title(const std::string& title) {
    float newSize;
    std::string newTitle = title;
    fitTitle(newTitle, newSize, titleTextSize);
    Title(newTitle, textureDicts[titleTextureIndex], textureNames[titleTextureIndex], newSize);
}

void Menu::Title(const std::string& title, float customSize) {
    Title(title, textureDicts[titleTextureIndex], textureNames[titleTextureIndex], customSize);
}

void Menu::Title(const std::string& title, const std::string& dict, const std::string& texture) {
    float newSize;
    std::string newTitle = title;
    fitTitle(newTitle, newSize, titleTextSize);
    Title(newTitle, dict, texture, newSize);
}

void Menu::Title(const std::string& title, const std::string& dict, const std::string& texture, float customSize) {
    optioncount = 0;
    totalHeight = 0.0f;

    float titletexty = menuY + totalHeight + titleTextOffset + titleTextOffset * 2.0f * (titleTextSize - customSize);
    float titley = menuY + totalHeight + titleTextureOffset;
    unsigned newlines = 0;
    for (const auto& c : title) {
        if (c == '\n')
            newlines++;
    }
    for (unsigned i = 0; i < newlines; ++i) {
        titletexty -= titleHeight / 5.0f;
    }

    textDraws.push_back([=]() { drawText(title, titleFont, menuX, titletexty, customSize, customSize, titleTextColor, 0); });
    backgroundSpriteDraws.push_back(
        [=]() { drawSprite(dict, texture,
            menuX, titley, menuWidth, titleHeight, 0.0f, titleBackgroundColor); });

    totalHeight = titleHeight;
    headerHeight = titleHeight;
}

void Menu::Title(const std::string& title, int textureHandle) {
    float newSize;
    std::string newTitle = title;
    fitTitle(newTitle, newSize, titleTextSize);
    Title(newTitle, textureHandle, newSize);
}

void Menu::Title(const std::string& title, int textureHandle, float customSize) {
    optioncount = 0;
    totalHeight = 0.0f;

    float titletexty = menuY + totalHeight + titleTextOffset + titleTextOffset * 2.0f * (titleTextSize - customSize);
    float titley = menuY + totalHeight + titleTextureOffset;
    unsigned newlines = 0;
    for (const auto& c : title) {
        if (c == '\n')
            newlines++;
    }
    for (unsigned i = 0; i < newlines; ++i) {
        titletexty -= titleHeight / 5.0f;
    }

    textDraws.push_back(
        [=]() { drawText(title, titleFont, menuX, titletexty, customSize, customSize, titleTextColor, 0); });

    float safeZone = GRAPHICS::GET_SAFE_ZONE_SIZE();
    float safeOffset = (1.0f - safeZone) * 0.5f;
    float safeOffsetX = safeOffset;

    float titleX = menuX;
    float ar = GRAPHICS::GET_ASPECT_RATIO(FALSE);
    float ar_true = GRAPHICS::GET_ASPECT_RATIO(TRUE);

    // game allows max 16/9 ratio for UI elements
    if (ar > 16.0f / 9.0f) {
        titleX += (ar - 16.0f / 9.0f) / (2.0f * ar);
    }

    float drawWidth = menuWidth;
    // handle multi-monitor setups
    if (ar_true > ar) {
        if (ar > 16.0f / 9.0f) {
            titleX -= (ar - 16.0f / 9.0f) / (2.0f * ar);
        }

        titleX /= ar_true / ar;
        titleX += ar / ar_true;

        drawWidth *= ar / ar_true;
        safeOffsetX *= ar / ar_true;
    }

    // We don't worry about depth since SHV draws these on top of the game anyway
    drawTexture(textureHandle, 0, -9999, 60,                                     // handle, index, depth, time
        drawWidth, titleHeight / GRAPHICS::GET_ASPECT_RATIO(FALSE), 0.5f, 0.5f, // width, height, origin x, origin y
        titleX + safeOffsetX, titley + safeOffset, 0.0f, GRAPHICS::GET_ASPECT_RATIO(FALSE), 1.0f, 1.0f, 1.0f, 1.0f);
    
    totalHeight = titleHeight;
    headerHeight = titleHeight;
}

void Menu::Subtitle(const std::string& subtitle) {    
    float subtitleY = subtitleTextureOffset + menuY + totalHeight;
    float subtitleTextY = menuY + totalHeight;
    textDraws.push_back(
        [=]() { drawText(subtitle, optionsFont, (menuX - menuWidth / 2.0f) + menuTextMargin, 
                  subtitleTextY, subtitleTextSize, subtitleTextSize, titleTextColor, 1); }
    );

    backgroundRectDraws.push_back(
        [=]() { drawRect(
        menuX, subtitleY, menuWidth, subtitleHeight, solidBlack); }
    );

    totalHeight += subtitleHeight;
    headerHeight += subtitleHeight;
}

void Menu::Footer(Color color) {
    footerType = FooterType::Color;
    footerColor = color;
}

void Menu::Footer(const std::string& dict, const std::string& texture) {
    footerType = FooterType::Sprite;
    footerSprite.Dictionary = dict;
    footerSprite.Name = texture;
}

/*
 * Section Options
 */
bool Menu::Option(const std::string& option, const std::vector<std::string>& details) {
    return Option(option, optionsBackgroundSelectColor, details);
}

bool Menu::Option(const std::string& option, Color highlight, const std::vector<std::string>& details) {
    optioncount++;

    bool highlighted = currentoption == optioncount;


    bool visible = false;
    float optiony;
    float optiontexty;

    if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
        visible = true;
        optiontexty = menuY + totalHeight;
        optiony = optiontexty + optionTextureOffset;
    }
    else if (optioncount > currentoption - maxDisplay && optioncount <= currentoption) {
        visible = true;
        optiontexty = menuY + headerHeight + 
            (optioncount - (currentoption - maxDisplay + 1)) * optionHeight;
        optiony = optiontexty + optionTextureOffset;
    }

    if (visible) {
        const float chaletLondonMult = optionsFont == 0 ? 0.75f : 1.0f;
        bool appendDots = false;
        std::string optionTrunc = option;
        while (getStringWidth(optionTrunc, optionTextSize * chaletLondonMult, optionsFont) > (menuWidth - 2.0f * menuTextMargin)) {
            optionTrunc.pop_back();
            appendDots = true;
        }
        if (appendDots) {
            optionTrunc.pop_back();
            optionTrunc.pop_back();
            optionTrunc.pop_back();
            optionTrunc += "...";
        }
        textDraws.push_back([=]() {
            drawText(optionTrunc, optionsFont,
                (menuX - menuWidth / 2.0f) + menuTextMargin, 
                optiontexty, 
                optionTextSize, optionTextSize, 
                highlighted ? optionsTextSelectColor : optionsTextColor, 1);
        });

        if (highlighted) {
            float highlightY = optiony;
            if (useSmoothScroll) {
                highlightY = lerp(oldSmoothY,
                    optiony,
                    1.0f - pow(smoothFactor, MISC::GET_FRAME_TIME()));
                oldSmoothY = highlightY;
            }
            highlightsSpriteDraws.push_back(
                [=]() { drawSprite(textureDicts[highlTextureIndex], textureNames[highlTextureIndex],
                    menuX, highlightY, menuWidth, optionHeight, 0.0f, highlight); }
            );

            if (!details.empty()) {
                this->details = details;
            }
        }
    }

    totalHeight += optionHeight;
    return optionpress && currentoption == optioncount;
}

bool Menu::MenuOption(const std::string& option, const std::string& menu, const std::vector<std::string>& details) {
    Option(option, details);
    float indicatorHeight = totalHeight - optionHeight; // why the hell was this menu designed like *this*?
    bool highlighted = currentoption == optioncount;

    if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
        textDraws.push_back(
            [=]() { drawText(
            "2", 3,
            optionRightMargin / 2.0f,//menuX + menuWidth / 2.0f - optionRightMargin,
            indicatorHeight + menuY,
            optionTextSize * 0.75f, optionTextSize * 0.75f,
            highlighted ? optionsTextSelectColor : optionsTextColor, 2
            ); });
    }
    else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption) {
        int optioncount_ = optioncount;
        textDraws.push_back(
            [=]() { drawText(
            "2", 3,
            optionRightMargin / 2.0f, //menuX + menuWidth / 2.0f - optionRightMargin,
            menuY + headerHeight + (optioncount_ - (currentoption - maxDisplay + 1)) * optionHeight,
            optionTextSize * 0.75f, optionTextSize * 0.75f,
            highlighted ? optionsTextSelectColor : optionsTextColor, 2
        ); });
    }

    if (optionpress && currentoption == optioncount) {
        optionpress = false;
        changeMenu(menu);
        return true;
    }
    return false;
}

bool Menu::OptionPlus(const std::string& option, const std::vector<std::string>& extra, bool *_highlighted,
                      const std::function<void() >& onRight, const std::function<void() >& onLeft,
                      const std::string& title, const std::vector<std::string>& details) {
    Option(option, details);
    size_t infoLines = extra.size();
    bool highlighted = currentoption == optioncount;
    if (_highlighted != nullptr) {
        *_highlighted = highlighted;
    }

    if (currentoption == optioncount) {
        if (onLeft && leftpress) {
            onLeft();
            leftpress = false;
            return false;
        }
        if (onRight && rightpress) {
            onRight();
            rightpress = false;
            return false;
        }
    }

    if (highlighted && infoLines > 0 && 
        ((currentoption <= maxDisplay && optioncount <= maxDisplay) ||
        ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption))) {
        drawOptionPlusExtras(extra, title);
    }

    return optionpress && currentoption == optioncount;
}

void Menu::OptionPlusPlus(const std::vector<std::string>& extra, const std::string& title) {
    drawOptionPlusExtras(extra, title);
}

bool Menu::UInt8Option(const std::string& option, uint8_t& var, uint8_t min, uint8_t max, uint8_t step, const std::vector<std::string>& details) {
    Option(option, details);
    std::string printVar = std::to_string(var);
    bool highlighted = currentoption == optioncount;

    drawOptionValue(printVar, highlighted, max - min);
    return processOptionItemControls(var, min, max, step);
}

bool Menu::IntOption(const std::string& option, int &var, int min, int max, int step, const std::vector<std::string>& details) {
    Option(option, details);
    std::string printVar = std::to_string(var);
    bool highlighted = currentoption == optioncount;

    drawOptionValue(printVar, highlighted, max - min);
    return processOptionItemControls(var, min, max, step);
}

bool Menu::IntOptionCb(const std::string& option, int& var, int min, int max, int step,
    const std::function<bool(int&)>& extFunc, const std::vector<std::string>& details) {
    Option(option, details);
    std::string printVar = std::to_string(var);

    bool highlighted = currentoption == optioncount;

    drawOptionValue(printVar, highlighted, max - min);
    return processOptionItemControls(var, min, max, step, extFunc);
}

bool Menu::FloatOption(const std::string& option, float &var, float min, float max, float step,
                       const std::vector<std::string>& details) {
    Option(option, details);
    unsigned precision = behindDec(step);
    if (precision < 2) precision = 2;
    if (precision > 6) precision = 6;

    char buf[100];
    _snprintf_s(buf, sizeof(buf), "%.*f", precision, var);
    std::string printVar = buf;
    int items = min != max ? 1 : 0;

    bool highlighted = currentoption == optioncount;
    
    drawOptionValue(printVar, highlighted, items);
    return processOptionItemControls(var, min, max, step);
}

bool Menu::FloatOptionCb(const std::string& option, float& var, float min, float max, float step,
                         const std::function<bool(float&)>& extFunc, const std::vector<std::string>& details) {
    Option(option, details);
    unsigned precision = behindDec(step);
    if (precision < 2) precision = 2;
    if (precision > 6) precision = 6;

    char buf[100];
    _snprintf_s(buf, sizeof(buf), "%.*f", precision, var);
    std::string printVar = buf;
    int items = min != max ? 1 : 0;

    bool highlighted = currentoption == optioncount;

    drawOptionValue(printVar, highlighted, items);
    return processOptionItemControls(var, min, max, step, extFunc);
}

bool Menu::BoolOption(const std::string& option, bool &var, const std::vector<std::string>& details) {
    Option(option, details);
    float indicatorHeight = totalHeight - optionHeight;
    bool highlighted = currentoption == optioncount;
    
    const char* tickBoxTexture;
    Color optionColors = optionsTextColor;
    float boxSz = 0.0475f;

    if (highlighted) {
        tickBoxTexture = var ? "shop_box_tickb" : "shop_box_blankb";
    }
    else {
        tickBoxTexture = var ? "shop_box_tick" : "shop_box_blank";
    }

    bool doDraw = false;
    float textureY;
    
    if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
        doDraw = true;
        textureY = (indicatorHeight + (menuY + 0.0175f));
    }
    else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption) {
        doDraw = true;
        textureY = menuY + headerHeight + (optioncount - (currentoption - maxDisplay + 1)) * optionHeight + 0.0175f;
    }

    if (doDraw) {
        int resX, resY;
        GRAPHICS::GET_ACTUAL_SCREEN_RESOLUTION(&resX, &resY);
        float ratio = static_cast<float>(resX) / static_cast<float>(resY);
        foregroundSpriteCalls.push_back(
            [=]() { drawSprite("commonmenu", tickBoxTexture,
                menuX + menuWidth / 2.0f - optionRightMargin, textureY, boxSz / ratio, boxSz, 0.0f, optionColors)
        ; });
    }

    if (optionpress && currentoption == optioncount) {
        var ^= 1;
        return true;
    }
    return false;
}

bool Menu::BoolSpriteOption(const std::string& option, bool enabled, std::string category, const std::string& spriteOn,
                            const std::string& spriteOff, const std::vector<std::string>& details) {
    Option(option, details);
    float indicatorHeight = totalHeight - optionHeight;
    bool highlighted = currentoption == optioncount;
    
    if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
        foregroundSpriteCalls.push_back(
            [=]() { drawSprite(category, enabled ? spriteOn : spriteOff,
            menuX + menuWidth/2.0f - optionRightMargin, 
            (indicatorHeight + (menuY + 0.016f)), 
            0.03f, 0.05f, 0.0f, highlighted ? optionsTextSelectColor : optionsTextColor); });
    }
    else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption) {
        int optioncount_ = optioncount;
        foregroundSpriteCalls.push_back(
            [=]() { drawSprite(category, enabled ? spriteOn : spriteOff,
            menuX + menuWidth/2.0f - optionRightMargin, 
            menuY + headerHeight + (optioncount_ - (currentoption - maxDisplay + 1)) * optionHeight + 0.016f,
            0.03f, 0.05f, 0.0f, highlighted ? optionsTextSelectColor : optionsTextColor); });
    }

    return optionpress && currentoption == optioncount;
}

bool Menu::IntArray(const std::string& option, std::vector<int> display, int &iterator, const std::vector<std::string>& details) {
    Option(option, details);
    bool highlighted = currentoption == optioncount;

    std::string printVar = std::to_string(display[iterator]);
    
    int min = 0;
    int max = static_cast<int>(display.size()) - 1;
    
    drawOptionValue(printVar, highlighted, max);
    return processOptionItemControls(iterator, min, max, 1);
}

bool Menu::FloatArray(const std::string& option, std::vector<float> display, int &iterator,
                      const std::vector<std::string>& details) {
    Option(option, details);
    bool highlighted = currentoption == optioncount;
    int min = 0;
    int max = static_cast<int>(display.size()) - 1;

    if (iterator > static_cast<int>(display.size()) || iterator < 0) {
        drawOptionValue("error", highlighted, max);
        return false;
    }

    unsigned precision = behindDec(display[iterator]);
    if (precision < 2) precision = 2;
    if (precision > 6) precision = 6;

    char buf[100];
    _snprintf_s(buf, sizeof(buf), "%.*f", precision, display[iterator]);
    std::string printVar = buf;

    drawOptionValue(printVar, highlighted, max);
    return processOptionItemControls(iterator, min, max, 1);
}

bool Menu::StringArray(const std::string& option, const std::vector<std::string>& display, int &iterator,
                       const std::vector<std::string>& details) {
    Option(option, details);
    bool highlighted = currentoption == optioncount; 
    int min = 0;
    int max = static_cast<int>(display.size()) - 1;

    if (iterator > static_cast<int>(display.size()) || iterator < 0) {
        drawOptionValue("error (" + std::to_string(iterator) + ")", highlighted, max);
        return false;
    }

    std::string printVar = display[iterator];
    drawOptionValue(printVar, highlighted, max);
    return processOptionItemControls(iterator, min, max, 1);
}


/*
 * Section Draw
 */
void Menu::drawInstructionalButtons() {
    const auto buttonSelect = std::string(PAD::GET_CONTROL_INSTRUCTIONAL_BUTTONS_STRING(2, ControlPhoneSelect, 0));
    const auto textSelect = std::string(HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION("HUD_INPUT2")); // Select
    
    const auto buttonBack = std::string(PAD::GET_CONTROL_INSTRUCTIONAL_BUTTONS_STRING(2, ControlPhoneCancel, 0));
    const auto textBack = std::string(HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION("HUD_INPUT3")); // Back

    mInstructionalButtonsScaleform.CallFunction("CLEAR_ALL");
    mInstructionalButtonsScaleform.CallFunction("TOGGLE_MOUSE_BUTTONS", { 0 });
    mInstructionalButtonsScaleform.CallFunction("CREATE_CONTAINER");
    mInstructionalButtonsScaleform.CallFunction("SET_DATA_SLOT", { 0, buttonSelect, textSelect });
    mInstructionalButtonsScaleform.CallFunction("SET_DATA_SLOT", { 1, buttonBack, textBack });

    mInstructionalButtonsScaleform.CallFunction("DRAW_INSTRUCTIONAL_BUTTONS", { -1 });
    mInstructionalButtonsScaleform.Render2D();
}

void Menu::drawMenuDetails() {
    int maxOptionCount = optioncount >  maxDisplay ?  maxDisplay : optioncount;

    float footerTextY;
    float footerBackY;

    if (optioncount > maxDisplay) {
        footerTextY = maxDisplay * optionHeight + menuY + headerHeight;
    }
    else {
        footerTextY = totalHeight + menuY;
    }
    footerBackY = footerTextY + optionTextureOffset;

    switch (footerType) {
        case FooterType::Color: {
            backgroundRectDraws.push_back(
                [=]() { drawRect(
                          menuX, footerBackY, menuWidth, optionHeight, footerColor); }
            );
            break;
        }
        case FooterType::Sprite: {
            backgroundSpriteDraws.push_back(
                [=]() { drawSprite(footerSprite.Dictionary, footerSprite.Name,
                          menuX, footerBackY, menuWidth, optionHeight, 0.0f, titleBackgroundColor); }
            );
            break;
        }
        default: {
            backgroundRectDraws.push_back(
                [=]() { drawRect(
                          menuX, footerBackY, menuWidth, optionHeight, solidBlack); }
            );
            break;
        }
    }
    
    textDraws.push_back(
        [=]() { drawText(std::to_string(currentoption) + " / " + std::to_string(optioncount),
                  optionsFont,
                  optionRightMargin / 2.0f,
                  footerTextY, optionTextSize, optionTextSize, titleTextColor, 2); }
    );

    // Options background
    backgroundSpriteDraws.push_back(
        [=]() { drawSprite(textureDicts[backgTextureIndex], textureNames[backgTextureIndex], 
                  menuX, 
                  menuY + headerHeight + maxOptionCount * optionHeight / 2,
                  menuWidth, 
                  optionHeight * maxOptionCount, 0.0f, optionsBackgroundColor); }
    );

    // Menu detail box
    if (!details.empty()) {
        drawMenuDetails(details, footerBackY + optionHeight / 1.5f);
    }

    // Indicators
    if (currentoption == 1) {
        foregroundSpriteCalls.push_back(
            [=]() { drawSprite("commonmenu", "arrowright",
                menuX,
                (footerTextY + 0.0175f),
                0.016f, 0.026f, 90.0f, titleTextColor); }
        );
    }
    else if (currentoption == optioncount) {
        foregroundSpriteCalls.push_back(
            [=]() { drawSprite("commonmenu", "arrowright",
                menuX,
                (footerTextY + 0.0175f),
                0.016f, 0.026f, 270.0f, titleTextColor); }
        );
    }
    else {
        foregroundSpriteCalls.push_back(
            [=]() { drawSprite("commonmenu", "arrowright",
                menuX,
                (footerTextY + 0.0105f),
                0.016f, 0.026f, 270.0f, titleTextColor); }
        );
        foregroundSpriteCalls.push_back(
            [=]() { drawSprite("commonmenu", "arrowright",
                menuX,
                (footerTextY + 0.0245f),
                0.016f, 0.026f, 90.0f, titleTextColor); }
        );
    }
}

void Menu::EndMenu() {
    if (menulevel < 1)
        return;

    drawMenuDetails();

    GRAPHICS::SET_SCRIPT_GFX_ALIGN('L', 'T');
    GRAPHICS::SET_SCRIPT_GFX_ALIGN_PARAMS({ 0, 0 }, 0, 0);
    for (const auto& f : backgroundSpriteDraws) { f(); }
    for (const auto& f : backgroundRectDraws)   { f(); }
    for (const auto& f : highlightsSpriteDraws) { f(); }
    for (const auto& f : foregroundSpriteCalls) { f(); }
    for (const auto& f : textDraws)             { f(); }
    GRAPHICS::RESET_SCRIPT_GFX_ALIGN();

    backgroundSpriteDraws.clear();
    backgroundRectDraws.clear();
    highlightsSpriteDraws.clear();
    foregroundSpriteCalls.clear();
    textDraws.clear();
    details.clear();
    footerType = FooterType::Default;

    hideHUDComponents();
    disableKeys();

    drawInstructionalButtons();
}

/*
 * Section Inputs
 */
void Menu::CheckKeys() {
    if (!cheatString.empty()) {
        if (MISC::HAS_PC_CHEAT_WITH_HASH_BEEN_ACTIVATED(MISC::GET_HASH_KEY((char*)cheatString.c_str()))) {
            OpenMenu();
            controls.Update();
            optionpress = false;
            return;
        }
    }
    controls.Update();
    optionpress = false;

    if (GetTickCount64() - delay > menuTime ||
        controls.IsKeyJustPressed(MenuControls::MenuKey) ||
        controls.IsKeyJustPressed(MenuControls::MenuSelect) || useNative && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneSelect) ||
        controls.IsKeyJustPressed(MenuControls::MenuCancel) || useNative && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneCancel) ||
        controls.IsKeyJustPressed(MenuControls::MenuUp) || useNative && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendUp) ||
        controls.IsKeyJustPressed(MenuControls::MenuDown) || useNative && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlFrontendDown) ||
        controls.IsKeyJustPressed(MenuControls::MenuLeft) || useNative && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneLeft) ||
        controls.IsKeyJustPressed(MenuControls::MenuRight) || useNative && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControlPhoneRight)) {
        processMenuNav();
    }

    if (controls.IsKeyJustReleased(MenuControls::MenuKey) || controls.IsKeyJustPressed(MenuControls::MenuKey) ||
        controls.IsKeyJustReleased(MenuControls::MenuSelect) || controls.IsKeyJustPressed(MenuControls::MenuSelect) ||
        controls.IsKeyJustReleased(MenuControls::MenuCancel) || controls.IsKeyJustPressed(MenuControls::MenuCancel) ||
        controls.IsKeyJustReleased(MenuControls::MenuUp) || controls.IsKeyJustPressed(MenuControls::MenuUp) ||
        controls.IsKeyJustReleased(MenuControls::MenuDown) || controls.IsKeyJustPressed(MenuControls::MenuDown) ||
        controls.IsKeyJustReleased(MenuControls::MenuLeft) || controls.IsKeyJustPressed(MenuControls::MenuLeft) ||
        controls.IsKeyJustReleased(MenuControls::MenuRight) || controls.IsKeyJustPressed(MenuControls::MenuRight) ||
        PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneSelect) || PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneSelect) ||
        PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneCancel) || PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneCancel) ||
        PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendUp) || PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendUp) ||
        PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendDown) || PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendDown) ||
        PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneLeft) || PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneLeft) ||
        PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight) || PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlPhoneRight)) {
        menuTime = menuTimeRepeat;
    }

    for (unsigned i = 1; i < menuTimeDelays.size(); ++i) {
        if (controls.IsKeyDownFor(MenuControls::MenuUp, (i+1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendUp, (i + 1) * menuTimeRepeat) ||
            controls.IsKeyDownFor(MenuControls::MenuDown, (i + 1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendDown, (i + 1) * menuTimeRepeat) ||
            controls.IsKeyDownFor(MenuControls::MenuLeft, (i + 1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendLeft, (i + 1) * menuTimeRepeat) ||
            controls.IsKeyDownFor(MenuControls::MenuRight, (i + 1) * menuTimeRepeat) || controls.IsControlDownFor(ControlFrontendRight, (i + 1) * menuTimeRepeat)) {
            menuTime = menuTimeDelays[i]; 
        }
    }


    if (!useNative && GetTickCount64() - delay > 1000) {
        useNative = true;
    }

}

void Menu::OpenMenu() {
    if (menulevel == 0) {
        changeMenu("mainmenu");
        updateScreenSize();
        if (onMain) {
            onMain();
        }
        mInstructionalButtonsScaleform.Init();
    }
}

void Menu::CloseMenu() {
    while (menulevel > 0) {
        backMenu();
    }
    if (onExit) {
        onExit();
    }
    mInstructionalButtonsScaleform.Deinit();
}

const MenuControls &Menu::GetControls() {
    return controls;
}

bool Menu::IsThisOpen() {
    return menulevel > 0;
}

void Menu::NextOption() {
    previousoption = currentoption;
    if (currentoption < optioncount)
        currentoption++;
    else
        currentoption = 1;
}

void Menu::PreviousOption() {
    previousoption = currentoption;
    if (currentoption > 1)
        currentoption--;
    else
        currentoption = optioncount;
}

/*
 * Section Draw/Utils
 */
float Menu::getStringWidth(const std::string& text, float scale, int font) {
    HUD::BEGIN_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT("STRING");
    HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
    HUD::SET_TEXT_FONT( font);
    HUD::SET_TEXT_SCALE( scale, scale);
    return HUD::END_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT(true);
}

std::vector<std::string> Menu::splitString(float maxWidth, const std::string& details, float scale, int font) {
    std::vector<std::string> splitLines;

    std::vector<std::string> words = split(details, ' ');

    std::string line;
    for (auto it = words.begin(); it != words.end(); ++it) {
        float lineWidth = getStringWidth(line, scale, font);
        float wordWidth = getStringWidth(*it, scale, font);
        if (lineWidth + wordWidth > maxWidth) {
            splitLines.push_back(line);
            line.clear();
        }
        line += *it + ' ';
        if (std::next(it) == words.end()) {
            splitLines.push_back(line);
        }
    }

    return splitLines;
}

void Menu::drawText(const std::string& text, int font, float x, float y, float pUnknown, float scale, Color color, int justify) {
    // justify: 0 - center, 1 - left, 2 - right
    // if justify == 2, treat x as right-dist?
    if (justify == 2) {
        HUD::SET_TEXT_WRAP(menuX - menuWidth / 2, menuX + menuWidth / 2 - x/* - optionRightMargin / 2.0f*/);
    }
    HUD::SET_TEXT_JUSTIFICATION(justify);

    HUD::SET_TEXT_FONT(font);
    // TODO: Handle Chalet London elsewhere
    if (font == 0) { // big-ass Chalet London
        scale *= 0.75f;
        y += 0.003f;
    }
    HUD::SET_TEXT_SCALE(0.0f, scale);
    HUD::SET_TEXT_COLOUR(color.R, color.G, color.B, color.A);
    HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
    HUD::END_TEXT_COMMAND_DISPLAY_TEXT({ x, y }, 0);
}

void Menu::drawRect(float x, float y, float width, float height, Color color) {
    GRAPHICS::DRAW_RECT({ x, y }, width, height, color.R, color.G, color.B, color.A, 0);
}

void Menu::drawSprite(const std::string& textureDict, const std::string& textureName, float x, float y, float width, 
                      float height, float rotation, Color color) {
    if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(textureDict.c_str())) {
        GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(textureDict.c_str(), false);
    }
    else {
        GRAPHICS::DRAW_SPRITE(textureDict.c_str(), textureName.c_str(), { x, y }, width, height, rotation, color.R, color.G, color.B, color.A, 0, 0);
    }
}

void Menu::drawOptionPlusTitle(const std::string& title) {
    float newSize;
    std::string newTitle = title;
    fitTitle(newTitle, newSize, titleTextSize);

    float extrax = menuX + menuWidth;

    float titletexty = menuY + titleTextOffset;
    float titley = menuY + titleTextureOffset;

    textDraws.push_back(
        [=]() { drawText(newTitle, titleFont, extrax, titletexty, newSize, newSize, titleTextColor, 0); }
    );

    backgroundSpriteDraws.push_back(
        [=]() { drawSprite(textureDicts[titleTextureIndex], textureNames[titleTextureIndex],
        extrax, titley, menuWidth, titleHeight, 180.0f, titleBackgroundColor); }
    );
    
}

void Menu::drawOptionPlusImage(const std::string& extra, float &finalHeight) {
    int imgHandle;
    int imgWidth; 
    int imgHeight;
    std::string scanFormat = ImagePrefix + "%dW%dH%d";
    int nParams = sscanf_s(extra.c_str(), scanFormat.c_str(), &imgHandle, &imgWidth, &imgHeight);
    if (nParams != 3) {
        std::string errTxt = "Format error: " + extra;
        textDraws.push_back(
            [=]() { drawText(
                      errTxt, optionsFont, menuX + menuWidth / 2.0f + menuTextMargin, finalHeight + (menuY + headerHeight), optionTextSize, optionTextSize, optionsTextColor, 1); });
        finalHeight += optionHeight;
        return;
    }
    float drawWidth = menuWidth - 2.0f * menuTextMargin;
    float drawHeight = static_cast<float>(imgHeight) * (drawWidth / static_cast<float>(imgWidth));
    float imgXpos = (menuX + menuWidth / 2.0f + menuTextMargin);
    float imgYpos = finalHeight + (menuY + headerHeight) + menuTextMargin;

    float safeZone = GRAPHICS::GET_SAFE_ZONE_SIZE();
    float safeOffset = (1.0f - safeZone) * 0.5f;
    float safeOffsetX = safeOffset;

    float ar = GRAPHICS::GET_ASPECT_RATIO(FALSE);

    // game allows max 16/9 ratio for UI elements
    if (ar > 16.0f / 9.0f) {
        imgXpos += (ar - 16.0f / 9.0f) / (2.0f * ar);
    }


    // handle multi-monitor setups
    float ar_true = GRAPHICS::GET_ASPECT_RATIO(TRUE);
    if (ar_true > ar) {
        if (ar > 16.0f / 9.0f) {
            imgXpos -= (ar - 16.0f / 9.0f) / (2.0f * ar);
        }

        imgXpos /= ar_true / ar;
        imgXpos += ar/ar_true;

        drawWidth *= ar / ar_true;
        safeOffsetX *= ar / ar_true;
    }

    drawTexture(imgHandle, 0, -9999, 60,                            // handle, index, depth, time
                drawWidth, drawHeight, 0.0f, 0.0f,                  // width, height, origin x, origin y
                imgXpos + safeOffsetX, imgYpos + safeOffset, 0.0f,   // pos x, pos y, rot
                ar, 1.0f, 1.0f, 1.0f, 1.0f);                        // screen correct, rgba
    finalHeight += drawHeight * ar + 2.0f * menuTextMargin;
}

void Menu::drawOptionPlusSprite(const std::string& extra, float &finalHeight) {
    const unsigned max_sz = 128;
    char dict[max_sz];
    char name[max_sz];
    int imgWidth;
    int imgHeight;
    std::string scanFormat = SpritePrefix + "%s %s W%dH%d";
    int nParams = sscanf_s(extra.c_str(), scanFormat.c_str(), dict, max_sz, name, max_sz, &imgWidth, &imgHeight);
    if (nParams != 4) {
        std::string errTxt = "Format error: " + extra;
        textDraws.push_back(
            [=]() { drawText(
                      errTxt, optionsFont, menuX + menuWidth / 2.0f + menuTextMargin, finalHeight + (menuY + headerHeight), optionTextSize, optionTextSize, optionsTextColor, 1); });
        finalHeight += optionHeight;
        return;
    }
    float drawWidth = menuWidth - 2.0f * menuTextMargin;
    float drawHeight = static_cast<float>(imgHeight) * (drawWidth / static_cast<float>(imgWidth)) * GRAPHICS::GET_ASPECT_RATIO(FALSE);
    float imgXpos = menuX + menuWidth / 2.0f + drawWidth / 2.0f + menuTextMargin;
    float imgYpos = finalHeight + drawHeight/2.0f + (menuY + headerHeight) + menuTextMargin;
            
    foregroundSpriteCalls.push_back(
        [=]() { drawSprite(std::string(dict), std::string(name),
                  imgXpos, imgYpos, drawWidth, drawHeight, 0.0f, titleTextColor); }
    );
            
    finalHeight += drawHeight + 2.0f * menuTextMargin;
}

void Menu::drawOptionPlusText(const std::string& extra, float &finalHeight) {
    std::vector<std::string> splitExtra;
    const float big_ass_Chalet_London_mult = optionsFont == 0 ? 0.75f : 1.0f;
    auto splitLines = splitString(menuWidth, extra, optionTextSize * big_ass_Chalet_London_mult, optionsFont);
    splitExtra.insert(std::end(splitExtra), std::begin(splitLines), std::end(splitLines));
    float heightModifier = 0.8f;
    for (size_t line = 0; line < splitExtra.size(); line++) {
        textDraws.push_back(
            [=]() { drawText(
                      splitExtra[line], optionsFont, 
                      menuX + menuWidth / 2.0f + menuTextMargin, finalHeight + (menuY + headerHeight) + line * optionHeight * heightModifier,
                      optionTextSize, optionTextSize, 
                      optionsTextColor, 1); });
    }
    finalHeight += splitExtra.size() * optionHeight * heightModifier;
}

void Menu::drawOptionPlusExtras(const std::vector<std::string>& extras, std::string title) {
    float extrax = menuX + menuWidth;
    drawOptionPlusTitle(title);

    if (headerHeight == titleHeight + subtitleHeight) {
        float subtitleY = subtitleTextureOffset + menuY + titleHeight;
        backgroundRectDraws.push_back(
            [=]() { drawRect(extrax, subtitleY, menuWidth, subtitleHeight, solidBlack); }
        );
    }

    float finalHeight = 0;

    for (const auto& extra : extras) {
        if (extra.compare(0, ImagePrefix.size(), ImagePrefix) == 0) {
            drawOptionPlusImage(extra, finalHeight);
        }
        else if (extra.compare(0, SpritePrefix.size(), SpritePrefix) == 0) {
            drawOptionPlusSprite(extra, finalHeight);
        }
        else {
            drawOptionPlusText(extra, finalHeight);
        }
    }

    backgroundSpriteDraws.push_back(
        [=]() { drawSprite(textureDicts[backgTextureIndex], textureNames[backgTextureIndex],
        extrax, menuY + headerHeight + finalHeight / 2.0f, menuWidth, finalHeight, 0.0f, optionsBackgroundColor); }
    );
}

void Menu::drawMenuDetails(const std::vector<std::string>& details, float y) {
    std::vector<std::string> splitDetails;
    const float big_ass_Chalet_London_mult = optionsFont == 0 ? 0.75f : 1.0f;
    for (const auto& detailLine : details) {
        auto splitLines = splitString(menuWidth, detailLine, optionTextSize * big_ass_Chalet_London_mult, optionsFont);
        splitDetails.insert(std::end(splitDetails), std::begin(splitLines), std::end(splitLines));
    }

    for (unsigned i = 0; i < splitDetails.size(); ++i) {
        textDraws.push_back(
            [=]() { drawText(
            splitDetails[i], optionsFont, (menuX - menuWidth / 2.0f) + menuTextMargin, i * detailLineHeight + y, optionTextSize, optionTextSize, optionsTextColor, 1); });
    }

    // The thin line
    backgroundRectDraws.push_back([=]() { drawRect(
                                  menuX, y, menuWidth, optionHeight / 16.0f, solidBlack); });

    float boxHeight = (splitDetails.size() * detailLineHeight) + (optionHeight - detailLineHeight);

    backgroundSpriteDraws.push_back(
        [=]() { drawSprite(textureDicts[backgTextureIndex], textureNames[backgTextureIndex],
        menuX, y + boxHeight / 2, menuWidth, boxHeight, 0.0f, optionsBackgroundColor); });
}

void Menu::drawOptionValue(const std::string& printVar, bool highlighted, int items) {
    float indicatorHeight = totalHeight - optionHeight;

    const float chaletLondonMult = optionsFont == 0 ? 0.75f : 1.0f;
    float textWidth = getStringWidth(printVar, optionTextSize * chaletLondonMult, optionsFont);
    const float spriteSz = 0.025f;

    float arrowBuff = 0.0f;
    if (items > 0) {
        bool doDraw = false;
        float textureY;
        if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
            doDraw = true;
            textureY = (indicatorHeight + (menuY + 0.0175f));
        }
        else if ((optioncount > (currentoption - maxDisplay)) && optioncount <= currentoption) {
            doDraw = true;
            textureY = menuY + headerHeight + (optioncount - (currentoption - maxDisplay + 1)) * optionHeight + 0.0175f;
        }

        if (doDraw) {
            int resX, resY;
            GRAPHICS::GET_ACTUAL_SCREEN_RESOLUTION(&resX, &resY);
            float ratio = static_cast<float>(resX) / static_cast<float>(resY);
            foregroundSpriteCalls.push_back(
                [=]() {
                    drawSprite("commonmenu", "arrowleft",
                        menuX + menuWidth / 2.0f - optionRightMargin - textWidth - spriteSz / ratio,
                        textureY,
                        spriteSz / ratio, spriteSz,
                        0.0f,
                        highlighted ? solidBlack : solidWhite);
                    drawSprite("commonmenu", "arrowright",
                        menuX + menuWidth / 2.0f - optionRightMargin,
                        textureY,
                        spriteSz / ratio, spriteSz,
                        0.0f,
                        highlighted ? solidBlack : solidWhite);
                });
            arrowBuff = spriteSz / ratio;
        }
    }

    // Non-scroll
    if (currentoption <= maxDisplay && optioncount <= maxDisplay) {
        textDraws.push_back([=]() { drawText(printVar, optionsFont,
                                      optionRightMargin / 2.0f + arrowBuff,
                                      indicatorHeight + menuY,
                                      optionTextSize, optionTextSize,
                                      highlighted ? optionsTextSelectColor : optionsTextColor, 2); });
    }
    // Scroll
    else if (optioncount > currentoption - maxDisplay && optioncount <= currentoption) {
        int optioncount_ = optioncount;
        textDraws.push_back([=]() { drawText(printVar, optionsFont,
                                      optionRightMargin / 2.0f + arrowBuff,
                                      menuY + headerHeight + (optioncount_ - (currentoption - maxDisplay + 1)) * optionHeight,
                                      optionTextSize, optionTextSize,
                                      highlighted ? optionsTextSelectColor : optionsTextColor, 2); });
    }
}

/*
 * Section Actions/Input
 */
void Menu::changeMenu(const std::string& menuname) {
    currentmenu[menulevel] = actualmenu;
    lastoption[actualmenu] = currentoption;
    menulevel++;
    actualmenu = menuname;
    previousoption = currentoption;
    currentoption = getWithDef(lastoption, actualmenu, 1);
    menuBeep();
    resetButtonStates();
}

void Menu::nextOption() {
    previousoption = currentoption;
    if (currentoption < optioncount)
        currentoption++;
    else
        currentoption = 1;
    if (menulevel > 0) {
        menuBeep();
    }
    resetButtonStates();
}

void Menu::previousOption() {
    previousoption = currentoption;
    if (currentoption > 1)
        currentoption--;
    else
        currentoption = optioncount;
    if (menulevel > 0) {
        menuBeep();
    }
    resetButtonStates();
}

void Menu::backMenu() {
    if (menulevel > 0) {
        menuBeep();
    }
    lastoption[actualmenu] = currentoption;
    menulevel--;
    actualmenu = currentmenu[menulevel];
    previousoption = currentoption;
    currentoption = getWithDef(lastoption, actualmenu, 1);

}

void Menu::menuBeep() {
    AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
}

void Menu::resetButtonStates() {
    optionpress = false;
    leftpress = false;
    rightpress = false;
}

void Menu::enableKeysOnce(bool enable) {
    CAM::SET_CINEMATIC_BUTTON_ACTIVE(enable);
    if (mRecordGlobal != 0) {
        auto* ptr = getGlobalPtr(mRecordGlobal);
        if (ptr)
            *ptr = !enable;
    }
}

void Menu::hideHUDComponents() {
    HUD::HIDE_HELP_TEXT_THIS_FRAME();
    HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HudComponentVehicleName);
    HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HudComponentAreaName);
    HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HudComponentUnused);
    HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HudComponentStreetName);
    HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HudComponentHelpText);
}

void Menu::disableKeys() {
    enableKeysOnce(false);

    // sjaak327
    // http://gtaforums.com/topic/796908-simple-trainer-for-gtav/?view=findpost&p=1069587144
    PAD::DISABLE_CONTROL_ACTION(0, ControlPhone, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlTalk, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlVehicleCinCam, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlVehicleRadioWheel, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlMeleeAttackLight, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlMeleeAttackHeavy, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlMeleeAttackAlternate, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlMeleeBlock, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlHUDSpecial, true);
    PAD::DISABLE_CONTROL_ACTION(0, ControlCharacterWheel, true);
}

void Menu::processMenuNav() {
    if (controls.IsKeyPressed(MenuControls::MenuSelect) ||
        controls.IsKeyPressed(MenuControls::MenuCancel) ||
        controls.IsKeyPressed(MenuControls::MenuUp) ||
        controls.IsKeyPressed(MenuControls::MenuDown) ||
        controls.IsKeyPressed(MenuControls::MenuLeft) ||
        controls.IsKeyPressed(MenuControls::MenuRight)) {
        useNative = false;
    }

    if (controls.IsKeyJustReleased(MenuControls::MenuKey) || useNative &&
        PAD::IS_DISABLED_CONTROL_PRESSED(0, controls.ControllerButton1) &&
        PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, controls.ControllerButton2)) {
        if (menulevel == 0) {
            OpenMenu();
        }
        else {
            CloseMenu();
            enableKeysOnce(true);
            if (onExit) {
                onExit();
            }
        }
        delay = GetTickCount64();
        return;
    }
    if (controls.IsKeyJustReleased(MenuControls::MenuCancel) || 
        useNative && PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendCancel)) {
        if (menulevel > 0) {
            if (menulevel == 1) {
                enableKeysOnce(true);
                if (onExit) {
                    onExit();
                }
            }
            backMenu();

        }
        delay = GetTickCount64();
    }
    if (controls.IsKeyJustReleased(MenuControls::MenuSelect) || 
        useNative && PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, ControlFrontendAccept)) {
        if (menulevel > 0) {
            menuBeep();
        }
        optionpress = true;
        delay = GetTickCount64();
    }
    if (controls.IsKeyPressed(MenuControls::MenuDown) || 
        useNative && PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendDown)) {
        nextOption();
        delay = GetTickCount64();
    }
    if (controls.IsKeyPressed(MenuControls::MenuUp) || 
        useNative && PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendUp)) {
        previousOption();
        delay = GetTickCount64();
    }
    if (controls.IsKeyPressed(MenuControls::MenuLeft) || 
        useNative && PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneLeft)) {
        if (menulevel > 0) {
            menuBeep();
        }
        leftpress = true;
        delay = GetTickCount64();
    }
    if (controls.IsKeyPressed(MenuControls::MenuRight) || 
        useNative && PAD::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight)) {
        if (menulevel > 0) {
            menuBeep();
        }
        rightpress = true;
        delay = GetTickCount64();
    }
}


void Menu::updateScreenSize() {
    aspectR = (16.0f / 9.0f)/GRAPHICS::GET_ASPECT_RATIO(FALSE);
    menuWidth = menuWidthOriginal*aspectR;
}

}
