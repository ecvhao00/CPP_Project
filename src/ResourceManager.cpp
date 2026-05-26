#include "ResourceManager.h"
#include <vector>

namespace
{
bool LoadTextureFromFirstExistingPath(const char* const* paths, int pathCount, Texture2D& texture)
{
    for (int i = 0; i < pathCount; ++i)
    {
        if (FileExists(paths[i]))
        {
            texture = LoadTexture(paths[i]);
            return texture.id != 0;
        }
    }

    return false;
}
}

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

    const char* const walk0Paths[] = { "Walk_0.png", "../Walk_0.png", "C:/Users/minky/Downloads/Walk_0.png" };
    const char* const walk1Paths[] = { "Walk_1.png", "../Walk_1.png", "C:/Users/minky/Downloads/Walk_1.png" };
    const char* const walk2Paths[] = { "Walk_2.png", "../Walk_2.png", "C:/Users/minky/Downloads/Walk_2.png" };

    playerWalkLoaded[0] = LoadTextureFromFirstExistingPath(walk0Paths, 3, playerWalkFrames[0]);
    playerWalkLoaded[1] = LoadTextureFromFirstExistingPath(walk1Paths, 3, playerWalkFrames[1]);
    playerWalkLoaded[2] = LoadTextureFromFirstExistingPath(walk2Paths, 3, playerWalkFrames[2]);
}

void ResourceManager::Unload()
{
    for (int i = 0; i < 3; ++i)
    {
        if (playerWalkLoaded[i])
        {
            UnloadTexture(playerWalkFrames[i]);
            playerWalkLoaded[i] = false;
        }
    }

    if (useCustomFont)
    {
        UnloadFont(uiFont);
    }
}

Font& ResourceManager::UiFont()
{
    return uiFont;
}

bool ResourceManager::HasPlayerWalkSprites() const
{
    return playerWalkLoaded[0] && playerWalkLoaded[1] && playerWalkLoaded[2];
}

int ResourceManager::PlayerWalkFrameCount() const
{
    return 3;
}

Texture2D& ResourceManager::PlayerWalkFrame(int index)
{
    if (index < 0) index = 0;
    if (index >= 3) index = 2;
    return playerWalkFrames[index];
}
