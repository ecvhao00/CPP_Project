#include "FieldScene.h"
#include "BattleScene.h"
#include "Game.h"
#include "TitleScene.h"
#include <memory>
#include <cmath>

FieldScene::FieldScene() {}
void FieldScene::Enter(Game& game) { (void)game; showMessage = false; }

void FieldScene::EndDay(Game& game){ auto& s=game.Data().semester; if(!s.foughtToday) s.assignmentScore-=1; if(!s.attendedClassToday) s.attendanceScore-=1; s.isNight=true; s.homeActionsUsedTonight=0; s.drinksTonight=0; }
void FieldScene::NextWeek(Game& game){ auto& s=game.Data().semester; s.week+=1; s.isNight=false; s.foughtToday=false; s.attendedClassToday=false; ApplyWeekEvent(game);} 

void FieldScene::ApplyWeekEvent(Game& game)
{
    auto& d = game.Data(); auto& s = d.semester;
    if (s.week == 8){ s.midtermExamDebuff = s.devPower < 12; message = s.midtermExamDebuff ? "중간고사 디버프 발생" : "중간고사 안정 통과"; showMessage = true; }
    else if (s.week == 9){ s.midtermPresentationDebuff = s.network < 10; message = s.midtermPresentationDebuff ? "중간발표 디버프 발생" : "중간발표 안정 통과"; showMessage = true; }
    else if (s.week == 14){ s.finalExamDebuff = s.devPower < 20; message = s.finalExamDebuff ? "기말고사 디버프 발생" : "기말고사 안정 통과"; showMessage = true; }
    else if (s.week == 15){ s.finalPresentationDebuff = s.network < 18; s.gameEnded = true; s.passed = !(s.finalPresentationDebuff||s.finalExamDebuff||s.midtermExamDebuff||s.midtermPresentationDebuff)&&s.assignmentScore>0&&s.attendanceScore>0; message = s.passed ? "학기 종료! PASS" : "학기 종료... FAIL"; showMessage = true; }
    int debuffs=(s.midtermExamDebuff?1:0)+(s.midtermPresentationDebuff?1:0)+(s.finalExamDebuff?1:0)+(s.finalPresentationDebuff?1:0);
    d.player.attack = 8 + d.player.level - debuffs; if (d.player.attack < 3) d.player.attack = 3;
}

void FieldScene::Update(Game& game, float dt)
{
    auto& d = game.Data(); auto& s = d.semester;
    if (s.gameEnded){ if(IsKeyPressed(KEY_ENTER)){ game.Data()=GameData{}; game.ChangeScene(std::make_unique<TitleScene>());} return; }

    Vector2 in{0,0};
    if (IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D)) in.x +=1; if (IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A)) in.x -=1;
    if (IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S)) in.y +=1; if (IsKeyDown(KEY_UP)||IsKeyDown(KEY_W)) in.y -=1;
    if (in.x!=0||in.y!=0){ float l=sqrtf(in.x*in.x+in.y*in.y); in.x/=l; in.y/=l; }
    d.player.position.x += in.x*d.player.speed*dt; d.player.position.y += in.y*d.player.speed*dt;
    d.player.position.x = Clamp(d.player.position.x, 20, (float)Game::ScreenWidth-56);
    d.player.position.y = Clamp(d.player.position.y, 20, (float)Game::ScreenHeight-56);

    Rectangle p{d.player.position.x,d.player.position.y,36,36};
    bool nearClass=CheckCollisionRecs(p, classZone), nearHelper=CheckCollisionRecs(p, helperZone), nearBattle=CheckCollisionRecs(p, battleZone), nearBar=CheckCollisionRecs(p, barZone), nearHome=CheckCollisionRecs(p, homeZone), nearNext=CheckCollisionRecs(p, nextZone);

    if (IsKeyPressed(KEY_E))
    {
        if (nearClass && !s.isNight){ s.attendedClassToday=true; s.devPower +=2; message="김교수 수업 수강! 개발력 +2"; showMessage=true; }
        else if (nearHelper){ s.network +=1; message="조력자 선배와 대화! 인맥도 +1"; showMessage=true; }
        else if (nearBattle){ game.ChangeScene(std::make_unique<BattleScene>()); return; }
        else if (nearBar && s.isNight && s.drinksTonight<2){ s.drinksTonight++; s.network +=2; message=TextFormat("%d차 술자리! 인맥도 +2", s.drinksTonight); showMessage=true; }
        else if (nearHome && s.isNight){ int maxTurns=2-s.drinksTonight; if(s.homeActionsUsedTonight<maxTurns){ s.homeActionsUsedTonight++; if(IsKeyDown(KEY_LEFT_SHIFT)){ s.devPower +=1; message="집 복습! 개발력 +1"; showMessage=true;} else { game.ChangeScene(std::make_unique<BattleScene>()); return; } } }
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

    DrawRectangle(15,15,620,190,Fade(BLACK,0.55f));
    DrawTextEx(f,TextFormat("Week %d (%s)",s.week,s.isNight?"Night":"Day"),{30,25},32,1,YELLOW);
    DrawTextEx(f,TextFormat("HP %d/%d  LV %d  ATK %d",d.player.hp,d.player.maxHp,d.player.level,d.player.attack),{30,65},28,1,WHITE);
    DrawTextEx(f,TextFormat("Dev:%d  Network:%d",s.devPower,s.network),{30,98},28,1,WHITE);
    DrawTextEx(f,TextFormat("Assignment:%d  Attendance:%d",s.assignmentScore,s.attendanceScore),{30,131},28,1,WHITE);
    DrawTextEx(f,"Move: WASD/Arrow, Interact: E, Home review: hold LeftShift + E",{30,166},20,1,LIGHTGRAY);

    if(showMessage){ DrawRectangle(60, Game::ScreenHeight-130, Game::ScreenWidth-120, 95, Fade(BLACK,0.8f)); DrawTextEx(f,message,{90,(float)Game::ScreenHeight-95},32,1,RAYWHITE); }
    if(s.gameEnded){ DrawRectangle(280,230,720,220,Fade(BLACK,0.85f)); DrawTextEx(f,s.passed?"PASS - ENTER":"FAIL - ENTER",{460,315},52,2,YELLOW); }
}
