#include "Game.h"
#include "BattleScene.h"
#include "FieldScene.h"
#include "Scene.h"
#include "TitleScene.h"
#include "raylib.h"

Game::Game() = default;
Game::~Game() = default;

void Game::Run()
{
    InitWindow(ScreenWidth, ScreenHeight, "A+을 노려라!");
    SetTargetFPS(60);

    resources.Load();

    ChangeScene(std::make_unique<TitleScene>());
    ApplySceneChange();

    while (running && !WindowShouldClose())
    {
        ApplySceneChange();

        float dt = GetFrameTime();

        if (currentScene)
        {
            currentScene->Update(*this, dt);
        }
#if defined(DEBUG)
        HandleDebugShortcuts();
#endif

        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScene)
        {
            currentScene->Draw(*this);
        }
#if defined(DEBUG)
        DrawDebugOverlay();
#endif

        EndDrawing();
    }

    resources.Unload();
    CloseWindow();
}

void Game::ChangeScene(std::unique_ptr<IScene> scene)
{
    nextScene = std::move(scene);
}

void Game::RequestQuit()
{
    running = false;
}

GameData& Game::Data()
{
    return data;
}

const GameData& Game::Data() const
{
    return data;
}

ResourceManager& Game::Resources()
{
    return resources;
}

void Game::ApplySceneChange()
{
    if (nextScene)
    {
        currentScene = std::move(nextScene);
        currentScene->Enter(*this);
    }
}

#if defined(DEBUG)
void Game::HandleDebugShortcuts()
{
    if (IsKeyPressed(KEY_F8))
    {
        JumpToDebugWeek(8, false);
    }
    else if (IsKeyPressed(KEY_F9))
    {
        JumpToDebugWeek(15, false);
    }
    else if (IsKeyPressed(KEY_F10))
    {
        JumpToDebugWeek(15, true);
    }
    else if (IsKeyPressed(KEY_F11))
    {
        data.player.hp = data.player.maxHp;
        data.hasPotion = true;
        data.semester.actionPoints = data.semester.maxActionPoints;
    }
    else if (IsKeyPressed(KEY_F5))
    {
        data.semester.devPower += IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) ? -5 : 5;
        if (data.semester.devPower < 0) data.semester.devPower = 0;
    }
    else if (IsKeyPressed(KEY_F6))
    {
        data.semester.network += IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) ? -5 : 5;
        if (data.semester.network < 0) data.semester.network = 0;
    }
}

void Game::JumpToDebugWeek(int week, bool startExamBattle)
{
    data = GameData{};
    data.semester.week = week;
    data.semester.isNight = false;
    data.semester.actionPoints = data.semester.maxActionPoints;
    data.semester.assignmentMisses = 0;
    data.semester.absences = 0;
    data.semester.devPower = week >= 15 ? 30 : 15;
    data.semester.network = week >= 15 ? 20 : 10;

    data.player.level = week >= 15 ? 7 : 4;
    data.player.maxHp = week >= 15 ? 46 : 38;
    data.player.hp = data.player.maxHp;
    data.player.attack = week >= 15 ? 16 : 12;
    data.player.position = { 210.0f, 250.0f };
    data.hasPotion = true;

    if (startExamBattle)
    {
        data.semester.currentBattleIsExam = true;
        ChangeScene(std::make_unique<BattleScene>());
    }
    else
    {
        ChangeScene(std::make_unique<FieldScene>(TextFormat("DEBUG: %d주차로 이동", week), true, true));
    }
}

void Game::DrawDebugOverlay()
{
    Font& font = resources.UiFont();
    DrawRectangle(14, 14, 670, 54, Fade(BLACK, 0.62f));
    DrawRectangleLines(14, 14, 670, 54, Fade(RAYWHITE, 0.45f));
    DrawTextEx(font, "DEBUG  F8: 8주차  F9: 15주차  F10: 기말전투  F11: 회복/행동력", { 24, 19 }, 18, 1, LIGHTGRAY);
    DrawTextEx(font, "F5: 개발력 +5  Shift+F5: -5    F6: 인맥 +5  Shift+F6: -5", { 24, 43 }, 18, 1, LIGHTGRAY);
}
#endif
