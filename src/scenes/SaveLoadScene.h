#pragma once

#include "core/Scene.h"
#include "ui/Button.h"
#include "ui/Panel.h"
#include <vector>
#include <memory>

namespace BachChien {

class SaveLoadScene : public Scene {
private:
    sf::Text headerText;
    sf::Text statusText;

    struct SlotUI {
        std::unique_ptr<Panel> panel;
        sf::Text infoText;
        std::unique_ptr<Button> btnSave;
        std::unique_ptr<Button> btnLoad;
    };

    std::vector<SlotUI> slots;
    std::unique_ptr<Button> btnBack;

    void refreshSlotInfos();

public:
    SaveLoadScene(Game& g);

    void onEnter() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};

} // namespace BachChien

