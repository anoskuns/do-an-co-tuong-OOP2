#include "scenes/HelpScene.h"
#include "ui/Theme.h"
#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SceneManager.h"

namespace BachChien {

HelpScene::HelpScene(Game& g) : Scene(g) {
    const sf::Font& font = game.getAssetManager().getUiFont();

    headerText.setFont(font);
    headerText.setString(Theme::toUnicode("TU ĐẠO TÂM PHÁP (QUY TẮC CỜ & ĐIỂN CỐ)"));
    headerText.setCharacterSize(36);
    headerText.setFillColor(sf::Color(255, 220, 120));
    headerText.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = headerText.getLocalBounds();
    headerText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
    headerText.setPosition(WINDOW_WIDTH / 2.0f, 40.0f);

    float panelX = 140.0f;
    float panelY = 95.0f;
    float panelW = WINDOW_WIDTH - 280.0f;
    float panelH = 520.0f;

    contentPanel = std::make_unique<Panel>(sf::Vector2f(panelX, panelY), sf::Vector2f(panelW, panelH),
                                           "BÍ CẢNH TRUYỀN THỪA", &font);

    std::vector<std::string> rawLines = {
        "1. TÔNG PHÁI ĐỐI ĐẦU:",
        "   - Thái Hư Huyễn Phái (Đỏ): Tông chủ xuất kiếm, linh xa ngự phong, thiên hỏa bừng sáng.",
        "   - Trầm Mộc Đan Tông (Đen): Đan tổ luyện hồn, âm pháo đoạt phách, huyết sát quỷ mã.",
        "",
        "2. NGUYÊN TẮC 'LẠC TỬ VÔ HỐI':",
        "   - Đạo tâm kiên định, một khi hạ cờ thì không thể hoàn tác (Không có tính năng Undo).",
        "",
        "3. QUY TẮC HÀNH CỜ TRUYỀN THỐNG:",
        "   - Tướng / Soái: Trấn thủ trong Cửu Cung, đi ngang dọc 1 ô. Cấm hai Tướng trực tiếp đối mặt.",
        "   - Sĩ / Hộ Pháp: Bảo vệ Tướng, đi chéo 1 ô trong phạm vi Cửu Cung.",
        "   - Tượng: Đi chéo 2 ô, không được vượt sông. Bị cản nếu có quân nằm tại 'mắt voi'.",
        "   - Mã: Đi chữ nhật (tiến 2 ngang 1 hoặc tiến 1 ngang 2). Bị chặn nếu có quân 'cản chân mã'.",
        "   - Xe: Tung hoành ngang dọc không giới hạn khoảng cách, uy lực vô song.",
        "   - Pháo: Đi như Xe, nhưng khi ăn quân bắt buộc phải có đúng 1 quân làm 'ngòi' nhảy qua.",
        "   - Tốt / Binh: Chưa qua sông chỉ được tiến 1 ô; sau khi qua sông có thể đi ngang hoặc tiến.",
        "",
        "4. ĐẮC ĐẠO THẮNG BẠI:",
        "   - Khiến Tướng đối phương hết đường thoái thác (Chiếu bí) sẽ đoạt được thiên cơ chiến thắng!"
    };

    float startLineY = panelY + 45.0f;
    for (size_t i = 0; i < rawLines.size(); ++i) {
        sf::Text t;
        t.setFont(font);
        t.setString(sf::String::fromUtf8(rawLines[i].begin(), rawLines[i].end()));
        t.setCharacterSize(17);
        if (rawLines[i].rfind("1.", 0) == 0 || rawLines[i].rfind("2.", 0) == 0 ||
            rawLines[i].rfind("3.", 0) == 0 || rawLines[i].rfind("4.", 0) == 0) {
            t.setFillColor(sf::Color(255, 215, 80));
            t.setStyle(sf::Text::Bold);
        } else {
            t.setFillColor(sf::Color(210, 220, 235));
        }
        t.setPosition(panelX + 30.0f, startLineY + i * 24.0f);
        lines.push_back(t);
    }

    btnBack = std::make_unique<Button>("Đã Hiểu (Quay Lại)", font,
                                       sf::Vector2f(WINDOW_WIDTH / 2.0f - 140.0f, WINDOW_HEIGHT - 85.0f), sf::Vector2f(280.0f, 48.0f), ButtonStyle::GOLD);
    btnBack->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getSceneManager().changeScene("Menu");
    });
}

void HelpScene::onEnter() {
}

void HelpScene::handleEvent(const sf::Event& event) {
    btnBack->handleEvent(event, game.getWindow());
}

void HelpScene::update(float dt) {
    btnBack->update(dt);
}

void HelpScene::render(sf::RenderWindow& window) {
    sf::Sprite background(game.getAssetManager().getTexture("scene_bg"));
    window.draw(background);

    window.draw(headerText);
    contentPanel->draw(window);

    for (const auto& t : lines) {
        window.draw(t);
    }

    btnBack->draw(window);
}

} // namespace BachChien

