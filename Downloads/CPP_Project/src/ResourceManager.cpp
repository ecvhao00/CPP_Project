#include "ResourceManager.h"
#include <vector>

void ResourceManager::Load()
{
    if (FileExists("malgunsl.ttf"))
    {
        std::vector<int> koreanCodepoints;
        koreanCodepoints.reserve(11172 + 51 + 30 + 128);

        // Basic ASCII
        for (int cp = 0x20; cp <= 0x7E; ++cp) koreanCodepoints.push_back(cp);
        // Hangul Jamo
        for (int cp = 0x1100; cp <= 0x11FF; ++cp) koreanCodepoints.push_back(cp);
        // Hangul Compatibility Jamo
        for (int cp = 0x3130; cp <= 0x318F; ++cp) koreanCodepoints.push_back(cp);
        // Hangul Syllables (가-힣)
        for (int cp = 0xAC00; cp <= 0xD7A3; ++cp) koreanCodepoints.push_back(cp);

        uiFont = LoadFontEx("malgunsl.ttf", 28, koreanCodepoints.data(), (int)koreanCodepoints.size());
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
