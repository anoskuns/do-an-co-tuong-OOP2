#pragma once

#include "core/Scene.h"
#include "ui/Button.h"
#include "effects/ParticleSystem.h"
#include <memory>

namespace BachChien {

class ResultScene : public Scene {
private:
    sf::Text resultTitle;
    sf::Text resultDesc;
    sf::Text movesCountText;
    ParticleSystem particleSystem;

    std::unique_ptr<Button> btnRematch;
    std::unique_ptr<Button> btnMenu;

public:
    ResultScene(Game& g);

    void onEnter() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};

} // namespace BachChien

