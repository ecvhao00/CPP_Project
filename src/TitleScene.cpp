#include "TitleScene.h"
#include "Game.h"
#include "IntroScene.h"
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
        game.ChangeScene(std::make_unique<IntroScene>());
    }
}

void TitleScene::Draw(Game& game)
{
    auto& f = game.Resources().UiFont();
    DrawCenteredText(f, "A+을 노려라!", 155, 64, 2, RAYWHITE);
    DrawCenteredText(f, "15주 학기 생존 RPG", 225, 30, 1, LIGHTGRAY);

    DrawRectangle(390, 314, 500, 2, Fade(RAYWHITE, 0.35f));

    DrawCenteredText(f, "ENTER : 학기 시작", 360, 38, 1, YELLOW);
    DrawCenteredText(f, "WASD / 방향키 이동   E : 상호작용", 438, 27, 1, LIGHTGRAY);
    DrawCenteredText(f, "공학관에서 성장하고, 과제를 해결해 A+ 엔딩을 노리세요", 480, 24, 1, GRAY);
}
