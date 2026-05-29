#pragma once
#include "Scene.h"
#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>

class FieldScene : public IScene
{
public:
    explicit FieldScene(std::string enterMessage = "", bool centerMessage = false, bool startFadeIn = false);
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

    enum class TimeTransition
    {
        None,
        FadeInOnly,
        ToNight,
        ToNextWeek
    };

    void EndDay(Game& game);
    void NextWeek(Game& game);
    void ApplyWeekEvent(Game& game);
    bool UseActionPoint(Game& game);
    void ShowFieldMessage(const char* text, bool center = false);
    void StartTimeTransition(TimeTransition transition);
    void UpdateTimeTransition(Game& game, float dt);
    void DrawFadeOverlay() const;
    void BeginDialogue(DialogueSpeaker speaker, const char* speakerName, std::vector<std::string> lines);
    void AdvanceDialogue();
    void DrawDialogue(Game& game);
    void DrawDialoguePortrait(Game& game, Rectangle area) const;

    Rectangle engineeringZone = { 70, 75, 360, 155 };
    Rectangle barZone = { 70, 300, 240, 130 };
    Rectangle homeZone = { 70, 500, 240, 130 };
    Rectangle nextZone = { 1030, 570, 210, 100 };

    bool showMessage = false;
    bool showCenterMessage = false;
    float messageTimer = 0.0f;
    std::string message = "";
    bool playerMoving = false;

    bool dialogueActive = false;
    DialogueSpeaker dialogueSpeaker = DialogueSpeaker::None;
    std::string dialogueName = "";
    std::vector<std::string> dialogueLines;
    int dialogueLineIndex = 0;

    std::string enterMessage = "";
    bool enterMessageCentered = false;
    bool startFadeIn = false;

    TimeTransition timeTransition = TimeTransition::None;
    float transitionTimer = 0.0f;
    bool transitionCommitted = false;
};
