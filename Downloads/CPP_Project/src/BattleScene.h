#pragma once
#include "Scene.h"
#include <string>

enum class BattleState
{
    PlayerTurn,
    EnemyTurn,
    Victory,
    Defeat
};

class BattleScene : public IScene
{
public:
    void Enter(Game& game) override;
    void Update(Game& game, float dt) override;
    void Draw(Game& game) override;

private:
    int enemyHp = 20;
    int enemyAttack = 6;
    float enemyTurnTimer = 0.0f;
    BattleState state = BattleState::PlayerTurn;
    std::string message = "An assignment monster appeared!";
};
