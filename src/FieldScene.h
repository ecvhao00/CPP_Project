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

    Rectangle classZone = { 150, 80, 120, 80 };
    Rectangle helperZone = { 350, 80, 120, 80 };
    Rectangle battleZone = { 600, 80, 120, 80 };
    Rectangle barZone = { 200, 300, 140, 80 };
    Rectangle homeZone = { 500, 300, 140, 80 };
    Rectangle nextZone = { 800, 420, 120, 80 };

    bool showMessage = false;
    const char* message = "";
};
