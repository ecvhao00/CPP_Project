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

    const char* const walk0Paths[] = { "assets/player/Walk_0.png" };
    const char* const walk1Paths[] = { "assets/player/Walk_1.png" };
    const char* const walk2Paths[] = { "assets/player/Walk_2.png" };

    playerWalkLoaded[0] = LoadTextureFromFirstExistingPath(walk0Paths, 1, playerWalkFrames[0]);
    playerWalkLoaded[1] = LoadTextureFromFirstExistingPath(walk1Paths, 1, playerWalkFrames[1]);
    playerWalkLoaded[2] = LoadTextureFromFirstExistingPath(walk2Paths, 1, playerWalkFrames[2]);

    const char* const battlePaths[] = { "assets/player/Battle.png", "Assets/player/Battle.png" };
    playerBattleLoaded = LoadTextureFromFirstExistingPath(battlePaths, 2, playerBattleTexture);
    if (playerBattleLoaded)
    {
        SetTextureFilter(playerBattleTexture, TEXTURE_FILTER_POINT);
    }

    const char* const assignmentMonsterPaths[] = {
        "assets/Enemy/Stylish_C_Monster_Battle.png",
        "Assets/Enemy/Stylish_C_Monster_Battle.png"
    };
    assignmentMonsterLoaded = LoadTextureFromFirstExistingPath(assignmentMonsterPaths, 2, assignmentMonsterTexture);
    if (assignmentMonsterLoaded)
    {
        SetTextureFilter(assignmentMonsterTexture, TEXTURE_FILTER_POINT);
    }

    const char* const midtermPaths[] = {
        "assets/Enemy/Midterm.png",
        "Assets/Enemy/Midterm.png"
    };
    midtermLoaded = LoadTextureFromFirstExistingPath(midtermPaths, 2, midtermTexture);
    if (midtermLoaded)
    {
        SetTextureFilter(midtermTexture, TEXTURE_FILTER_POINT);
    }

    const char* const finalPaths[] = {
        "assets/Enemy/Final.png",
        "Assets/Enemy/Final.png"
    };
    finalLoaded = LoadTextureFromFirstExistingPath(finalPaths, 2, finalTexture);
    if (finalLoaded)
    {
        SetTextureFilter(finalTexture, TEXTURE_FILTER_POINT);
    }

    const char* const utmuPortraitPaths[] = {
        "assets/NPC/Utmu.png",
        "Assets/NPC/Utmu.png"
    };
    utmuPortraitLoaded = LoadTextureFromFirstExistingPath(utmuPortraitPaths, 2, utmuPortraitTexture);
    if (utmuPortraitLoaded)
    {
        SetTextureFilter(utmuPortraitTexture, TEXTURE_FILTER_POINT);
    }

    const char* const professorKPortraitPaths[] = {
        "assets/NPC/Professor_K.png",
        "Assets/NPC/Professor_K.png"
    };
    professorKPortraitLoaded = LoadTextureFromFirstExistingPath(professorKPortraitPaths, 2, professorKPortraitTexture);
    if (professorKPortraitLoaded)
    {
        SetTextureFilter(professorKPortraitTexture, TEXTURE_FILTER_POINT);
    }

    const char* const senpaiPortraitPaths[] = {
        "assets/NPC/Senpai.png",
        "Assets/NPC/Senpai.png"
    };
    senpaiPortraitLoaded = LoadTextureFromFirstExistingPath(senpaiPortraitPaths, 2, senpaiPortraitTexture);
    if (senpaiPortraitLoaded)
    {
        SetTextureFilter(senpaiPortraitTexture, TEXTURE_FILTER_POINT);
    }

    const char* const campusDayBackgroundPaths[] = {
        "assets/background/Smu_Day.png",
        "assets/Background/Smu_Day.png",
        "Assets/background/Smu_Day.png",
        "Assets/Background/Smu_Day.png"
    };
    campusDayBackgroundLoaded = LoadTextureFromFirstExistingPath(campusDayBackgroundPaths, 4, campusDayBackgroundTexture);

    const char* const campusNightBackgroundPaths[] = {
        "assets/background/Smu_Night.png",
        "assets/Background/Smu_Night.png",
        "Assets/background/Smu_Night.png",
        "Assets/Background/Smu_Night.png"
    };
    campusNightBackgroundLoaded = LoadTextureFromFirstExistingPath(campusNightBackgroundPaths, 4, campusNightBackgroundTexture);

    const char* const engineeringBackgroundPaths[] = {
        "assets/background/GongHak.png",
        "assets/Background/GongHak.png",
        "Assets/background/GongHak.png",
        "Assets/Background/GongHak.png"
    };
    engineeringBackgroundLoaded = LoadTextureFromFirstExistingPath(engineeringBackgroundPaths, 4, engineeringBackgroundTexture);

    const char* const homeBackgroundPaths[] = {
        "assets/background/Home.png",
        "assets/Background/Home.png",
        "Assets/background/Home.png",
        "Assets/Background/Home.png"
    };
    homeBackgroundLoaded = LoadTextureFromFirstExistingPath(homeBackgroundPaths, 4, homeBackgroundTexture);
}

