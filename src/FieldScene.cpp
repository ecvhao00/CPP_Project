#include "FieldScene.h"
#include "EngineeringScene.h"
#include "Game.h"
#include "HomeScene.h"
#include "TitleScene.h"
#include "UiWidgets.h"
#include <memory>
#include <cmath>
#include <utility>

namespace
{
constexpr float TimeTransitionDuration = 1.2f;
constexpr float TimeTransitionHalf = TimeTransitionDuration * 0.5f;

std::vector<std::string> BuildBarDialogue(int drinksTonight)
{
    return {
        TextFormat("%d차 술자리!", drinksTonight),
        "시끄러운 분위기 속에서 묘하게 쓸 만한 이야기를 들었다.",
        "술자리에 참석했다. 인맥도 +2"
    };
}

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

FieldScene::FieldScene(std::string enterMessage, bool centerMessage, bool startFadeIn)
    : enterMessage(std::move(enterMessage)), enterMessageCentered(centerMessage), startFadeIn(startFadeIn)
{
}

void FieldScene::Enter(Game& game)
{
    (void)game;
    playerMoving = false;
    showMessage = !enterMessage.empty();
    showCenterMessage = enterMessageCentered;
    messageTimer = 0.0f;
    message = enterMessage;
    dialogueActive = false;
    if (startFadeIn)
    {
        timeTransition = TimeTransition::FadeInOnly;
        transitionTimer = TimeTransitionHalf;
        transitionCommitted = true;
    }
}

void FieldScene::EndDay(Game& game)
{
    auto& s=game.Data().semester;
    if(!s.attendedClassToday) s.attendanceScore-=1;

    s.isNight=true;
    s.homeActionsUsedTonight=0;
    s.drinksTonight=0;
    s.actionPoints=s.maxActionPoints;
}
void FieldScene::NextWeek(Game& game)
{
    auto& d = game.Data();
    auto& s = d.semester;
    if (IsExamWeek(s.week) && !s.tookExamToday)
    {
        s.gameEnded = true;
        s.passed = false;
        s.endingName = "F";
        message = "시험 주에 시험을 치르지 못했다.";
        showMessage = true;
        showCenterMessage = true;
        return;
    }

    if(!s.foughtToday) s.assignmentScore-=1;

    if (s.week == 15)
    {
        s.gameEnded = true;
        s.endingName = ResolveSemesterEnding(s.devPower, s.network);
        s.passed = true;
        message = TextFormat("ENDING: %s", s.endingName.c_str());
        showMessage = true;
        showCenterMessage = false;
        return;
    }

    s.week += 1;
    s.isNight = false;
    s.foughtToday = false;
    s.tookExamToday = false;
    s.currentBattleIsExam = false;
    s.attendedClassToday = false;
    s.actionPoints = s.maxActionPoints;
    d.player.position = { 210.0f, 250.0f };
    ApplyWeekEvent(game);
}

bool FieldScene::UseActionPoint(Game& game)
{
    auto& s = game.Data().semester;
    if (s.actionPoints <= 0)
    {
        ShowFieldMessage("행동력이 부족합니다", true);
        return false;
    }
    s.actionPoints -= 1;
    return true;
}

void FieldScene::ShowFieldMessage(const char* text, bool center)
{
    message = text;
    showMessage = true;
    showCenterMessage = center;
    messageTimer = center ? 1.6f : 0.0f;
}

void FieldScene::StartTimeTransition(TimeTransition transition)
{
    if (timeTransition != TimeTransition::None) return;

    timeTransition = transition;
    transitionTimer = 0.0f;
    transitionCommitted = false;
    showMessage = false;
    showCenterMessage = false;
    messageTimer = 0.0f;
}

void FieldScene::UpdateTimeTransition(Game& game, float dt)
{
    if (timeTransition == TimeTransition::None) return;

    transitionTimer += dt;
    if (!transitionCommitted && transitionTimer >= TimeTransitionHalf)
    {
        if (timeTransition == TimeTransition::ToNight)
        {
            EndDay(game);
        }
        else if (timeTransition == TimeTransition::ToNextWeek)
        {
            NextWeek(game);
        }
        transitionCommitted = true;
    }

    if (transitionTimer >= TimeTransitionDuration)
    {
        timeTransition = TimeTransition::None;
        transitionTimer = 0.0f;
        transitionCommitted = false;
    }
}

void FieldScene::DrawFadeOverlay() const
{
    if (timeTransition == TimeTransition::None) return;

    float progress = Clamp(transitionTimer / TimeTransitionDuration, 0.0f, 1.0f);
    float alpha = progress < 0.5f ? progress * 2.0f : (1.0f - progress) * 2.0f;
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Fade(BLACK, alpha));
}

