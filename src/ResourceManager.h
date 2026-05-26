#pragma once
#include "raylib.h"

class ResourceManager
{
public:
    void Load();
    void Unload();
    Font& UiFont();
    bool HasPlayerWalkSprites() const;
    int PlayerWalkFrameCount() const;
    Texture2D& PlayerWalkFrame(int index);

private:
    Font uiFont{};
    bool useCustomFont = false;
    Texture2D playerWalkFrames[3]{};
    bool playerWalkLoaded[3]{};
};
