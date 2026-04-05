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

    DrawText("RPG ASSIGNMENT SKELETON", 250, 120, 32, RAYWHITE);
    DrawText("ENTER : Start", 380, 220, 24, LIGHTGRAY);
    DrawText("Move : Arrow keys / WASD", 330, 270, 22, GRAY);
    DrawText("NPC talk : E", 410, 305, 22, GRAY);
    DrawText("Battle : A(attack), H(heal)", 320, 340, 22, GRAY);
}