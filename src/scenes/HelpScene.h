#pragma once

#include "core/Scene.h"
#include "ui/Button.h"
#include "ui/Panel.h"
#include <vector>
#include <memory>

namespace BachChien {

class HelpScene : public Scene {
private:
    sf::Text headerText;
    std::unique_ptr<Panel> contentPanel;
    std::vector<sf::Text> lines;
    std::unique_ptr<Button> btnBack;

public:
    HelpScene(Game& g);

    void onEnter() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};

} // namespace BachChien

