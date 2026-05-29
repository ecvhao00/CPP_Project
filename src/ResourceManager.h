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
};