void FieldScene::BeginDialogue(DialogueSpeaker speaker, const char* speakerName, std::vector<std::string> lines)
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

void FieldScene::AdvanceDialogue()
{
    dialogueLineIndex += 1;
    if (dialogueLineIndex >= (int)dialogueLines.size())
    {
        dialogueActive = false;
        dialogueLines.clear();
        dialogueLineIndex = 0;
    }
}

void FieldScene::DrawDialoguePortrait(Game& game, Rectangle area) const
{
    if (dialogueSpeaker == DialogueSpeaker::None) return;

    ResourceManager& resources = game.Resources();
    if (dialogueSpeaker == DialogueSpeaker::ProfessorK && resources.HasProfessorKPortrait())
    {
        UiWidgets::DrawPortrait(resources.ProfessorKPortrait(), area);
        return;
    }
    if (dialogueSpeaker == DialogueSpeaker::Senior && resources.HasSenpaiPortrait())
    {
        UiWidgets::DrawPortrait(resources.SenpaiPortrait(), area);
        return;
    }

    Font& font = game.Resources().UiFont();
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

void FieldScene::DrawDialogue(Game& game)
{
    if (!dialogueActive || dialogueLines.empty()) return;

    auto& f = game.Resources().UiFont();
    bool hasPortrait = dialogueSpeaker != DialogueSpeaker::None;
    Rectangle panel = { 70, 485, 1140, 190 };
    Rectangle portrait = { 95, 515, 145, 145 };
    float textX = hasPortrait ? 270.0f : 105.0f;

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, Fade(BLACK, 0.25f));
    DrawRectangleRec(panel, Fade(BLACK, 0.88f));
    DrawRectangleLinesEx(panel, 3, RAYWHITE);
    DrawDialoguePortrait(game, portrait);

    DrawTextEx(f, dialogueName.c_str(), { textX, 510 }, 28, 1, YELLOW);
    DrawTextEx(f, dialogueLines[dialogueLineIndex].c_str(), { textX, 555 }, 29, 1, RAYWHITE);
    DrawTextEx(f, TextFormat("%d/%d", dialogueLineIndex + 1, (int)dialogueLines.size()), { 1065, 630 }, 22, 1, LIGHTGRAY);
    DrawTextEx(f, "E: 다음", { 1128, 630 }, 22, 1, LIGHTGRAY);
}

void FieldScene::ApplyWeekEvent(Game& game)
{
    auto& d = game.Data(); auto& s = d.semester;
    showCenterMessage = false;
    if (s.week == 8){ s.midtermExamDebuff = s.devPower < 12; message = s.midtermExamDebuff ? "중간고사 디버프 발생" : "중간고사 안정 통과"; showMessage = true; }
    else if (s.week == 9){ s.midtermPresentationDebuff = s.network < 10; message = s.midtermPresentationDebuff ? "중간발표 디버프 발생" : "중간발표 안정 통과"; showMessage = true; }
    else if (s.week == 14){ s.finalExamDebuff = s.devPower < 20; message = s.finalExamDebuff ? "기말고사 디버프 발생" : "기말고사 안정 통과"; showMessage = true; }
    else if (s.week == 15){ s.finalPresentationDebuff = s.network < 18; message = s.finalPresentationDebuff ? "기말발표 디버프 발생" : "기말발표 안정 통과"; showMessage = true; }
    int debuffs=(s.midtermExamDebuff?1:0)+(s.midtermPresentationDebuff?1:0)+(s.finalExamDebuff?1:0)+(s.finalPresentationDebuff?1:0);
    d.player.attack = 8 + d.player.level - debuffs; if (d.player.attack < 3) d.player.attack = 3;
}

