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
    message = "An assignment monster appeared!";
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
                message = "Victory! LV up. ENTER to return.";
                state = BattleState::Victory;
            }
            else
            {
                message = TextFormat("You dealt %d damage!", data.player.attack);
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
                message = "You used a potion.";
                state = BattleState::EnemyTurn;
            }
            else
            {
                message = "No potion left.";
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
                message = "Burnout... ENTER to title.";
                state = BattleState::Defeat;
            }
            else
            {
                message = TextFormat("Enemy dealt %d damage!", enemyAttack);
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
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, DARKPURPLE);
    DrawRectangle(160, 280, 100, 100, BLUE);
    DrawRectangle(700, 140, 120, 120, RED);
    DrawText("STUDENT", 160, 390, 24, WHITE);
    DrawText("ASSIGNMENT", 675, 270, 24, WHITE);
    DrawRectangle(40, 30, 880, 90, Fade(BLACK, 0.5f));
    DrawRectangle(40, 420, 880, 90, Fade(BLACK, 0.6f));
    DrawText(TextFormat("HP: %d / %d  LV:%d", data.player.hp, data.player.maxHp, data.player.level), 60, 50, 28, WHITE);
    DrawText(TextFormat("Enemy HP: %d", enemyHp), 650, 50, 28, WHITE);
    DrawText(message.c_str(), 60, 440, 24, YELLOW);
    if (state == BattleState::PlayerTurn)
    {
        DrawText("[A] Attack", 60, 475, 22, RAYWHITE);
        DrawText("[H] Heal", 220, 475, 22, RAYWHITE);
    }
    else if (state == BattleState::EnemyTurn) DrawText("Enemy is acting...", 60, 475, 22, LIGHTGRAY);
    else DrawText("[ENTER] Continue", 60, 475, 22, RAYWHITE);
}
