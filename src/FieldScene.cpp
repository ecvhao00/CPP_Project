#include "FieldScene.h"
#include "BattleScene.h"
#include "Game.h"
#include "HomeScene.h"
#include "TitleScene.h"
#include <memory>
#include <cmath>
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

std::vector<std::string> BuildBarDialogue(int drinksTonight)
{
    return {
        TextFormat("%d차 술자리!", drinksTonight),
        "시끄러운 분위기 속에서 묘하게 쓸 만한 이야기를 들었다.",
        "술자리에 참석했다. 인맥도 +2"
    };
}
}

FieldScene::FieldScene(std::string enterMessage, bool centerMessage)
    : enterMessage(std::move(enterMessage)), enterMessageCentered(centerMessage)
{
}

void FieldScene::Enter(Game& game)
{
    (void)game;
    showMessage = !enterMessage.empty();
    showCenterMessage = enterMessageCentered;
    message = enterMessage;
    dialogueActive = false;
}

void FieldScene::EndDay(Game& game)
{
    auto& s=game.Data().semester;
    if ((s.week == 8 || s.week == 15) && !s.foughtToday)
    {
        s.gameEnded = true;
        s.passed = false;
        s.endingName = "F";
        message = "시험 주에 과제를 수행하지 못했다.";
        showMessage = true;
        showCenterMessage = true;
        return;
    }

    if(!s.foughtToday) s.assignmentScore-=1;
    if(!s.attendedClassToday) s.attendanceScore-=1;

    if (s.week == 15)
    {
        s.gameEnded = true;
        s.passed = !(s.finalPresentationDebuff||s.finalExamDebuff||s.midtermExamDebuff||s.midtermPresentationDebuff)&&s.assignmentScore>0&&s.attendanceScore>0;
        s.endingName = s.passed ? "PASS" : "FAIL";
        message = s.passed ? "학기 종료! PASS" : "학기 종료... FAIL";
        showMessage = true;
        showCenterMessage = false;
        return;
    }

    s.isNight=true;
    s.homeActionsUsedTonight=0;
    s.drinksTonight=0;
    s.actionPoints=s.maxActionPoints;
}
void FieldScene::NextWeek(Game& game){ auto& s=game.Data().semester; s.week+=1; s.isNight=false; s.foughtToday=false; s.attendedClassToday=false; s.actionPoints=s.maxActionPoints; ApplyWeekEvent(game);} 

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

void FieldScene::DrawDialoguePortrait(Font& font, Rectangle area) const
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
    DrawDialoguePortrait(f, portrait);

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
    if (s.gameEnded){ if(IsKeyPressed(KEY_ENTER)){ game.Data()=GameData{}; game.ChangeScene(std::make_unique<TitleScene>());} return; }
    if (dialogueActive){ if(IsKeyPressed(KEY_E)) AdvanceDialogue(); return; }

    Vector2 in{0,0};
    if (IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D)) in.x +=1;
    if (IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A)) in.x -=1;
    if (IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S)) in.y +=1;
    if (IsKeyDown(KEY_UP)||IsKeyDown(KEY_W)) in.y -=1;
    if (in.x!=0||in.y!=0){ float l=sqrtf(in.x*in.x+in.y*in.y); in.x/=l; in.y/=l; }
    d.player.position.x += in.x*d.player.speed*dt; d.player.position.y += in.y*d.player.speed*dt;
    d.player.position.x = Clamp(d.player.position.x, 20, (float)Game::ScreenWidth-56);
    d.player.position.y = Clamp(d.player.position.y, 20, (float)Game::ScreenHeight-56);

    Rectangle p{d.player.position.x,d.player.position.y,36,36};
    bool nearClass=CheckCollisionRecs(p, classZone), nearHelper=CheckCollisionRecs(p, helperZone), nearBattle=CheckCollisionRecs(p, battleZone), nearBar=CheckCollisionRecs(p, barZone), nearHome=CheckCollisionRecs(p, homeZone), nearNext=CheckCollisionRecs(p, nextZone);

    if (IsKeyPressed(KEY_E))
    {
        showCenterMessage = false;
        bool wantsLimitedAction = (nearClass && !s.isNight) || nearHelper || nearBattle || (nearBar && s.isNight && s.drinksTonight < 2);
        if (wantsLimitedAction && !UseActionPoint(game)) return;

        if (nearClass && !s.isNight){ s.attendedClassToday=true; s.devPower +=2; BeginDialogue(DialogueSpeaker::ProfessorK, "프로페서 K", ProfessorKClassDialogue); }
        else if (nearHelper){ s.network +=1; BeginDialogue(DialogueSpeaker::Senior, "선배", SeniorSocialDialogue); }
        else if (nearBattle){ game.ChangeScene(std::make_unique<BattleScene>()); return; }
        else if (nearBar && s.isNight && s.drinksTonight<2){ s.drinksTonight++; s.network +=2; BeginDialogue(DialogueSpeaker::None, "술자리", BuildBarDialogue(s.drinksTonight)); }
        else if (nearHome && s.isNight){ game.ChangeScene(std::make_unique<HomeScene>()); return; }
        else if (nearNext){ if(!s.isNight) EndDay(game); else NextWeek(game); }
    }
}

