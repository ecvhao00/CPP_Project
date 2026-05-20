#pragma once
#include "Scene.h"
#include <string>
#include <vector>

class IntroScene : public IScene
{
public:
    void Enter(Game& game) override;
    void Update(Game& game, float dt) override;
    void Draw(Game& game) override;

private:
    enum class Phase
    {
        Help,
        Dialogue
    };

    struct DialogueLine
    {
        std::string speaker;
        std::string text;
    };

    void AdvanceDialogue(Game& game);
    void DrawHelp(Game& game);
    void DrawDialogue(Game& game);

    Phase phase = Phase::Help;
    int dialogueIndex = 0;
    std::vector<DialogueLine> dialogueLines;
};
