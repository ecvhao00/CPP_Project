#include "EngineeringScene.h"
#include "BattleScene.h"
#include "FieldScene.h"
#include "Game.h"
#include "UiWidgets.h"
#include <cmath>
#include <memory>
#include <utility>

namespace
{
const std::vector<std::string> ProfessorKClassDialogue = {
    "프로페서 K: 오늘은 코드가 왜 터지는지부터 보자.",
    "프로페서 K: 좋은 개발자는 에러 메시지를 무시하지 않는다.",
    "수업을 들었다. 개발력 +2"
};

const std::vector<std::string> SeniorSocialDialogue = {
    "선배: 막히면 혼자 끙끙대지 말고 물어봐.",
    "선배: 대신 질문하기 전에 어디까지 봤는지는 정리해두고.",
    "선배와 대화했다. 인맥도 +1"
};

void DrawCenteredTextInRect(Font& font, const char* text, Rectangle rect, float fontSize, Color color)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, 1);
    DrawTextEx(font, text, { rect.x + (rect.width - size.x) * 0.5f, rect.y + (rect.height - size.y) * 0.5f }, fontSize, 1, color);
}

bool IsExamWeek(int week)
{
    return week == 8 || week == 15;
}
}

void EngineeringScene::Enter(Game& game)
{
    (void)game;
    showMessage = false;
    showCenterMessage = false;
    messageTimer = 0.0f;
    dialogueActive = false;
    playerPosition = { 600.0f, 520.0f };
}

bool EngineeringScene::UseActionPoint(Game& game)
{
    auto& s = game.Data().semester;
    if (s.actionPoints <= 0)
    {
        ShowMessage("행동력이 부족합니다", true);
        return false;
    }

    s.actionPoints -= 1;
    return true;
}

void EngineeringScene::ShowMessage(const std::string& text, bool center)
{
    message = text;
    showMessage = true;
    showCenterMessage = center;
    messageTimer = center ? 1.6f : 0.0f;
}

void EngineeringScene::BeginDialogue(DialogueSpeaker speaker, const char* speakerName, std::vector<std::string> lines)
{
    if (lines.empty()) return;

    dialogueActive = true;
    dialogueSpeaker = speaker;
    dialogueName = speakerName;
    dialogueLines = std::move(lines);
    dialogueLineIndex = 0;
    showMessage = false;
    showCenterMessage = false;
}

void EngineeringScene::AdvanceDialogue()
{
    dialogueLineIndex += 1;
    if (dialogueLineIndex >= (int)dialogueLines.size())
    {
        dialogueActive = false;
        dialogueLines.clear();
        dialogueLineIndex = 0;
    }
}

void EngineeringScene::Update(Game& game, float dt)
{
    auto& d = game.Data();
    auto& s = d.semester;

    if (dialogueActive)
    {
        if (IsKeyPressed(KEY_E)) AdvanceDialogue();
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        d.player.position = { 210.0f, 250.0f };
        game.ChangeScene(std::make_unique<FieldScene>());
        return;
    }
    if (messageTimer > 0.0f)
    {
        messageTimer -= dt;
        if (messageTimer <= 0.0f)
        {
            messageTimer = 0.0f;
            showMessage = false;
            showCenterMessage = false;
        }
    }

    Vector2 input{0,0};
    if (IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D)) input.x +=1;
    if (IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A)) input.x -=1;
    if (IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S)) input.y +=1;
    if (IsKeyDown(KEY_UP)||IsKeyDown(KEY_W)) input.y -=1;
    if (input.x!=0||input.y!=0){ float l=sqrtf(input.x*input.x+input.y*input.y); input.x/=l; input.y/=l; }

    playerPosition.x += input.x*d.player.speed*dt;
    playerPosition.y += input.y*d.player.speed*dt;
    playerPosition.x = Clamp(playerPosition.x, 20, (float)Game::ScreenWidth-56);
    playerPosition.y = Clamp(playerPosition.y, 20, (float)Game::ScreenHeight-56);

    Rectangle playerRect{playerPosition.x, playerPosition.y, 36, 36};
    bool nearClass = CheckCollisionRecs(playerRect, classZone);
    bool nearSenior = CheckCollisionRecs(playerRect, seniorZone);
    bool nearExam = IsExamWeek(s.week) && CheckCollisionRecs(playerRect, examZone);
    bool nearExit = CheckCollisionRecs(playerRect, exitZone);

    if (IsKeyPressed(KEY_E))
    {
        showCenterMessage = false;
        if (nearClass)
        {
            if (s.isNight)
            {
                ShowMessage("밤에는 수업이 없습니다.", true);
                return;
            }
            if (!UseActionPoint(game)) return;
            s.attendedClassToday = true;
            s.devPower += 2;
            BeginDialogue(DialogueSpeaker::ProfessorK, "프로페서 K", ProfessorKClassDialogue);
        }
        else if (nearSenior)
        {
            if (!UseActionPoint(game)) return;
            s.network += 1;
            BeginDialogue(DialogueSpeaker::Senior, "선배", SeniorSocialDialogue);
        }
        else if (nearExam)
        {
            if (!UseActionPoint(game)) return;
            s.currentBattleIsExam = true;
            game.ChangeScene(std::make_unique<BattleScene>());
            return;
        }
        else if (nearExit)
        {
            d.player.position = { 210.0f, 250.0f };
            game.ChangeScene(std::make_unique<FieldScene>());
            return;
        }
    }
}

