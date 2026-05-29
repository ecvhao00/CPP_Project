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

    Rectangle gameZone = { 120, 160, 260, 150 };
    Rectangle sleepZone = { 510, 160, 260, 150 };
    Rectangle assignmentZone = { 120, 430, 260, 150 };
    Rectangle schoolZone = { 510, 430, 260, 150 };
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
