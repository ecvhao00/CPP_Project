#include "ResourceManager.h"

void ResourceManager::Load()
{
    if (FileExists("malgunsl.ttf"))
    {
        uiFont = LoadFontEx("malgunsl.ttf", 28, nullptr, 0);
        useCustomFont = uiFont.texture.id != 0;
    }
    if (!useCustomFont)
    {
        uiFont = GetFontDefault();
    }
}

void ResourceManager::Unload()
{
    if (useCustomFont)
    {
        UnloadFont(uiFont);
    }
}

Font& ResourceManager::UiFont()
{
    return uiFont;
}
