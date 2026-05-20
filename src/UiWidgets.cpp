#include "UiWidgets.h"

namespace
{
float ClampRatio(int value, int maxValue)
{
    if (maxValue <= 0) return 0.0f;
    float ratio = (float)value / (float)maxValue;
    if (ratio < 0.0f) return 0.0f;
    if (ratio > 1.0f) return 1.0f;
    return ratio;
}

void DrawBar(Font& font, const char* label, int value, int maxValue, Rectangle bar, Color fill)
{
    DrawTextEx(font, TextFormat("%s %d/%d", label, value, maxValue), { bar.x - 115, bar.y - 6 }, 18, 1, RAYWHITE);
    DrawRectangleRec(bar, DARKGRAY);
    DrawRectangle((int)bar.x, (int)bar.y, (int)(bar.width * ClampRatio(value, maxValue)), (int)bar.height, fill);
    DrawRectangleLinesEx(bar, 1, Fade(RAYWHITE, 0.85f));
}
}

namespace UiWidgets
{
void DrawTopStatus(Font& font, const GameData& data, const char* location, const char* hint1, const char* hint2)
{
    const auto& player = data.player;
    const auto& semester = data.semester;

    DrawRectangle(950, 15, 315, 275, Fade(BLACK, 0.55f));
    DrawTextEx(font, TextFormat("%d주차 (%s)", semester.week, semester.isNight ? "밤" : "낮"), { 965, 25 }, 30, 1, YELLOW);
    DrawTextEx(font, TextFormat("위치: %s", location), { 965, 61 }, 22, 1, LIGHTGRAY);

    DrawTextEx(font, TextFormat("멘탈 %d/%d", player.hp, player.maxHp), { 965, 91 }, 22, 1, WHITE);
    Rectangle mentalBar = { 965, 119, 285, 16 };
    DrawRectangleRec(mentalBar, DARKGRAY);
    DrawRectangle((int)mentalBar.x, (int)mentalBar.y, (int)(mentalBar.width * ClampRatio(player.hp, player.maxHp)), (int)mentalBar.height, GREEN);
    DrawRectangleLinesEx(mentalBar, 1, RAYWHITE);

    DrawTextEx(font, TextFormat("개발력:%d  인맥:%d", semester.devPower, semester.network), { 965, 150 }, 22, 1, WHITE);
    DrawTextEx(font, TextFormat("레벨 %d  공격력 %d", player.level, player.attack), { 965, 180 }, 22, 1, WHITE);
    DrawTextEx(font, hint1, { 965, 235 }, 18, 1, LIGHTGRAY);
    DrawTextEx(font, hint2, { 965, 256 }, 18, 1, LIGHTGRAY);
}

void DrawBottomGraphs(Font& font, const GameData& data)
{
    const auto& semester = data.semester;

    DrawRectangle(30, 635, 555, 75, Fade(BLACK, 0.62f));
    DrawRectangleLines(30, 635, 555, 75, Fade(RAYWHITE, 0.55f));
    DrawBar(font, "과제", semester.assignmentScore, 10, { 160, 648, 385, 12 }, SKYBLUE);
    DrawBar(font, "출석", semester.attendanceScore, 10, { 160, 672, 385, 12 }, LIME);
    DrawBar(font, "행동력", semester.actionPoints, semester.maxActionPoints, { 160, 696, 385, 12 }, ORANGE);
}
}
