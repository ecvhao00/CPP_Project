#include "BattleScene.h"
#include "Game.h"
#include "FieldScene.h"
#include "TitleScene.h"
#include "raylib.h"
#include <algorithm>
#include <memory>

void BattleScene::Enter(Game& game)
{
    (void)game;
    enemyHp = 20;
    enemyAttack = 6;
    enemyTurnTimer = 0.0f;
    state = BattleState::PlayerTurn;
    message = "A boss appeared!";
}

void BattleScene::Update(Game& game, float dt)
{
    GameData& data = game.Data();

    switch (state)
    {
    case BattleState::PlayerTurn:
    {
        if (IsKeyPressed(KEY_A))
        {
            enemyHp -= data.player.attack;

            if (enemyHp <= 0)
            {
                enemyHp = 0;
                message = "You won! Press ENTER.";
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
                enemyTurnTimer = 0.0f;
            }
            else
            {
                message = "No potion left.";
            }
        }
        break;
    }

    case BattleState::EnemyTurn:
    {
        enemyTurnTimer += dt;

        if (enemyTurnTimer >= 0.6f)
        {
            data.player.hp -= enemyAttack;

            if (data.player.hp <= 0)
            {
                data.player.hp = 0;
                message = "You were defeated... Press ENTER.";
                state = BattleState::Defeat;
            }
            else
            {
                message = TextFormat("Enemy dealt %d damage!", enemyAttack);
                state = BattleState::PlayerTurn;
            }
        }
        break;
    }

    case BattleState::Victory:
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            data.bossDefeated = true;
            data.player.gold += 10;
            data.player.position = { 100.0f, 100.0f };
            game.ChangeScene(std::make_unique<FieldScene>());
        }
        break;
    }

    case BattleState::Defeat:
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            game.Data() = GameData{};
            game.ChangeScene(std::make_unique<TitleScene>());
        }
        break;
    }
    }
}

void BattleScene::Draw(Game& game)
{
    const GameData& data = game.Data();

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, DARKPURPLE);

    // 플레이어/적
    DrawRectangle(160, 280, 100, 100, BLUE);
    DrawRectangle(700, 140, 120, 120, RED);

    DrawText("PLAYER", 165, 390, 24, WHITE);
    DrawText("BOSS", 725, 270, 24, WHITE);

    // UI 패널
    DrawRectangle(40, 30, 880, 90, Fade(BLACK, 0.5f));
    DrawRectangle(40, 420, 880, 90, Fade(BLACK, 0.6f));

    DrawText(TextFormat("Player HP: %d / %d", data.player.hp, data.player.maxHp), 60, 50, 28, WHITE);
    DrawText(TextFormat("Boss HP: %d", enemyHp), 600, 50, 28, WHITE);

    DrawText(message.c_str(), 60, 440, 26, YELLOW);

    if (state == BattleState::PlayerTurn)
    {
        DrawText("[A] Attack", 60, 475, 22, RAYWHITE);
        DrawText("[H] Heal", 220, 475, 22, RAYWHITE);
    }
    else if (state == BattleState::EnemyTurn)
    {
        DrawText("Enemy is acting...", 60, 475, 22, LIGHTGRAY);
    }
    else
    {
        DrawText("[ENTER] Continue", 60, 475, 22, RAYWHITE);
    }
}