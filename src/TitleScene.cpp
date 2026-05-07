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
    (void)game;
    DrawText("UNIV SURVIVAL : 15 WEEKS", 250, 120, 36, RAYWHITE);
    DrawText("ENTER : Start Semester", 320, 220, 28, LIGHTGRAY);
    DrawText("Mouse click zones to act", 340, 270, 22, GRAY);
    DrawText("Day: class/battle  Night: bar/home", 280, 310, 22, GRAY);
    DrawText("Debuffs happen on exam/presentation checks", 220, 350, 22, GRAY);
}
