#include "HomeScene.h"
#include "BattleScene.h"
#include "FieldScene.h"
#include "Game.h"
#include "TitleScene.h"
#include "UiWidgets.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

namespace
{
constexpr int GameMentalRecovery = 10;
constexpr int SleepMentalRecovery = 4;
constexpr float SleepTransitionDuration = 0.6f;

void DrawCenteredTextInRect(Font& font, const char* text, Rectangle rect, float fontSize, Color color)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, 1);
    DrawTextEx(font, text, { rect.x + (rect.width - size.x) * 0.5f, rect.y + (rect.height - size.y) * 0.5f }, fontSize, 1, color);
}

const char* ResolveSemesterEnding(int devPower, int network)
{
    if (devPower >= 50 && network >= 35) return "A+";
    if (devPower >= 30 && network >= 20) return "B+";
    return "C";
}

bool IsExamWeek(int week)
{
    return week == 8 || week == 15;
}
}

void HomeScene::Enter(Game& game)
{
    (void)game;
    playerMoving = false;
    showMessage = false;
    showCenterMessage = false;
    sleepTransitionActive = false;
    sleepTransitionTimer = 0.0f;
    dialogueActive = false;
    playerPosition = { 600.0f, 520.0f };
}

bool HomeScene::UseHomeAction(Game& game)
{
    auto& s = game.Data().semester;
    if (s.actionPoints <= 0)
    {
        ShowMessage("행동력이 부족합니다", true);
        return false;
    }

    s.homeActionsUsedTonight += 1;
    s.actionPoints -= 1;
    return true;
}

int HomeScene::RecoverMental(Game& game, int amount)
{
    auto& player = game.Data().player;
    int before = player.hp;
    player.hp = std::min(player.maxHp, player.hp + amount);
    return player.hp - before;
}

std::string HomeScene::StartNextWeek(Game& game)
{
    auto& d = game.Data();
    auto& s = d.semester;
    if (IsExamWeek(s.week) && !s.tookExamToday)
    {
        s.gameEnded = true;
        s.passed = false;
        s.endingName = "F";
        return "시험 주에 시험을 치르지 못했다.";
    }

    if (!s.foughtToday)
    {
        s.assignmentMisses += 1;
        if (s.assignmentMisses >= 5)
        {
            s.gameEnded = true;
            s.passed = false;
            s.endingName = "F";
            return "과제 미제출 5회로 F학점입니다.";
        }
    }

    if (s.week == 15)
    {
        s.gameEnded = true;
        s.endingName = ResolveSemesterEnding(s.devPower, s.network);
        s.passed = true;
        return TextFormat("ENDING: %s", s.endingName.c_str());
    }

    s.week += 1;
    s.isNight = false;
    s.foughtToday = false;
    s.tookExamToday = false;
    s.currentBattleIsExam = false;
    s.attendedClassToday = false;
    s.homeActionsUsedTonight = 0;
    s.drinksTonight = 0;
    s.actionPoints = s.maxActionPoints;
    d.player.position = { 210.0f, 250.0f };
    return ApplyWeekEvent(game);
}

std::string HomeScene::ApplyWeekEvent(Game& game)
{
    auto& d = game.Data();
    auto& s = d.semester;
    std::string eventMessage = "";

    if (s.week == 8)
    {
        s.midtermExamDebuff = s.devPower < 12;
        eventMessage = s.midtermExamDebuff ? "중간고사 디버프 발생" : "중간고사 안정 통과";
    }
    else if (s.week == 9)
    {
        s.midtermPresentationDebuff = s.network < 10;
        eventMessage = s.midtermPresentationDebuff ? "중간발표 디버프 발생" : "중간발표 안정 통과";
    }
    else if (s.week == 14)
    {
        s.finalExamDebuff = s.devPower < 20;
        eventMessage = s.finalExamDebuff ? "기말고사 디버프 발생" : "기말고사 안정 통과";
    }
    else if (s.week == 15)
    {
        s.finalPresentationDebuff = s.network < 18;
        eventMessage = s.finalPresentationDebuff ? "기말발표 디버프 발생" : "기말발표 안정 통과";
    }

    int debuffs = (s.midtermExamDebuff ? 1 : 0) + (s.midtermPresentationDebuff ? 1 : 0) + (s.finalExamDebuff ? 1 : 0) + (s.finalPresentationDebuff ? 1 : 0);
    d.player.attack = 8 + d.player.level - debuffs;
    if (d.player.attack < 3) d.player.attack = 3;

    return eventMessage;
}

