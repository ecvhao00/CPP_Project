#include "Game.h"
#include "Scene.h"
#include "TitleScene.h"
#include "raylib.h"

Game::Game() = default;
Game::~Game() = default;

void Game::Run()
{
    InitWindow(ScreenWidth, ScreenHeight, "raylib RPG skeleton");
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

        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScene)
        {
            currentScene->Draw(*this);
        }

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