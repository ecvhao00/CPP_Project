#pragma once
#include "Scene.h"
#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>

class HomeScene : public IScene
{
public:
    void Enter(Game& game) override;
    void Update(Game& game, float dt) override;
    void Draw(Game& game) override;

private:
    bool UseHomeAction(Game& game);
    int RecoverMental(Game& game, int amount);
    std::string StartNextWeek(Game& game);
    std::string ApplyWeekEvent(Game& game);
    void StartSleepTransition();
    void UpdateSleepTransition(Game& game, float dt);
    void DrawFadeOverlay() const;
    void ShowMessage(const std::string& text, bool center = false);
    void BeginDialogue(std::vector<std::string> lines);
    void AdvanceDialogue();
    void DrawDialogue(Game& game);

    Rectangle gameZone = { 190, 55, 375, 320 };
    Rectangle sleepZone = { 790, 120, 300, 390 };
    Rectangle assignmentZone = { 320, 410, 385, 210 };
    Rectangle schoolZone = { 875, 535, 260, 105 };
    Vector2 playerPosition = { 600.0f, 520.0f };
    bool playerMoving = false;

    bool showMessage = false;
    bool showCenterMessage = false;
    std::string message = "";

    bool sleepTransitionActive = false;
    float sleepTransitionTimer = 0.0f;

    bool dialogueActive = false;
    std::vector<std::string> dialogueLines;
    int dialogueLineIndex = 0;
};
