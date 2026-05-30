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
    bool HasPlayerBattleSprite() const;
    Texture2D& PlayerBattleSprite();
    bool HasAssignmentMonsterSprite() const;
    Texture2D& AssignmentMonsterSprite();
    bool HasMidtermSprite() const;
    Texture2D& MidtermSprite();
    bool HasFinalSprite() const;
    Texture2D& FinalSprite();
    bool HasUtmuPortrait() const;
    Texture2D& UtmuPortrait();
    bool HasProfessorKPortrait() const;
    Texture2D& ProfessorKPortrait();
    bool HasSenpaiPortrait() const;
    Texture2D& SenpaiPortrait();
    bool HasCampusDayBackground() const;
    Texture2D& CampusDayBackground();
    bool HasCampusNightBackground() const;
    Texture2D& CampusNightBackground();
    bool HasEngineeringBackground() const;
    Texture2D& EngineeringBackground();
    bool HasHomeBackground() const;
    Texture2D& HomeBackground();

private:
    Font uiFont{};
    bool useCustomFont = false;
    Texture2D playerWalkFrames[3]{};
    bool playerWalkLoaded[3]{};
    Texture2D playerBattleTexture{};
    bool playerBattleLoaded = false;
    Texture2D assignmentMonsterTexture{};
    bool assignmentMonsterLoaded = false;
    Texture2D midtermTexture{};
    bool midtermLoaded = false;
    Texture2D finalTexture{};
    bool finalLoaded = false;
    Texture2D utmuPortraitTexture{};
    bool utmuPortraitLoaded = false;
    Texture2D professorKPortraitTexture{};
    bool professorKPortraitLoaded = false;
    Texture2D senpaiPortraitTexture{};
    bool senpaiPortraitLoaded = false;
    Texture2D campusDayBackgroundTexture{};
    bool campusDayBackgroundLoaded = false;
    Texture2D campusNightBackgroundTexture{};
    bool campusNightBackgroundLoaded = false;
    Texture2D engineeringBackgroundTexture{};
    bool engineeringBackgroundLoaded = false;
    Texture2D homeBackgroundTexture{};
    bool homeBackgroundLoaded = false;
};