void FieldScene::Draw(Game& game)
{
    const auto& d=game.Data(); const auto& s=d.semester; auto& f=game.Resources().UiFont();
    DrawRectangle(0,0,Game::ScreenWidth,Game::ScreenHeight,Color{35,60,40,255});
    DrawRectangleRec(classZone, BLUE); DrawRectangleRec(helperZone, ORANGE); DrawRectangleRec(battleZone, RED); DrawRectangleRec(barZone, PURPLE); DrawRectangleRec(homeZone, BROWN); DrawRectangleRec(nextZone, DARKBLUE);
    DrawRectangle((int)d.player.position.x,(int)d.player.position.y,36,36, SKYBLUE);
    DrawTextEx(f,"CLASS",{classZone.x+60,classZone.y+45},28,1,WHITE); DrawTextEx(f,"HELPER",{helperZone.x+45,helperZone.y+45},28,1,WHITE); DrawTextEx(f,"BATTLE",{battleZone.x+50,battleZone.y+45},28,1,WHITE);
    DrawTextEx(f,"BAR",{barZone.x+90,barZone.y+50},30,1,WHITE); DrawTextEx(f,"HOME",{homeZone.x+78,homeZone.y+50},30,1,WHITE); DrawTextEx(f,s.isNight?"NEXT WEEK":"TO NIGHT",{nextZone.x+20,nextZone.y+38},28,1,WHITE);

    DrawRectangle(950,15,315,240,Fade(BLACK,0.55f));
    DrawTextEx(f,TextFormat("Week %d (%s)",s.week,s.isNight?"Night":"Day"),{965,25},30,1,YELLOW);
    DrawTextEx(f,TextFormat("멘탈 %d/%d  LV %d  ATK %d",d.player.hp,d.player.maxHp,d.player.level,d.player.attack),{965,61},24,1,WHITE);
    DrawTextEx(f,TextFormat("Dev:%d  Network:%d",s.devPower,s.network),{965,91},24,1,WHITE);
    DrawTextEx(f,TextFormat("과제:%d  출석:%d",s.assignmentScore,s.attendanceScore),{965,121},24,1,WHITE);
    DrawTextEx(f,TextFormat("행동력 %d/%d",s.actionPoints,s.maxActionPoints),{965,151},22,1,RAYWHITE);
    float energyRate = s.maxActionPoints > 0 ? (float)s.actionPoints / (float)s.maxActionPoints : 0.0f;
    DrawRectangle(965,181,285,18,DARKGRAY);
    DrawRectangle(965,181,(int)(285.0f*energyRate),18,GREEN);
    DrawRectangleLines(965,181,285,18,WHITE);
    DrawTextEx(f,"Move: WASD/Arrow  E: Interact",{965,210},18,1,LIGHTGRAY);
    DrawTextEx(f,"HOME: 집 내부 선택지",{965,231},18,1,LIGHTGRAY);

    if(showMessage && showCenterMessage)
    {
        DrawRectangle(330, 295, 620, 110, Fade(BLACK,0.85f));
        Vector2 messageSize = MeasureTextEx(f, message.c_str(), 36, 1);
        DrawTextEx(f,message.c_str(),{(Game::ScreenWidth-messageSize.x)*0.5f, 332},36,1,RAYWHITE);
    }
    else if(showMessage){ DrawRectangle(60, Game::ScreenHeight-130, Game::ScreenWidth-120, 95, Fade(BLACK,0.8f)); DrawTextEx(f,message.c_str(),{90,(float)Game::ScreenHeight-95},32,1,RAYWHITE); }
    DrawDialogue(game);
    if(s.gameEnded){ DrawRectangle(280,230,720,220,Fade(BLACK,0.85f)); DrawTextEx(f,TextFormat("%s - ENTER",s.endingName.c_str()),{460,315},52,2,YELLOW); }
}
