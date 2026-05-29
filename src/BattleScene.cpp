#include "BattleScene.h"
#include "Game.h"
#include "FieldScene.h"
#include "TitleScene.h"
#include "raylib.h"
#include <algorithm>
#include <memory>

namespace
{
constexpr const char* AssignmentMonsterName = "스타일리시한 C 과제 괴물";
constexpr const char* MidtermName = "중간고사";
constexpr const char* FinalName = "기말고사";

bool IsMidtermBattle(const GameData& data)
{
    return data.semester.currentBattleIsExam && data.semester.week == 8;
}

bool IsFinalBattle(const GameData& data)
{
    return data.semester.currentBattleIsExam && data.semester.week == 15;
}

const char* GetOpponentName(const GameData& data)
{
    if (IsMidtermBattle(data)) return MidtermName;
    if (IsFinalBattle(data)) return FinalName;
    if (data.semester.currentBattleIsExam) return "시험";
    return AssignmentMonsterName;
}

const char* GetOpponentSubjectMarker(const GameData& data)
{
    return IsMidtermBattle(data) || IsFinalBattle(data) ? "가" : "이";
}
}

void BattleScene::Enter(Game& game)
{
    GameData& data = game.Data();
    const bool isExamBattle = data.semester.currentBattleIsExam;
    const bool isMidtermBattle = IsMidtermBattle(data);
    const bool isFinalBattle = IsFinalBattle(data);
    enemyHp = 12 + data.player.level * 4;
    enemyAttack = 3 + data.player.level;
    if (data.semester.midtermExamDebuff || data.semester.midtermPresentationDebuff || data.semester.finalExamDebuff || data.semester.finalPresentationDebuff)
    {
        enemyAttack += 2;
    }
    enemyTurnTimer = 0.0f;
    state = BattleState::PlayerTurn;
    if (isMidtermBattle) message = "중간고사가 출현했다!!";
    else if (isFinalBattle) message = "기말고사가 출현했다!!";
    else message = isExamBattle ? "시험이 시작됐다!" : TextFormat("%s이 나타났다!", AssignmentMonsterName);
}

