#pragma once
#include "Scene.h"
#include "raylib.h"

class FieldScene : public IScene
{
public:
    FieldScene();
    void Enter(Game& game) override;
    void Update(Game& game, float dt) override;
    void Draw(Game& game) override;

private:
    void EndDay(Game& game);
    void NextWeek(Game& game);
    void ApplyWeekEvent(Game& game);

    Rectangle classZone = { 120, 90, 220, 120 };
    Rectangle helperZone = { 410, 90, 220, 120 };
    Rectangle battleZone = { 700, 90, 220, 120 };
    Rectangle barZone = { 200, 360, 240, 130 };
    Rectangle homeZone = { 560, 360, 240, 130 };
    Rectangle nextZone = { 1030, 570, 210, 100 };

    bool showMessage = false;
    const char* message = "";
};
