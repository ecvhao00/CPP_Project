#pragma once
#include "Scene.h"
#include "raylib.h"
#include <vector>

class FieldScene : public IScene
{
public:
    FieldScene();

    void Enter(Game& game) override;
    void Update(Game& game, float dt) override;
    void Draw(Game& game) override;

private:
    bool CheckWallCollision(const Rectangle& rect) const;
    void ResolveMovement(Rectangle& playerRect, const Vector2& move);

private:
    std::vector<Rectangle> walls;

    Rectangle npc = { 260, 120, 40, 40 };
    Rectangle enemy = { 720, 360, 48, 48 };
    Rectangle exitZone = { 860, 40, 50, 50 };

    bool showDialog = false;
    bool touchingNpc = false;
};