void FieldScene::Update(Game& game, float dt)
{
    auto& d = game.Data(); auto& s = d.semester;
    if (timeTransition != TimeTransition::None)
    {
        playerMoving = false;
        UpdateTimeTransition(game, dt);
        return;
    }
    if (s.gameEnded){ playerMoving = false; if(IsKeyPressed(KEY_ENTER)){ game.Data()=GameData{}; game.ChangeScene(std::make_unique<TitleScene>());} return; }
    if (dialogueActive){ playerMoving = false; if(IsKeyPressed(KEY_E)) AdvanceDialogue(); return; }
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

    Vector2 in{0,0};
    if (IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D)) in.x +=1;
    if (IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A)) in.x -=1;
    if (IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S)) in.y +=1;
    if (IsKeyDown(KEY_UP)||IsKeyDown(KEY_W)) in.y -=1;
    playerMoving = in.x!=0||in.y!=0;
    if (playerMoving){ float l=sqrtf(in.x*in.x+in.y*in.y); in.x/=l; in.y/=l; }
    d.player.position.x += in.x*d.player.speed*dt; d.player.position.y += in.y*d.player.speed*dt;
    d.player.position.x = Clamp(d.player.position.x, 20, (float)Game::ScreenWidth-56);
    d.player.position.y = Clamp(d.player.position.y, 20, (float)Game::ScreenHeight-56);

    Rectangle p{d.player.position.x,d.player.position.y,36,36};
    bool nearEngineering=CheckCollisionRecs(p, engineeringZone), nearBar=CheckCollisionRecs(p, barZone), nearHome=CheckCollisionRecs(p, homeZone), nearNext=CheckCollisionRecs(p, nextZone);

    if (IsKeyPressed(KEY_E))
    {
        showCenterMessage = false;
        bool wantsLimitedAction = nearBar && s.isNight && s.drinksTonight < 2;
        if (wantsLimitedAction && !UseActionPoint(game)) return;

        if (nearEngineering){ game.ChangeScene(std::make_unique<EngineeringScene>()); return; }
        else if (nearBar && s.isNight && s.drinksTonight<2){ s.drinksTonight++; s.network +=2; BeginDialogue(DialogueSpeaker::None, "술자리", BuildBarDialogue(s.drinksTonight)); }
        else if (nearBar && !s.isNight){ ShowFieldMessage("낮술은 자제하세요!", true); }
        else if (nearHome && s.isNight){ game.ChangeScene(std::make_unique<HomeScene>()); return; }
        else if (nearHome && !s.isNight){ ShowFieldMessage("오자마자 가시려고요?", true); }
        else if (nearNext){ StartTimeTransition(s.isNight ? TimeTransition::ToNextWeek : TimeTransition::ToNight); }
    }
}

void FieldScene::Draw(Game& game)
{
    const auto& d=game.Data(); const auto& s=d.semester; auto& f=game.Resources().UiFont();
    DrawRectangle(0,0,Game::ScreenWidth,Game::ScreenHeight,Color{35,60,40,255});
    DrawRectangleRec(engineeringZone, BLUE);
    DrawRectangleRec(barZone, PURPLE); DrawRectangleRec(homeZone, BROWN); DrawRectangleRec(nextZone, DARKBLUE);
    UiWidgets::DrawPlayer(game.Resources(), d.player.position, playerMoving);
    DrawCenteredTextInRect(f, "공학관", engineeringZone, 32, WHITE);
    DrawCenteredTextInRect(f, "술자리", barZone, 30, WHITE);
    DrawCenteredTextInRect(f, "집", homeZone, 30, WHITE);
    DrawCenteredTextInRect(f, s.isNight ? "다음 주" : "밤으로", nextZone, 28, WHITE);

    UiWidgets::DrawTopStatus(f, d, "캠퍼스", "이동: WASD/방향키  E: 상호작용", "공학관: 수업/선배  집: 밤에 이용");
    UiWidgets::DrawBottomGraphs(f, d);

    if(showMessage && showCenterMessage)
    {
        DrawRectangle(330, 295, 620, 110, Fade(BLACK,0.85f));
        Vector2 messageSize = MeasureTextEx(f, message.c_str(), 36, 1);
        DrawTextEx(f,message.c_str(),{(Game::ScreenWidth-messageSize.x)*0.5f, 332},36,1,RAYWHITE);
    }
    else if(showMessage){ DrawRectangle(60, Game::ScreenHeight-130, Game::ScreenWidth-120, 95, Fade(BLACK,0.8f)); DrawTextEx(f,message.c_str(),{90,(float)Game::ScreenHeight-95},32,1,RAYWHITE); }
    DrawDialogue(game);
    if(s.gameEnded){ DrawRectangle(280,230,720,220,Fade(BLACK,0.85f)); DrawTextEx(f,TextFormat("%s - ENTER",s.endingName.c_str()),{460,315},52,2,YELLOW); }
    DrawFadeOverlay();
}