void HomeScene::StartSleepTransition()
{
    if (sleepTransitionActive) return;

    sleepTransitionActive = true;
    sleepTransitionTimer = 0.0f;
    showMessage = false;
    showCenterMessage = false;
}

void HomeScene::UpdateSleepTransition(Game& game, float dt)
{
    sleepTransitionTimer += dt;
    if (sleepTransitionTimer < SleepTransitionDuration) return;

    int recovered = RecoverMental(game, SleepMentalRecovery);
    std::string eventMessage = StartNextWeek(game);
    std::string sleepMessage = TextFormat("잠을 자고 멘탈을 %d 회복했다. 다음 주가 시작되었다.", recovered);
    if (!eventMessage.empty()) sleepMessage = eventMessage;
    game.ChangeScene(std::make_unique<FieldScene>(sleepMessage, false, true));
}

void HomeScene::DrawFadeOverlay() const
{
    if (!sleepTransitionActive) return;

    float alpha = Clamp(sleepTransitionTimer / SleepTransitionDuration, 0.0f, 1.0f);
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Fade(BLACK, alpha));
}

void HomeScene::ShowMessage(const std::string& text, bool center)
{
    message = text;
    showMessage = true;
    showCenterMessage = center;
}

void HomeScene::BeginDialogue(std::vector<std::string> lines)
{
    if (lines.empty()) return;

    dialogueActive = true;
    dialogueLines = std::move(lines);
    dialogueLineIndex = 0;
    showMessage = false;
    showCenterMessage = false;
}

void HomeScene::AdvanceDialogue()
{
    dialogueLineIndex += 1;
    if (dialogueLineIndex >= (int)dialogueLines.size())
    {
        dialogueActive = false;
        dialogueLines.clear();
        dialogueLineIndex = 0;
    }
}

void HomeScene::Update(Game& game, float dt)
{
    auto& d = game.Data();
    auto& s = d.semester;

    if (sleepTransitionActive)
    {
        playerMoving = false;
        UpdateSleepTransition(game, dt);
        return;
    }
    if (s.gameEnded)
    {
        playerMoving = false;
        if (IsKeyPressed(KEY_ENTER))
        {
            game.Data() = GameData{};
            game.ChangeScene(std::make_unique<TitleScene>());
        }
        return;
    }
    if (dialogueActive)
    {
        playerMoving = false;
        if (IsKeyPressed(KEY_E)) AdvanceDialogue();
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        playerMoving = false;
        game.ChangeScene(std::make_unique<FieldScene>());
        return;
    }

    Vector2 input{0,0};
    if (IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D)) input.x +=1;
    if (IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A)) input.x -=1;
    if (IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S)) input.y +=1;
    if (IsKeyDown(KEY_UP)||IsKeyDown(KEY_W)) input.y -=1;
    playerMoving = input.x!=0||input.y!=0;
    if (playerMoving){ float l=sqrtf(input.x*input.x+input.y*input.y); input.x/=l; input.y/=l; }

    playerPosition.x += input.x*d.player.speed*dt;
    playerPosition.y += input.y*d.player.speed*dt;
    playerPosition.x = Clamp(playerPosition.x, 20, (float)Game::ScreenWidth-56);
    playerPosition.y = Clamp(playerPosition.y, 20, (float)Game::ScreenHeight-56);

    Rectangle playerRect{playerPosition.x, playerPosition.y, 36, 36};
    bool nearGame = CheckCollisionRecs(playerRect, gameZone);
    bool nearSleep = CheckCollisionRecs(playerRect, sleepZone);
    bool nearAssignment = CheckCollisionRecs(playerRect, assignmentZone);
    bool nearSchool = CheckCollisionRecs(playerRect, schoolZone);

    if (IsKeyPressed(KEY_E))
    {
        showCenterMessage = false;
        if (nearGame)
        {
            if (!UseHomeAction(game)) return;
            int recovered = RecoverMental(game, GameMentalRecovery);
            BeginDialogue({
                "게임을 하며 머리를 식혔다.",
                TextFormat("멘탈을 %d 회복했다.", recovered)
            });
        }
        else if (nearSleep)
        {
            StartSleepTransition();
        }
        else if (nearAssignment)
        {
            if (!UseHomeAction(game)) return;
            s.currentBattleIsExam = false;
            game.ChangeScene(std::make_unique<BattleScene>());
            return;
        }
        else if (nearSchool)
        {
            d.player.position = { 360.0f, 545.0f };
            game.ChangeScene(std::make_unique<FieldScene>());
            return;
        }
    }
}

