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
    int level = 1;
};

struct SemesterData
{
    int week = 1;
    bool isNight = false;

    int assignmentScore = 10;
    int attendanceScore = 10;
    int devPower = 5;
    int network = 3;

    bool foughtToday = false;
    bool attendedClassToday = false;
    int homeActionsUsedTonight = 0;
    int drinksTonight = 0;

    bool midtermExamDebuff = false;
    bool midtermPresentationDebuff = false;
    bool finalExamDebuff = false;
    bool finalPresentationDebuff = false;

    bool gameEnded = false;
    bool passed = false;
};

struct GameData
{
    PlayerData player;
    SemesterData semester;
    bool hasPotion = true;
    bool bossDefeated = false;
};
