#include "FieldScene.h"
#include "BattleScene.h"
#include "Game.h"
#include "TitleScene.h"
#include <memory>

FieldScene::FieldScene() {}

void FieldScene::Enter(Game& game)
{
    (void)game;
    showMessage = false;
}

void FieldScene::EndDay(Game& game)
{
    auto& s = game.Data().semester;
    if (!s.foughtToday) s.assignmentScore -= 1;
    if (!s.attendedClassToday) s.attendanceScore -= 1;
    s.isNight = true;
    s.homeActionsUsedTonight = 0;
    s.drinksTonight = 0;
}

void FieldScene::NextWeek(Game& game)
{
    auto& s = game.Data().semester;
    s.week += 1;
    s.isNight = false;
    s.foughtToday = false;
    s.attendedClassToday = false;
    ApplyWeekEvent(game);
}

void FieldScene::ApplyWeekEvent(Game& game)
{
    auto& d = game.Data();
    auto& s = d.semester;

    if (s.week == 8)
    {
        s.midtermExamDebuff = s.devPower < 12;
        message = s.midtermExamDebuff ? "중간고사 디버프 발생!" : "중간고사 안정 통과!";
        showMessage = true;
    }
    else if (s.week == 9)
    {
        s.midtermPresentationDebuff = s.network < 10;
        message = s.midtermPresentationDebuff ? "중간발표 디버프 발생!" : "중간발표 안정 통과!";
        showMessage = true;
    }
    else if (s.week == 14)
    {
        s.finalExamDebuff = s.devPower < 20;
        message = s.finalExamDebuff ? "기말고사 디버프 발생!" : "기말고사 안정 통과!";
        showMessage = true;
    }
    else if (s.week == 15)
    {
        s.finalPresentationDebuff = s.network < 18;
        s.gameEnded = true;
        s.passed = !(s.finalPresentationDebuff || s.finalExamDebuff || s.midtermExamDebuff || s.midtermPresentationDebuff) && s.assignmentScore > 0 && s.attendanceScore > 0;
        message = s.passed ? "학기 종료! 훌륭한 생존!" : "학기 종료... 아쉬운 결과";
        showMessage = true;
    }

    int debuffs = (s.midtermExamDebuff?1:0)+(s.midtermPresentationDebuff?1:0)+(s.finalExamDebuff?1:0)+(s.finalPresentationDebuff?1:0);
    d.player.attack = 8 + d.player.level - debuffs;
    if (d.player.attack < 3) d.player.attack = 3;
}

void FieldScene::Update(Game& game, float dt)
{
    (void)dt;
    auto& d = game.Data();
    auto& s = d.semester;
    Vector2 m = GetMousePosition();

    if (s.gameEnded)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            game.Data() = GameData{};
            game.ChangeScene(std::make_unique<TitleScene>());
        }
        return;
    }

    if (CheckCollisionPointRec(m, classZone) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !s.isNight)
    {
        s.attendedClassToday = true;
        s.devPower += 2;
        message = "김교수 수업 미니게임 성공! 개발력 +2";
        showMessage = true;
    }
    if (CheckCollisionPointRec(m, helperZone) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        s.network += 1;
        message = "조력자(선배) 조언 획득! 인맥도 +1";
        showMessage = true;
    }
    if (CheckCollisionPointRec(m, battleZone) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        game.ChangeScene(std::make_unique<BattleScene>());
        return;
    }
    if (CheckCollisionPointRec(m, barZone) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && s.isNight)
    {
        if (s.drinksTonight < 2)
        {
            s.drinksTonight++;
            s.network += 2;
            message = TextFormat("%d차 술자리! 인맥도 +2", s.drinksTonight);
            showMessage = true;
        }
    }
    if (CheckCollisionPointRec(m, homeZone) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && s.isNight)
    {
        int maxTurns = 2 - s.drinksTonight;
        if (s.homeActionsUsedTonight < maxTurns)
        {
            s.homeActionsUsedTonight++;
            if (IsKeyDown(KEY_LEFT_SHIFT))
            {
                s.devPower += 1;
                message = "집에서 복습! 개발력 +1";
            }
            else
            {
                game.ChangeScene(std::make_unique<BattleScene>());
                return;
            }
            showMessage = true;
        }
    }

    if (CheckCollisionPointRec(m, nextZone) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (!s.isNight) EndDay(game);
        else NextWeek(game);
    }
}

void FieldScene::Draw(Game& game)
{
    const auto& d = game.Data();
    const auto& s = d.semester;
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Color{35, 60, 40, 255});
    DrawRectangleRec(classZone, BLUE);
    DrawText("DAY: CLASS", classZone.x + 8, classZone.y + 28, 20, WHITE);
    DrawRectangleRec(helperZone, ORANGE);
    DrawText("HELPER", helperZone.x + 20, helperZone.y + 28, 20, WHITE);
    DrawRectangleRec(battleZone, RED);
    DrawText("BATTLE", battleZone.x + 20, battleZone.y + 28, 20, WHITE);
    DrawRectangleRec(barZone, PURPLE);
    DrawText("NIGHT: BAR", barZone.x + 12, barZone.y + 28, 20, WHITE);
    DrawRectangleRec(homeZone, BROWN);
    DrawText("NIGHT: HOME", homeZone.x + 8, homeZone.y + 28, 20, WHITE);
    DrawRectangleRec(nextZone, DARKBLUE);
    DrawText(s.isNight ? "NEXT WEEK" : "TO NIGHT", nextZone.x + 8, nextZone.y + 28, 20, WHITE);

    DrawRectangle(10, 10, 500, 170, Fade(BLACK, 0.5f));
    DrawText(TextFormat("Week %d (%s)", s.week, s.isNight ? "Night" : "Day"), 20, 20, 24, YELLOW);
    DrawText(TextFormat("HP %d/%d  LV %d  ATK %d", d.player.hp, d.player.maxHp, d.player.level, d.player.attack), 20, 50, 22, WHITE);
    DrawText(TextFormat("Dev:%d  Network:%d", s.devPower, s.network), 20, 78, 22, WHITE);
    DrawText(TextFormat("Assignment:%d  Attendance:%d", s.assignmentScore, s.attendanceScore), 20, 106, 22, WHITE);
    DrawText("HOME: click + Shift=study, click only=battle", 20, 136, 18, LIGHTGRAY);

    if (showMessage)
    {
        DrawRectangle(120, 500, 760, 70, Fade(BLACK, 0.7f));
        DrawText(message, 140, 525, 24, RAYWHITE);
    }

    if (s.gameEnded)
    {
        DrawRectangle(160, 190, 640, 180, Fade(BLACK, 0.8f));
        DrawText(s.passed ? "PASS - ENTER to title" : "FAIL - ENTER to title", 250, 250, 36, YELLOW);
    }
}
