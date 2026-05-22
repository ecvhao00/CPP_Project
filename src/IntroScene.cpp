#include "IntroScene.h"
#include "FieldScene.h"
#include "Game.h"
#include "raylib.h"
#include <memory>
#include <utility>

namespace
{
void DrawCenteredText(Font& font, const char* text, float y, float fontSize, Color color)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, 1);
    DrawTextEx(font, text, { (Game::ScreenWidth - size.x) * 0.5f, y }, fontSize, 1, color);
}
}

void IntroScene::Enter(Game& game)
{
    (void)game;
    phase = Phase::Help;
    dialogueIndex = 0;
    dialogueLines = {
        {"주인공", "하. . . 이번학기 공부 잘하는 사람들이 너무 많은데 A+ 맞을 수 있을까?"},
        {"주인공", "장학금. . . 꼭 받아야 하는데. . ."},
        {"교수", "자 새로운 학기가 시작 되었네 이번학기도 공부 열심히 해서 다들 좋은 성적 받길 바라네"},
        {"교수", "물론 A+을 받는 사람은 단 몇명 밖에 안되겠지만 말이야..."},
        {"주인공", ". . ."},
        {"교수", "어쨋든 공부하다가 궁금한게 있으면 나에게 오게 좋은 성적을 받는데 도움이 될걸세"},
        {"안내", "교수에게 말을 걸면 수업을 들을 수 있습니다. 수업은 체력 관리와 개발력 성장에 중요합니다."},
        {"안내", "체력은 전투에서 버티는 힘이고, 개발력은 시험과 과제 해결에 영향을 줍니다."},
        {"주인공", "하. . ."},
        {"선배", "? 왜 한숨이야? 뭐 고민있어?"},
        {"안내", "선배는 내가 친하게 지내는 유일한 선배이다. 인맥이 넓고 다양한 사람들과 친하게 지낸다."},
        {"주인공", "아무것도 아니에요 선배"},
        {"선배", "뭐 그럼 다행이고 그나저나 새학기 시작했는데 너도 사람들 좀 만나야지"},
        {"선배", "소심하다고 혼자서 학교생활하지 말고 가끔은 회식에 참여해서 기분전환 하는것도 좋아"},
        {"선배", "인맥도 성적에 도움이 된다니까?"},
        {"안내", "선배에게 말을 걸면 인맥을 올릴 수 있습니다. 인맥은 발표와 학기 평가에 영향을 줍니다."},
        {"주인공", "네 생각해볼께요 선배"},
        {"선배", "그래 그럼 나는 오늘 회식이 있어서 이만"}
    };
}

void IntroScene::Update(Game& game, float dt)
{
    (void)dt;
    if (!IsKeyPressed(KEY_ENTER) && !IsKeyPressed(KEY_E)) return;

    if (phase == Phase::Help)
    {
        phase = Phase::Dialogue;
        dialogueIndex = 0;
        return;
    }

    AdvanceDialogue(game);
}

void IntroScene::Draw(Game& game)
{
    if (phase == Phase::Help)
    {
        DrawHelp(game);
    }
    else
    {
        DrawDialogue(game);
    }
}

void IntroScene::AdvanceDialogue(Game& game)
{
    if (dialogueIndex + 1 >= (int)dialogueLines.size())
    {
        game.ChangeScene(std::make_unique<FieldScene>("", false, true));
        return;
    }

    dialogueIndex += 1;
}

void IntroScene::DrawHelp(Game& game)
{
    auto& f = game.Resources().UiFont();

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, BLACK);
    DrawCenteredText(f, "학기 시작 전 안내", 90, 42, YELLOW);
    DrawRectangle(330, 158, 620, 2, Fade(RAYWHITE, 0.35f));

    DrawTextEx(f, "목표: 15주 동안 개발력과 인맥을 키워 A+ 엔딩을 노리세요.", {260, 205}, 27, 1, RAYWHITE);
    DrawTextEx(f, "공학관에서는 수업으로 개발력을 올리고, 선배에게 인맥을 얻을 수 있습니다.", {260, 255}, 27, 1, RAYWHITE);
    DrawTextEx(f, "밤에는 술자리와 집을 이용할 수 있고, 집에서 과제를 수행할 수 있습니다.", {260, 305}, 27, 1, RAYWHITE);
    DrawTextEx(f, "8주와 15주는 시험 주입니다. 해당 주에는 과제를 반드시 수행해야 합니다.", {260, 355}, 27, 1, RAYWHITE);
    DrawTextEx(f, "개발력과 인맥이 부족하면 시험/발표 디버프가 생기고 전투가 어려워집니다.", {260, 405}, 27, 1, RAYWHITE);
    DrawTextEx(f, "집에서는 게임하기로 멘탈을 회복하거나, 잠을 자서 다음 주로 넘어갈 수 있습니다.", {260, 455}, 27, 1, RAYWHITE);

    DrawCenteredText(f, "ENTER : 안내 닫기", 585, 32, LIGHTGRAY);
}

void IntroScene::DrawDialogue(Game& game)
{
    auto& f = game.Resources().UiFont();
    const DialogueLine& line = dialogueLines[dialogueIndex];

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, BLACK);

    Rectangle panel = { 70, 485, 1140, 190 };
    DrawRectangleRec(panel, Fade(BLACK, 0.92f));
    DrawRectangleLinesEx(panel, 3, RAYWHITE);

    DrawTextEx(f, line.speaker.c_str(), { 105, 510 }, 28, 1, YELLOW);
    DrawTextEx(f, line.text.c_str(), { 105, 555 }, 28, 1, RAYWHITE);
    DrawTextEx(f, TextFormat("%d/%d", dialogueIndex + 1, (int)dialogueLines.size()), { 1050, 630 }, 22, 1, LIGHTGRAY);
    DrawTextEx(f, "E/ENTER: 다음", { 1110, 630 }, 22, 1, LIGHTGRAY);
}
