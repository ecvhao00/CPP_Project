#include "TitleScene.h"
#include "Game.h"
#include "FieldScene.h"
#include "raylib.h"
#include <memory>

void TitleScene::Enter(Game& game)
{
    game.Data() = GameData{};
}

void TitleScene::Update(Game& game, float dt)
{
    (void)dt;
    if (IsKeyPressed(KEY_ENTER))
    {
        game.Data() = GameData{};
        game.ChangeScene(std::make_unique<FieldScene>());
    }
}

void TitleScene::Draw(Game& game)
{
    auto& f = game.Resources().UiFont();
    DrawTextEx(f, "UNIV SURVIVAL : 15 WEEKS", {250, 120}, 52, 2, RAYWHITE);
    DrawTextEx(f, "ENTER : Start Semester", {380, 250}, 34, 1, LIGHTGRAY);
    DrawTextEx(f, "Move with WASD/Arrows and press E to interact", {240, 320}, 30, 1, GRAY);
    DrawTextEx(f, "malgunsl.ttf exists -> Korean text rendering enabled", {250, 360}, 26, 1, GRAY);
}
