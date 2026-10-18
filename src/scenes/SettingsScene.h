#pragma once

#include "core/Scene.h"
#include "ui/Button.h"
#include "ui/Panel.h"
#include <memory>

namespace BachChien {

class SettingsScene : public Scene {
private:
    sf::Text headerText;

    sf::Text sfxLabel;
    sf::Text sfxValText;
    std::unique_ptr<Button> btnSfxDown;
    std::unique_ptr<Button> btnSfxUp;

    sf::Text bgmLabel;
    sf::Text bgmValText;
    std::unique_ptr<Button> btnBgmDown;
    std::unique_ptr<Button> btnBgmUp;

    sf::Text fsLabel;
    std::unique_ptr<Button> btnFsToggle;

    std::unique_ptr<Button> btnBack;

    void updateTexts();

public:
    SettingsScene(Game& g);

    void onEnter() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};

} // namespace BachChien

