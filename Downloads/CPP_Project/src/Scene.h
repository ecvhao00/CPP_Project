#pragma once

class Game;

class IScene
{
public:
    virtual ~IScene() = default;

    virtual void Enter(Game& game) {}
    virtual void Update(Game& game, float dt) = 0;
    virtual void Draw(Game& game) = 0;
};