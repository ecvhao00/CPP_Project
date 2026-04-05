#pragma once
#include <memory>
#include "GameData.h"
#include "ResourceManager.h"

class IScene;

class Game
{
public:
    static constexpr int ScreenWidth = 960;
    static constexpr int ScreenHeight = 540;

    Game();
    ~Game();

    void Run();
    void ChangeScene(std::unique_ptr<IScene> scene);
    void RequestQuit();

    GameData& Data();
    const GameData& Data() const;

    ResourceManager& Resources();

private:
    void ApplySceneChange();

private:
    bool running = true;
    std::unique_ptr<IScene> currentScene;
    std::unique_ptr<IScene> nextScene;
    GameData data;
    ResourceManager resources;
};