#pragma once
#include "raylib.h"
#include <string>

struct PlayerData
{
    Vector2 position = { 720.0f, 490.0f };
    int hp = 30;
    int maxHp = 30;
    int attack = 8;
    int gold = 0;
    float speed = 540.0f;
    int level = 1;
};

struct SemesterData
{
    int week = 1;
    bool isNight = false;

    int assignmentMisses = 0;
    int absences = 0;
    int devPower = 5;
    int network = 3;
    int maxActionPoints = 3;
    int actionPoints = 3;

    bool foughtToday = false;
    bool tookExamToday = false;
    bool currentBattleIsExam = false;
    bool attendedClassToday = false;
    int homeActionsUsedTonight = 0;
    int drinksTonight = 0;

    bool midtermExamDebuff = false;
    bool midtermPresentationDebuff = false;
    bool finalExamDebuff = false;
    bool finalPresentationDebuff = false;

    bool gameEnded = false;
    bool passed = false;
    std::string endingName = "C";
};

struct GameData
{
    PlayerData player;
    SemesterData semester;
    bool hasPotion = true;
    bool bossDefeated = false;
};
