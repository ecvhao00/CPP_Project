#pragma once
#include "Scene.h"
#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>

class EngineeringScene : public IScene
{
public:
    void Enter(Game& game) override;
    void Update(Game& game, float dt) override;
    void Draw(Game& game) override;

private:
    enum class DialogueSpeaker
    {
        None,
        ProfessorK,
        Senior
    };

    bool UseActionPoint(Game& game);
    void ShowMessage(const std::string& text, bool center = false);
    void BeginDialogue(DialogueSpeaker speaker, const char* speakerName, std::vector<std::string> lines);
    void AdvanceDialogue();
    void DrawDialogue(Game& game);
    void DrawDialoguePortrait(Game& game, Rectangle area) const;

    Rectangle classZone = { 140, 150, 300, 160 };
    Rectangle seniorZone = { 520, 150, 300, 160 };
    Rectangle examZone = { 900, 150, 260, 160 };
    Rectangle exitZone = { 1010, 570, 220, 100 };
    Vector2 playerPosition = { 600.0f, 520.0f };
    bool playerMoving = false;

    bool showMessage = false;
    bool showCenterMessage = false;
    float messageTimer = 0.0f;
    std::string message = "";

    bool dialogueActive = false;
    DialogueSpeaker dialogueSpeaker = DialogueSpeaker::None;
    std::string dialogueName = "";
    std::vector<std::string> dialogueLines;
    int dialogueLineIndex = 0;
};
