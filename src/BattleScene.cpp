#include "BattleScene.h"
#include "Game.h"
#include "FieldScene.h"
#include "TitleScene.h"
#include "raylib.h"
#include <algorithm>
#include <memory>

void BattleScene::Enter(Game& game)
{
    GameData& data = game.Data();
    const bool isExamBattle = data.semester.currentBattleIsExam;
    enemyHp = 12 + data.player.level * 4;
    enemyAttack = 3 + data.player.level;
    if (data.semester.midtermExamDebuff || data.semester.midtermPresentationDebuff || data.semester.finalExamDebuff || data.semester.finalPresentationDebuff)
    {
        enemyAttack += 2;
    }
    enemyTurnTimer = 0.0f;
    state = BattleState::PlayerTurn;
    message = isExamBattle ? "시험이 시작됐다!" : "과제 괴물이 나타났다!";
}

void BattleScene::Update(Game& game, float dt)
{
    GameData& data = game.Data();
    const char* opponentName = data.semester.currentBattleIsExam ? "시험" : "과제";

    switch (state)
    {
    case BattleState::PlayerTurn:
        if (IsKeyPressed(KEY_A))
        {
            enemyHp -= data.player.attack;
            if (enemyHp <= 0)
            {
                enemyHp = 0;
                if (data.semester.currentBattleIsExam) data.semester.tookExamToday = true;
                else data.semester.foughtToday = true;
                data.player.level += 1;
                data.player.maxHp += 2;
                data.player.hp = std::min(data.player.maxHp, data.player.hp + 3);
                data.player.attack += 1;
                message = "승리! 레벨 상승. ENTER로 돌아가기.";
                state = BattleState::Victory;
            }
            else
            {
                message = TextFormat("%s의 체력을 %d 깎았다!", opponentName, data.player.attack);
                state = BattleState::EnemyTurn;
                enemyTurnTimer = 0.0f;
            }
        }
        else if (IsKeyPressed(KEY_H))
        {
            if (data.hasPotion)
            {
                data.player.hp = std::min(data.player.maxHp, data.player.hp + 10);
                data.hasPotion = false;
                message = "간식을 먹고 멘탈을 회복했다.";
                state = BattleState::EnemyTurn;
            }
            else
            {
                message = "남은 회복 아이템이 없다.";
            }
        }
        break;
    case BattleState::EnemyTurn:
        enemyTurnTimer += dt;
        if (enemyTurnTimer > 0.5f)
        {
            data.player.hp -= enemyAttack;
            if (data.player.hp <= 0)
            {
                data.player.hp = 0;
                message = "번아웃... ENTER로 타이틀로 돌아가기.";
                state = BattleState::Defeat;
            }
            else
            {
                message = TextFormat("%s이 멘탈을 %d 깎았다!", opponentName, enemyAttack);
                state = BattleState::PlayerTurn;
            }
        }
        break;
    case BattleState::Victory:
        if (IsKeyPressed(KEY_ENTER))
        {
            data.semester.currentBattleIsExam = false;
            game.ChangeScene(std::make_unique<FieldScene>());
        }
        break;
    case BattleState::Defeat:
        if (IsKeyPressed(KEY_ENTER))
        {
            game.Data() = GameData{};
            game.ChangeScene(std::make_unique<TitleScene>());
        }
        break;
    }
}

void BattleScene::Draw(Game& game)
{
    const GameData& data = game.Data();
    const bool isExamBattle = data.semester.currentBattleIsExam;
    auto& f = game.Resources().UiFont();

    Rectangle topPanel = { 40, 30, (float)Game::ScreenWidth - 80, 92 };
    Rectangle bottomPanel = { 0, 520, (float)Game::ScreenWidth, 200 };
    Rectangle playerBody = { 190, 285, 120, 120 };
    Rectangle enemyBody = { 915, 185, 140, 140 };

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, DARKPURPLE);
    DrawRectangleRec(playerBody, BLUE);
    DrawRectangleRec(enemyBody, RED);
    DrawTextEx(f, "학생", {playerBody.x + 34, playerBody.y + playerBody.height + 12}, 28, 1, WHITE);
    DrawTextEx(f, isExamBattle ? "시험" : "과제", {enemyBody.x + 42, enemyBody.y + enemyBody.height + 12}, 28, 1, WHITE);

    DrawRectangleRec(topPanel, Fade(BLACK, 0.55f));
    DrawRectangleLinesEx(topPanel, 2, Fade(RAYWHITE, 0.7f));
    DrawTextEx(f, TextFormat("멘탈 %d/%d", data.player.hp, data.player.maxHp), {70, 50}, 30, 1, WHITE);
    DrawTextEx(f, TextFormat("레벨 %d  공격 %d", data.player.level, data.player.attack), {70, 86}, 22, 1, LIGHTGRAY);
    DrawTextEx(f, "VS", {618, 60}, 38, 2, YELLOW);
    DrawTextEx(f, TextFormat("%s 체력 %d", isExamBattle ? "시험" : "과제", enemyHp), {930, 50}, 30, 1, WHITE);
    DrawTextEx(f, TextFormat("반격 피해 %d", enemyAttack), {930, 86}, 22, 1, LIGHTGRAY);

    DrawRectangleRec(bottomPanel, Fade(BLACK, 0.78f));
    DrawRectangle(0, 520, Game::ScreenWidth, 3, Fade(RAYWHITE, 0.45f));
    DrawTextEx(f, message.c_str(), {70, 550}, 28, 1, YELLOW);
    if (state == BattleState::PlayerTurn)
    {
        DrawTextEx(f, "[A] 공격", {70, 630}, 26, 1, RAYWHITE);
        DrawTextEx(f, "[H] 회복", {240, 630}, 26, 1, RAYWHITE);
    }
    else if (state == BattleState::EnemyTurn) DrawTextEx(f, isExamBattle ? "시험이 반격하는 중..." : "과제가 반격하는 중...", {70, 630}, 26, 1, LIGHTGRAY);
    else DrawTextEx(f, "[ENTER] 계속", {70, 630}, 26, 1, RAYWHITE);
}
