#pragma once
#include "GameData.h"
#include "ResourceManager.h"
#include "raylib.h"

namespace UiWidgets
{
void DrawTopStatus(Font& font, const GameData& data, const char* location, const char* hint1, const char* hint2);
void DrawBottomGraphs(Font& font, const GameData& data);
void DrawPlayer(ResourceManager& resources, Vector2 position, bool moving);
void DrawPortrait(Texture2D& texture, Rectangle area);
}
