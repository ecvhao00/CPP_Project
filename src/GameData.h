#pragma once
#include "raylib.h"

struct PlayerData
{
    Vector2 position = { 100.0f, 100.0f };
    int hp = 30;
    int maxHp = 30;
    int attack = 8;
    int gold = 0;
    float speed = 180.0f;
};

struct GameData
{
    PlayerData player;
    bool hasPotion = true;
    bool bossDefeated = false;
};