void ResourceManager::Unload()
{
    if (homeBackgroundLoaded)
    {
        UnloadTexture(homeBackgroundTexture);
        homeBackgroundLoaded = false;
    }

    if (engineeringBackgroundLoaded)
    {
        UnloadTexture(engineeringBackgroundTexture);
        engineeringBackgroundLoaded = false;
    }

    if (campusNightBackgroundLoaded)
    {
        UnloadTexture(campusNightBackgroundTexture);
        campusNightBackgroundLoaded = false;
    }

    if (campusDayBackgroundLoaded)
    {
        UnloadTexture(campusDayBackgroundTexture);
        campusDayBackgroundLoaded = false;
    }

    if (senpaiPortraitLoaded)
    {
        UnloadTexture(senpaiPortraitTexture);
        senpaiPortraitLoaded = false;
    }

    if (professorKPortraitLoaded)
    {
        UnloadTexture(professorKPortraitTexture);
        professorKPortraitLoaded = false;
    }

    if (utmuPortraitLoaded)
    {
        UnloadTexture(utmuPortraitTexture);
        utmuPortraitLoaded = false;
    }

    if (finalLoaded)
    {
        UnloadTexture(finalTexture);
        finalLoaded = false;
    }

    if (midtermLoaded)
    {
        UnloadTexture(midtermTexture);
        midtermLoaded = false;
    }

    if (assignmentMonsterLoaded)
    {
        UnloadTexture(assignmentMonsterTexture);
        assignmentMonsterLoaded = false;
    }

    if (playerBattleLoaded)
    {
        UnloadTexture(playerBattleTexture);
        playerBattleLoaded = false;
    }

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

bool ResourceManager::HasPlayerBattleSprite() const
{
    return playerBattleLoaded;
}

Texture2D& ResourceManager::PlayerBattleSprite()
{
    return playerBattleTexture;
}

bool ResourceManager::HasAssignmentMonsterSprite() const
{
    return assignmentMonsterLoaded;
}

Texture2D& ResourceManager::AssignmentMonsterSprite()
{
    return assignmentMonsterTexture;
}

bool ResourceManager::HasMidtermSprite() const
{
    return midtermLoaded;
}

Texture2D& ResourceManager::MidtermSprite()
{
    return midtermTexture;
}

bool ResourceManager::HasFinalSprite() const
{
    return finalLoaded;
}

Texture2D& ResourceManager::FinalSprite()
{
    return finalTexture;
}

bool ResourceManager::HasUtmuPortrait() const
{
    return utmuPortraitLoaded;
}

Texture2D& ResourceManager::UtmuPortrait()
{
    return utmuPortraitTexture;
}

bool ResourceManager::HasProfessorKPortrait() const
{
    return professorKPortraitLoaded;
}

Texture2D& ResourceManager::ProfessorKPortrait()
{
    return professorKPortraitTexture;
}

bool ResourceManager::HasSenpaiPortrait() const
{
    return senpaiPortraitLoaded;
}

Texture2D& ResourceManager::SenpaiPortrait()
{
    return senpaiPortraitTexture;
}

bool ResourceManager::HasCampusDayBackground() const
{
    return campusDayBackgroundLoaded;
}

Texture2D& ResourceManager::CampusDayBackground()
{
    return campusDayBackgroundTexture;
}

bool ResourceManager::HasCampusNightBackground() const
{
    return campusNightBackgroundLoaded;
}

Texture2D& ResourceManager::CampusNightBackground()
{
    return campusNightBackgroundTexture;
}

bool ResourceManager::HasEngineeringBackground() const
{
    return engineeringBackgroundLoaded;
}

Texture2D& ResourceManager::EngineeringBackground()
{
    return engineeringBackgroundTexture;
}

bool ResourceManager::HasHomeBackground() const
{
    return homeBackgroundLoaded;
}

Texture2D& ResourceManager::HomeBackground()
{
    return homeBackgroundTexture;
}
