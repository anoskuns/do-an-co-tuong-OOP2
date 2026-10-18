#pragma once

#include "core/Common.h"
#include "core/Scene.h"
#include "ui/Button.h"
#include "ui/Panel.h"
#include <vector>
#include <memory>

namespace BachChien {

class FactionSelectScene : public Scene {
private:
    sf::Text headerText;
    sf::Text promptFactionText;
    sf::Text promptDiffText;

    Side selectedSide = Side::RED;
    Difficulty selectedDiff = Difficulty::MEDIUM;

    std::unique_ptr<Button> btnSideRed;
    std::unique_ptr<Button> btnSideBlack;

    std::unique_ptr<Button> btnDiffEasy;
    std::unique_ptr<Button> btnDiffMedium;
    std::unique_ptr<Button> btnDiffHard;

    std::unique_ptr<Button> btnStart;
    std::unique_ptr<Button> btnBack;

    void updateSelectionVisuals();

public:
    FactionSelectScene(Game& g);

    void onEnter() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};

} // namespace BachChien