void BattleScene::Update(Game& game, float dt)
{
    GameData& data = game.Data();
    const char* opponentName = GetOpponentName(data);
    const char* opponentSubjectMarker = GetOpponentSubjectMarker(data);

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
                message = TextFormat("%s%s 멘탈을 %d 깎았다!", opponentName, opponentSubjectMarker, enemyAttack);
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
    const bool isMidtermBattle = IsMidtermBattle(data);
    const bool isFinalBattle = IsFinalBattle(data);
    const char* opponentName = GetOpponentName(data);
    const char* opponentSubjectMarker = GetOpponentSubjectMarker(data);
    auto& f = game.Resources().UiFont();

    Rectangle topPanel = { 40, 30, (float)Game::ScreenWidth - 80, 92 };
    Rectangle bottomPanel = { 0, 520, (float)Game::ScreenWidth, 200 };
    Rectangle playerBody = { 190, 270, 250, 250 };
    Rectangle enemyBody = { 915, 185, 140, 140 };
    Rectangle examBody = { enemyBody.x - 100.0f, enemyBody.y, enemyBody.width, enemyBody.height };

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, LIGHTGRAY);
    if (game.Resources().HasPlayerBattleSprite())
    {
        Texture2D& playerSprite = game.Resources().PlayerBattleSprite();
        Rectangle source = { 0, 0, (float)playerSprite.width, (float)playerSprite.height };
        float scale = playerBody.width / (float)playerSprite.width;
        Rectangle destination = {
            playerBody.x,
            bottomPanel.y - (float)playerSprite.height * scale,
            (float)playerSprite.width * scale,
            (float)playerSprite.height * scale
        };
        DrawTexturePro(playerSprite, source, destination, { 0, 0 }, 0.0f, WHITE);
    }
    else
    {
        DrawRectangleRec(playerBody, BLUE);
    }
    Vector2 enemyLabelPosition = { enemyBody.x + 42, enemyBody.y + enemyBody.height + 12 };
    if (isFinalBattle && game.Resources().HasFinalSprite())
    {
        Texture2D& enemySprite = game.Resources().FinalSprite();
        float finalScale = 1.5f;
        Rectangle finalDestination = {
            examBody.x - 15.0f,
            examBody.y - 2.0f,
            (float)enemySprite.width * finalScale,
            (float)enemySprite.height * finalScale
        };

        if (game.Resources().HasAssignmentMonsterSprite())
        {
            Texture2D& decorSprite = game.Resources().AssignmentMonsterSprite();
            DrawTexture(decorSprite, 635, 215, Fade(WHITE, 0.9f));
            DrawTexture(decorSprite, 1030, 215, Fade(WHITE, 0.9f));
        }

        Rectangle source = { 0, 0, (float)enemySprite.width, (float)enemySprite.height };
        DrawTexturePro(enemySprite, source, finalDestination, { 0, 0 }, 0.0f, WHITE);

        Vector2 enemyNameSize = MeasureTextEx(f, opponentName, 28, 1);
        enemyLabelPosition = {
            finalDestination.x + (finalDestination.width - enemyNameSize.x) * 0.5f,
            finalDestination.y + finalDestination.height + 12
        };
    }
    else if (isMidtermBattle && game.Resources().HasMidtermSprite())
    {
        Texture2D& enemySprite = game.Resources().MidtermSprite();
        DrawTexture(enemySprite, (int)examBody.x, (int)examBody.y, WHITE);

        Vector2 enemyNameSize = MeasureTextEx(f, opponentName, 28, 1);
        enemyLabelPosition = {
            examBody.x + ((float)enemySprite.width - enemyNameSize.x) * 0.5f,
            examBody.y + (float)enemySprite.height + 12
        };
    }
    else if (!isExamBattle && game.Resources().HasAssignmentMonsterSprite())
    {
        Texture2D& enemySprite = game.Resources().AssignmentMonsterSprite();
        DrawTexture(enemySprite, (int)enemyBody.x, (int)enemyBody.y, WHITE);

        Vector2 enemyNameSize = MeasureTextEx(f, AssignmentMonsterName, 24, 1);
        enemyLabelPosition = {
            enemyBody.x + ((float)enemySprite.width - enemyNameSize.x) * 0.5f,
            enemyBody.y + (float)enemySprite.height + 12
        };
    }
    else
    {
        DrawRectangleRec(enemyBody, RED);
    }
    DrawTextEx(f, opponentName, enemyLabelPosition, isExamBattle ? 28 : 24, 1, WHITE);

    DrawRectangleRec(topPanel, Fade(BLACK, 0.55f));
    DrawRectangleLinesEx(topPanel, 2, Fade(RAYWHITE, 0.7f));
    DrawTextEx(f, TextFormat("멘탈 %d/%d", data.player.hp, data.player.maxHp), {70, 50}, 30, 1, WHITE);
    DrawTextEx(f, TextFormat("레벨 %d  공격 %d", data.player.level, data.player.attack), {70, 86}, 22, 1, LIGHTGRAY);
    DrawTextEx(f, "VS", {618, 60}, 38, 2, YELLOW);
    DrawTextEx(f, TextFormat("%s 체력 %d", opponentName, enemyHp), {930, 50}, isExamBattle ? 30 : 22, 1, WHITE);
    DrawTextEx(f, TextFormat("반격 피해 %d", enemyAttack), {930, 86}, 22, 1, LIGHTGRAY);

    DrawRectangleRec(bottomPanel, Fade(BLACK, 0.78f));
    DrawRectangle(0, 520, Game::ScreenWidth, 3, Fade(RAYWHITE, 0.45f));
    DrawTextEx(f, message.c_str(), {70, 550}, 28, 1, YELLOW);
    if (state == BattleState::PlayerTurn)
    {
        DrawTextEx(f, "[A] 공격", {70, 630}, 26, 1, RAYWHITE);
        DrawTextEx(f, "[H] 회복", {240, 630}, 26, 1, RAYWHITE);
    }
    else if (state == BattleState::EnemyTurn) DrawTextEx(f, TextFormat("%s%s 반격하는 중...", opponentName, opponentSubjectMarker), {70, 630}, 26, 1, LIGHTGRAY);
    else DrawTextEx(f, "[ENTER] 계속", {70, 630}, 26, 1, RAYWHITE);
}
