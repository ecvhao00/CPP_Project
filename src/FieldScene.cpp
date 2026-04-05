#include "FieldScene.h"
#include "Game.h"
#include "BattleScene.h"
#include "raylib.h"
#include <cmath>
#include <memory>

FieldScene::FieldScene()
{
    // 벽 몇 개만 대충 배치
    walls.push_back({ 150, 80, 30, 300 });
    walls.push_back({ 400, 0, 30, 220 });
    walls.push_back({ 400, 320, 30, 220 });
    walls.push_back({ 560, 140, 220, 30 });
    walls.push_back({ 560, 260, 30, 200 });
}

void FieldScene::Enter(Game& game)
{
    (void)game;
    showDialog = false;
    touchingNpc = false;
}

bool FieldScene::CheckWallCollision(const Rectangle& rect) const
{
    if (rect.x < 0) return true;
    if (rect.y < 0) return true;
    if (rect.x + rect.width > Game::ScreenWidth) return true;
    if (rect.y + rect.height > Game::ScreenHeight) return true;

    for (const Rectangle& wall : walls)
    {
        if (CheckCollisionRecs(rect, wall))
        {
            return true;
        }
    }

    return false;
}

void FieldScene::ResolveMovement(Rectangle& playerRect, const Vector2& move)
{
    Rectangle nextX = playerRect;
    nextX.x += move.x;
    if (!CheckWallCollision(nextX))
    {
        playerRect.x = nextX.x;
    }

    Rectangle nextY = playerRect;
    nextY.y += move.y;
    if (!CheckWallCollision(nextY))
    {
        playerRect.y = nextY.y;
    }
}

void FieldScene::Update(Game& game, float dt)
{
    GameData& data = game.Data();

    Rectangle playerRect = {
        data.player.position.x,
        data.player.position.y,
        36,
        36
    };

    Vector2 input = { 0.0f, 0.0f };

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) input.x += 1.0f;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) input.x -= 1.0f;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) input.y += 1.0f;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) input.y -= 1.0f;

    if (input.x != 0.0f || input.y != 0.0f)
    {
        float len = sqrtf(input.x * input.x + input.y * input.y);
        input.x /= len;
        input.y /= len;
    }

    Vector2 move = {
        input.x * data.player.speed * dt,
        input.y * data.player.speed * dt
    };

    ResolveMovement(playerRect, move);

    data.player.position = { playerRect.x, playerRect.y };

    Rectangle talkArea = {
        npc.x - 24,
        npc.y - 24,
        npc.width + 48,
        npc.height + 48
    };

    touchingNpc = CheckCollisionRecs(playerRect, talkArea);

    if (!touchingNpc)
    {
        showDialog = false;
    }
    else if (IsKeyPressed(KEY_E))
    {
        showDialog = !showDialog;
    }

    if (!data.bossDefeated && CheckCollisionRecs(playerRect, enemy))
    {
        game.ChangeScene(std::make_unique<BattleScene>());
        return;
    }

    if (data.bossDefeated && CheckCollisionRecs(playerRect, exitZone) && IsKeyPressed(KEY_E))
    {
        game.RequestQuit();
        return;
    }
}

void FieldScene::Draw(Game& game)
{
    const GameData& data = game.Data();

    // 바닥
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Color{ 30, 60, 30, 255 });

    // 벽
    for (const Rectangle& wall : walls)
    {
        DrawRectangleRec(wall, DARKBROWN);
    }

    // 출구
    if (data.bossDefeated)
    {
        DrawRectangleRec(exitZone, SKYBLUE);
        DrawText("EXIT", (int)exitZone.x - 2, (int)exitZone.y - 24, 20, WHITE);
    }

    // NPC
    DrawRectangleRec(npc, ORANGE);
    DrawText("NPC", (int)npc.x - 2, (int)npc.y - 22, 18, WHITE);

    // 적
    if (!data.bossDefeated)
    {
        DrawRectangleRec(enemy, RED);
        DrawText("BOSS", (int)enemy.x - 2, (int)enemy.y - 22, 18, WHITE);
    }

    // 플레이어
    Rectangle playerRect = {
        data.player.position.x,
        data.player.position.y,
        36,
        36
    };
    DrawRectangleRec(playerRect, BLUE);

    // UI
    DrawRectangle(10, 10, 230, 90, Fade(BLACK, 0.5f));
    DrawText(TextFormat("HP: %d / %d", data.player.hp, data.player.maxHp), 20, 20, 24, WHITE);
    DrawText(TextFormat("ATK: %d", data.player.attack), 20, 50, 20, WHITE);
    DrawText(TextFormat("Potion: %s", data.hasPotion ? "Yes" : "No"), 20, 72, 20, WHITE);

    if (touchingNpc)
    {
        DrawText("Press E to talk", 350, 500, 22, YELLOW);
    }

    if (showDialog)
    {
        DrawRectangle(140, 390, 680, 100, Fade(BLACK, 0.8f));
        DrawText("Professor: boss를 잡고 출구로 가라.", 170, 420, 24, RAYWHITE);
    }

    if (data.bossDefeated)
    {
        DrawText("Boss defeated! Go to blue exit and press E.", 520, 20, 20, YELLOW);
    }
}