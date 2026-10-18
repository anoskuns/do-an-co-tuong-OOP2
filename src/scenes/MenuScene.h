#pragma once

#include "core/Scene.h"
#include "ui/Button.h"
#include "effects/ParticleSystem.h"
#include <vector>
#include <memory>

namespace BachChien {

class MenuScene : public Scene {
private:
    sf::Text titleText;      // Tên trò chơi
    sf::Text hanTitleText;   // 百战仙局
    sf::Text subtitleText;   // Hai tông phái đối đầu
    sf::Text hintText;       // Dòng gợi ý dưới cùng
    sf::Sprite emblemSprite; // Con dấu mờ phía sau tiêu đề

    std::vector<std::unique_ptr<Button>> buttons;
    ParticleSystem particleSystem;

    float elapsed = 0.0f;

    void drawTitle(sf::RenderWindow& window);
    void drawDivider(sf::RenderWindow& window, float y);

public:
    MenuScene(Game& g);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};

} // namespace BachChien