void HomeScene::DrawDialogue(Game& game)
{
    if (!dialogueActive || dialogueLines.empty()) return;

    auto& f = game.Resources().UiFont();
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Fade(BLACK, 0.25f));
    DrawRectangle(70, 485, 1140, 190, Fade(BLACK, 0.88f));
    DrawRectangleLinesEx({70, 485, 1140, 190}, 3, RAYWHITE);
    DrawTextEx(f, "집", {105, 510}, 28, 1, YELLOW);
    DrawTextEx(f, dialogueLines[dialogueLineIndex].c_str(), {105, 555}, 29, 1, RAYWHITE);
    DrawTextEx(f, TextFormat("%d/%d", dialogueLineIndex + 1, (int)dialogueLines.size()), {1065, 630}, 22, 1, LIGHTGRAY);
    DrawTextEx(f, "E: 다음", {1128, 630}, 22, 1, LIGHTGRAY);
}

void HomeScene::Draw(Game& game)
{
    const auto& d = game.Data();
    auto& f = game.Resources().UiFont();

    DrawRectangle(0,0,Game::ScreenWidth,Game::ScreenHeight,Color{54,45,58,255});
    DrawRectangleRec(gameZone, Color{60, 105, 180, 255});
    DrawRectangleRec(sleepZone, Color{80, 80, 120, 255});
    DrawRectangleRec(assignmentZone, Color{145, 70, 70, 255});
    DrawRectangleRec(schoolZone, Color{48, 112, 92, 255});
    DrawCenteredTextInRect(f, "게임하기", gameZone, 32, WHITE);
    DrawCenteredTextInRect(f, "자기", sleepZone, 32, WHITE);
    DrawCenteredTextInRect(f, "과제 수행하기", assignmentZone, 32, WHITE);
    DrawCenteredTextInRect(f, "학교 가기", schoolZone, 32, WHITE);
    UiWidgets::DrawPlayer(game.Resources(), playerPosition, playerMoving);

    UiWidgets::DrawTopStatus(f, d, "집", "이동: WASD/방향키  E: 선택", "ESC: 밖으로");
    UiWidgets::DrawBottomGraphs(f, d);

    if(showMessage && showCenterMessage)
    {
        DrawRectangle(330, 295, 620, 110, Fade(BLACK,0.85f));
        Vector2 messageSize = MeasureTextEx(f, message.c_str(), 36, 1);
        DrawTextEx(f,message.c_str(),{(Game::ScreenWidth-messageSize.x)*0.5f, 332},36,1,RAYWHITE);
    }
    else if(showMessage)
    {
        DrawRectangle(60, Game::ScreenHeight-130, Game::ScreenWidth-120, 95, Fade(BLACK,0.8f));
        DrawTextEx(f,message.c_str(),{90,(float)Game::ScreenHeight-95},32,1,RAYWHITE);
    }

    DrawDialogue(game);
    DrawFadeOverlay();
}
