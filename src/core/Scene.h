#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace BachChien {

class Game;

class Scene {
protected:
    Game& game;

public:
    Scene(Game& g) : game(g) {}
    virtual ~Scene() = default;

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};

} // namespace BachChien

