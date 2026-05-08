#pragma once
#include "Scene.h"
#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>

class FieldScene : public IScene
{
public:
    explicit FieldScene(std::string enterMessage = "", bool centerMessage = false);
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

    void EndDay(Game& game);
    void NextWeek(Game& game);
    void ApplyWeekEvent(Game& game);
    bool UseActionPoint(Game& game);
    void ShowFieldMessage(const char* text, bool center = false);
    void BeginDialogue(DialogueSpeaker speaker, const char* speakerName, std::vector<std::string> lines);
    void AdvanceDialogue();
    void DrawDialogue(Game& game);
    void DrawDialoguePortrait(Font& font, Rectangle area) const;

    Rectangle classZone = { 120, 90, 220, 120 };
    Rectangle helperZone = { 410, 90, 220, 120 };
    Rectangle battleZone = { 700, 90, 220, 120 };
    Rectangle barZone = { 200, 360, 240, 130 };
    Rectangle homeZone = { 560, 360, 240, 130 };
    Rectangle nextZone = { 1030, 570, 210, 100 };

    bool showMessage = false;
    bool showCenterMessage = false;
    std::string message = "";

    bool dialogueActive = false;
    DialogueSpeaker dialogueSpeaker = DialogueSpeaker::None;
    std::string dialogueName = "";
    std::vector<std::string> dialogueLines;
    int dialogueLineIndex = 0;

    std::string enterMessage = "";
    bool enterMessageCentered = false;
};
