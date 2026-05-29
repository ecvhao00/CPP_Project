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
    DrawTextEx(font, TextFormat("%s %d/%d", label, value, maxValue), { bar.x - 145, bar.y - 6 }, 18, 1, RAYWHITE);
    DrawRectangleRec(bar, DARKGRAY);
    DrawRectangle((int)bar.x, (int)bar.y, (int)(bar.width * ClampRatio(value, maxValue)), (int)bar.height, fill);
    DrawRectangleLinesEx(bar, 1, Fade(RAYWHITE, 0.85f));
}
}

namespace UiWidgets
{
void DrawPlayer(ResourceManager& resources, Vector2 position, bool moving)
{
    if (!resources.HasPlayerWalkSprites())
    {
        DrawRectangle((int)position.x, (int)position.y, 36, 36, SKYBLUE);
        return;
    }

    int frameIndex = moving ? ((int)(GetTime() * 8.0) % resources.PlayerWalkFrameCount()) : 0;
    Texture2D& texture = resources.PlayerWalkFrame(frameIndex);
    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dest = { position.x, position.y, 36, 36 };
    DrawTexturePro(texture, source, dest, { 0, 0 }, 0.0f, WHITE);
}

void DrawPortrait(Texture2D& texture, Rectangle area)
{
    DrawRectangleRec(area, Fade(BLACK, 0.35f));

    float scale = area.width / (float)texture.width;
    float heightScale = area.height / (float)texture.height;
    if (heightScale < scale) scale = heightScale;

    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dest = {
        area.x + (area.width - (float)texture.width * scale) * 0.5f,
        area.y + (area.height - (float)texture.height * scale) * 0.5f,
        (float)texture.width * scale,
        (float)texture.height * scale
    };
    DrawTexturePro(texture, source, dest, { 0, 0 }, 0.0f, WHITE);
    DrawRectangleLinesEx(area, 3, RAYWHITE);
}

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
    DrawBar(font, "과제 미제출", semester.assignmentMisses, 5, { 190, 648, 355, 12 }, RED);
    DrawBar(font, "결석", semester.absences, 4, { 190, 672, 355, 12 }, ORANGE);
    DrawBar(font, "행동력", semester.actionPoints, semester.maxActionPoints, { 190, 696, 355, 12 }, SKYBLUE);
}
}
