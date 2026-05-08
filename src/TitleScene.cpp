#include "TitleScene.h"
#include "Game.h"
#include "FieldScene.h"
#include "raylib.h"
#include <memory>

static void DrawCenteredText(Font& font, const char* text, float y, float fontSize, float spacing, Color color)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextEx(font, text, { (Game::ScreenWidth - size.x) * 0.5f, y }, fontSize, spacing, color);
}

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
    DrawCenteredText(f, "UNIV SURVIVAL : 15 WEEKS", 120, 52, 2, RAYWHITE);
    DrawCenteredText(f, "ENTER : Start Semester", 250, 34, 1, LIGHTGRAY);
    DrawCenteredText(f, "Move with WASD/Arrows and press E to interact", 320, 30, 1, GRAY);
    DrawCenteredText(f, "malgunsl.ttf exists -> Korean text rendering enabled", 360, 26, 1, GRAY);
}