void EngineeringScene::DrawDialoguePortrait(Font& font, Rectangle area) const
{
    if (dialogueSpeaker == DialogueSpeaker::None) return;

    Color background = dialogueSpeaker == DialogueSpeaker::ProfessorK ? DARKBLUE : DARKGREEN;
    Color jacket = dialogueSpeaker == DialogueSpeaker::ProfessorK ? DARKGRAY : BLUE;
    const char* label = dialogueSpeaker == DialogueSpeaker::ProfessorK ? "K" : "선배";

    DrawRectangleRec(area, background);
    DrawRectangleLinesEx(area, 3, RAYWHITE);
    DrawCircle((int)(area.x + area.width * 0.5f), (int)(area.y + 46), 31, Color{238, 196, 154, 255});
    DrawRectangle((int)(area.x + 41), (int)(area.y + 78), 66, 48, jacket);

    if (dialogueSpeaker == DialogueSpeaker::ProfessorK)
    {
        DrawRectangle((int)(area.x + 42), (int)(area.y + 16), 57, 18, BLACK);
        DrawCircleLines((int)(area.x + 52), (int)(area.y + 45), 9, BLACK);
        DrawCircleLines((int)(area.x + 78), (int)(area.y + 45), 9, BLACK);
        DrawLine((int)(area.x + 61), (int)(area.y + 45), (int)(area.x + 69), (int)(area.y + 45), BLACK);
    }
    else
    {
        DrawRectangle((int)(area.x + 33), (int)(area.y + 20), 72, 15, BLACK);
        DrawCircle((int)(area.x + 53), (int)(area.y + 45), 4, BLACK);
        DrawCircle((int)(area.x + 78), (int)(area.y + 45), 4, BLACK);
    }

    Vector2 labelSize = MeasureTextEx(font, label, 22, 1);
    DrawTextEx(font, label, { area.x + (area.width - labelSize.x) * 0.5f, area.y + 132 }, 22, 1, RAYWHITE);
}

void EngineeringScene::DrawDialogue(Game& game)
{
    if (!dialogueActive || dialogueLines.empty()) return;

    auto& f = game.Resources().UiFont();
    Rectangle panel = { 70, 485, 1140, 190 };
    Rectangle portrait = { 95, 515, 145, 145 };
    float textX = dialogueSpeaker != DialogueSpeaker::None ? 270.0f : 105.0f;

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Fade(BLACK, 0.25f));
    DrawRectangleRec(panel, Fade(BLACK, 0.88f));
    DrawRectangleLinesEx(panel, 3, RAYWHITE);
    DrawDialoguePortrait(f, portrait);

    DrawTextEx(f, dialogueName.c_str(), { textX, 510 }, 28, 1, YELLOW);
    DrawTextEx(f, dialogueLines[dialogueLineIndex].c_str(), { textX, 555 }, 29, 1, RAYWHITE);
    DrawTextEx(f, TextFormat("%d/%d", dialogueLineIndex + 1, (int)dialogueLines.size()), {1065, 630}, 22, 1, LIGHTGRAY);
    DrawTextEx(f, "E: 다음", {1128, 630}, 22, 1, LIGHTGRAY);
}

void EngineeringScene::Draw(Game& game)
{
    const auto& d = game.Data();
    auto& f = game.Resources().UiFont();

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Color{42, 51, 64, 255});
    DrawRectangleRec(classZone, BLUE);
    DrawRectangleRec(seniorZone, ORANGE);
    if (IsExamWeek(d.semester.week)) DrawRectangleRec(examZone, Color{185, 145, 55, 255});
    DrawRectangleRec(exitZone, DARKBLUE);
    DrawCenteredTextInRect(f, "수업", classZone, 32, WHITE);
    DrawCenteredTextInRect(f, "선배", seniorZone, 32, WHITE);
    if (IsExamWeek(d.semester.week)) DrawCenteredTextInRect(f, "시험", examZone, 32, WHITE);
    DrawCenteredTextInRect(f, "밖으로", exitZone, 30, WHITE);
    DrawRectangle((int)playerPosition.x, (int)playerPosition.y, 36, 36, SKYBLUE);

    UiWidgets::DrawTopStatus(f, d, "공학관", "이동: WASD/방향키  E: 선택", "ESC: 밖으로");
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
}
