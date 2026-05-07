#pragma once
#include "raylib.h"

class ResourceManager
{
public:
    void Load();
    void Unload();
    Font& UiFont();

private:
    Font uiFont{};
    bool useCustomFont = false;
};
