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
    enemyHp = 12 + data.player.level * 4;
    enemyAttack = 3 + data.player.level;
    if (data.semester.midtermExamDebuff || data.semester.midtermPresentationDebuff || data.semester.finalExamDebuff || data.semester.finalPresentationDebuff)
    {
        enemyAttack += 2;
    }
    enemyTurnTimer = 0.0f;
    state = BattleState::PlayerTurn;
    message = "과제 괴물이 나타났다!";
}

void BattleScene::Update(Game& game, float dt)
{
    GameData& data = game.Data();

    switch (state)
    {
    case BattleState::PlayerTurn:
        if (IsKeyPressed(KEY_A))
        {
            enemyHp -= data.player.attack;
            if (enemyHp <= 0)
            {
                enemyHp = 0;
                data.semester.foughtToday = true;
                data.player.level += 1;
                data.player.maxHp += 2;
                data.player.hp = std::min(data.player.maxHp, data.player.hp + 3);
                data.player.attack += 1;
                message = "승리! 레벨 상승. ENTER로 돌아가기.";
                state = BattleState::Victory;
            }
            else
            {
                message = TextFormat("과제의 체력을 %d 깎았다!", data.player.attack);
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
                message = TextFormat("과제가 멘탈을 %d 깎았다!", enemyAttack);
                state = BattleState::PlayerTurn;
            }
        }
        break;
    case BattleState::Victory:
        if (IsKeyPressed(KEY_ENTER)) game.ChangeScene(std::make_unique<FieldScene>());
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
    auto& f = game.Resources().UiFont();
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, DARKPURPLE);
    DrawRectangle(160, 280, 100, 100, BLUE);
    DrawRectangle(700, 140, 120, 120, RED);
    DrawTextEx(f, "학생", {180, 390}, 26, 1, WHITE);
    DrawTextEx(f, "과제", {728, 270}, 26, 1, WHITE);
    DrawRectangle(40, 30, 880, 90, Fade(BLACK, 0.5f));
    DrawRectangle(40, 420, 880, 90, Fade(BLACK, 0.6f));
    DrawTextEx(f, TextFormat("멘탈: %d / %d  레벨:%d", data.player.hp, data.player.maxHp, data.player.level), {60, 50}, 28, 1, WHITE);
    DrawTextEx(f, TextFormat("과제 체력: %d", enemyHp), {650, 50}, 28, 1, WHITE);
    DrawTextEx(f, message.c_str(), {60, 440}, 24, 1, YELLOW);
    if (state == BattleState::PlayerTurn)
    {
        DrawTextEx(f, "[A] 공격", {60, 475}, 22, 1, RAYWHITE);
        DrawTextEx(f, "[H] 회복", {220, 475}, 22, 1, RAYWHITE);
    }
    else if (state == BattleState::EnemyTurn) DrawTextEx(f, "과제가 반격하는 중...", {60, 475}, 22, 1, LIGHTGRAY);
    else DrawTextEx(f, "[ENTER] 계속", {60, 475}, 22, 1, RAYWHITE);
}
