#pragma once

#include "core/Common.h"
#include "core/Scene.h"
#include "ui/Button.h"
#include "ui/Panel.h"
#include "effects/ParticleSystem.h"
#include <memory>
#include <vector>

namespace BachChien {

class GameScene : public Scene {
private:
    ParticleSystem particleSystem;

    Panel sidebarPanel;
    sf::Text turnLabelText;     // "LƯỢT ĐI"
    sf::Text turnNameText;      // Tên tông phái đang tới lượt
    sf::Text opponentText;      // Tông phái còn lại
    sf::Text stateText;         // Trạng thái thế cờ / chiếu tướng
    sf::Text aiThinkingText;    // Thông báo AI đang tính toán
    sf::Text historyTitleText;  // Tiêu đề sử ký nước cờ
    sf::Text footerHanText;     // "落子无悔" — Hán tự, dùng font thư pháp
    sf::Text footerText;        // Dòng chú thích tiếng Việt
    std::vector<sf::Text> historyTexts;

    std::unique_ptr<Button> btnSave;
    std::unique_ptr<Button> btnNewGame;
    std::unique_ptr<Button> btnMenu;

    float elapsed = 0.0f;  // Dùng cho hiệu ứng nhấp nháy của quân đang chọn
    bool lastMoveWasCheck = false;

    // Tọa độ pixel tâm của ô (row, col) trên màn hình
    sf::Vector2f getCellCenter(int row, int col) const;

    // Chuyển đổi tọa độ chuột sang (row, col) trên bàn cờ (-1 nếu ngoài biên)
    Position getCellFromPixel(float x, float y) const;

    void drawBoard(sf::RenderWindow& window);
    void drawHighlights(sf::RenderWindow& window);
    void drawPieces(sf::RenderWindow& window);
    void drawSidebar(sf::RenderWindow& window);

public:
    GameScene(Game& g);

    void onEnter() override;
    void onExit() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
};

} // namespace BachChien
