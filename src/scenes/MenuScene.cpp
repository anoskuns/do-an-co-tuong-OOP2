#include "scenes/MenuScene.h"
#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SceneManager.h"
#include "ui/Theme.h"

#include <cmath>

namespace BachChien {

namespace {

constexpr float TITLE_Y = 78.0f;
constexpr float BUTTON_WIDTH = 400.0f;
constexpr float BUTTON_HEIGHT = 46.0f;
constexpr float BUTTON_TOP = 316.0f;
constexpr float BUTTON_GAP = 56.0f;

} // namespace

MenuScene::MenuScene(Game& g) : Scene(g) {
    // Chữ Latin/tiếng Việt dùng font giao diện; Hán tự dùng font thư pháp.
    const sf::Font& font = game.getAssetManager().getUiFont();

    // --- Con dấu mờ làm hình nền cho tiêu đề ---
    emblemSprite.setTexture(game.getAssetManager().getTexture("emblem"));
    const sf::Vector2u emblemSize = emblemSprite.getTexture()->getSize();
    emblemSprite.setOrigin(emblemSize.x * 0.5f, emblemSize.y * 0.5f);
    const float emblemScale = 380.0f / static_cast<float>(emblemSize.x == 0 ? 1 : emblemSize.x);
    emblemSprite.setScale(emblemScale, emblemScale);
    emblemSprite.setPosition(WINDOW_WIDTH * 0.5f, 176.0f);
    emblemSprite.setColor(sf::Color(255, 255, 255, 34));

    // --- Tiêu đề chính ---
    titleText.setFont(font);
    titleText.setString(Theme::toUnicode("BÁCH CHIẾN TIÊN CỤC"));
    titleText.setCharacterSize(70);
    titleText.setStyle(sf::Text::Bold);
    titleText.setLetterSpacing(2.5f);
    titleText.setFillColor(Theme::Palette::GoldPale);
    {
        const sf::FloatRect bounds = titleText.getLocalBounds();
        titleText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        titleText.setPosition(WINDOW_WIDTH * 0.5f, TITLE_Y);
    }

    // --- Hán tự 百战仙局 (dùng font thư pháp) ---
    hanTitleText.setFont(game.getAssetManager().getFont());
    hanTitleText.setString(Theme::toUnicode(""));
    hanTitleText.setCharacterSize(34);
    hanTitleText.setFillColor(Theme::withAlpha(Theme::Palette::Gold, 225));
    {
        const sf::FloatRect bounds = hanTitleText.getLocalBounds();
        hanTitleText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        hanTitleText.setPosition(WINDOW_WIDTH * 0.5f, 142.0f);
    }

    // --- Phụ đề hai tông phái ---
    subtitleText.setFont(font);
    subtitleText.setString(Theme::toUnicode("Thái Hư Huyễn Phái   •   Trầm Mộc Đan Tông"));
    subtitleText.setCharacterSize(22);
    subtitleText.setFillColor(sf::Color(188, 212, 236));
    {
        const sf::FloatRect bounds = subtitleText.getLocalBounds();
        subtitleText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        subtitleText.setPosition(WINDOW_WIDTH * 0.5f, 208.0f);
    }

    // --- Dòng gợi ý chân trang ---
    hintText.setFont(font);
    hintText.setString(Theme::toUnicode("Đồ án Lập trình hướng đối tượng — C++ "));
    hintText.setCharacterSize(14);
    hintText.setFillColor(sf::Color(130, 138, 158));
    {
        const sf::FloatRect bounds = hintText.getLocalBounds();
        hintText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        hintText.setPosition(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - 26.0f);
    }

    // --- Danh sách nút chức năng ---
    const float buttonX = (WINDOW_WIDTH - BUTTON_WIDTH) * 0.5f;

    struct MenuEntry {
        const char* label;
        ButtonStyle style;
        const char* targetScene;
        bool isQuit;
    };
    const MenuEntry entries[] = {
        {"Đại Chiến Tiên Ma  (1 Người vs AI)", ButtonStyle::GOLD,   "FactionSelect", false},
        {"Tông Môn Luận Đạo  (2 Người)",       ButtonStyle::CYAN,   "FactionSelect", false},
        {"Tải Ván Cờ",        ButtonStyle::GOLD,   "SaveLoad",      false},
        {"Luật Cờ Tướng",   ButtonStyle::CYAN,   "Help",          false},
        {"Cài Đặt",      ButtonStyle::DARK,   "Settings",      false},
        {"Thoát Game",       ButtonStyle::DANGER, "",              true},
    };

    int index = 0;
    for (const MenuEntry& entry : entries) {
        auto button = std::make_unique<Button>(
            entry.label, font,
            sf::Vector2f(buttonX, BUTTON_TOP + BUTTON_GAP * static_cast<float>(index)),
            sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT), entry.style);

        if (entry.isQuit) {
            button->setOnClick([this]() {
                game.getAudioManager().playSound("click");
                game.quit();
            });
        } else if (index < 2) {
            // Hai chế độ chơi: chọn cấu hình tương ứng rồi vào màn chọn phe.
            const GameMode mode = (index == 0) ? GameMode::PVE : GameMode::PVP;
            button->setOnClick([this, mode]() {
                game.getAudioManager().playSound("click");
                game.setPendingConfig(mode, Side::RED, Difficulty::MEDIUM);
                game.getSceneManager().changeScene("FactionSelect");
            });
        } else {
            const std::string target = entry.targetScene;
            button->setOnClick([this, target]() {
                game.getAudioManager().playSound("click");
                game.getSceneManager().changeScene(target);
            });
        }

        buttons.push_back(std::move(button));
        ++index;
    }
}

void MenuScene::onEnter() {
    game.getAudioManager().playBGM("menu_bgm");
}

void MenuScene::onExit() {
}

void MenuScene::handleEvent(const sf::Event& event) {
    for (auto& button : buttons) {
        button->handleEvent(event, game.getWindow());
    }
}

void MenuScene::update(float dt) {
    elapsed += dt;
    particleSystem.emitMist(sf::FloatRect(0.0f, static_cast<float>(WINDOW_HEIGHT) - 150.0f,
                                          static_cast<float>(WINDOW_WIDTH), 150.0f), 1);
    particleSystem.update(dt);

    for (auto& button : buttons) {
        button->update(dt);
    }

    // Con dấu xoay rất chậm, tạo cảm giác "linh khí vận chuyển".
    emblemSprite.setRotation(elapsed * 4.0f);
}

void MenuScene::drawDivider(sf::RenderWindow& window, float y) {
    const float centerX = WINDOW_WIDTH * 0.5f;
    const float halfWidth = 190.0f;
    const sf::Color clear = Theme::withAlpha(Theme::Palette::Gold, 0);
    const sf::Color bright = Theme::withAlpha(Theme::Palette::Gold, 165);

    for (int side = 0; side < 2; ++side) {
        const float dir = (side == 0) ? -1.0f : 1.0f;
        sf::VertexArray line(sf::Quads, 4);
        line[0] = sf::Vertex(sf::Vector2f(centerX, y), clear);
        line[1] = sf::Vertex(sf::Vector2f(centerX + dir * halfWidth, y), bright);
        line[2] = sf::Vertex(sf::Vector2f(centerX + dir * halfWidth, y + 1.2f), bright);
        line[3] = sf::Vertex(sf::Vector2f(centerX, y + 1.2f), clear);
        window.draw(line);
    }

    sf::CircleShape gem(5.0f, 4);
    gem.setOrigin(5.0f, 5.0f);
    gem.setPosition(centerX, y + 0.6f);
    gem.setFillColor(Theme::withAlpha(Theme::Palette::GoldBright, 225));
    window.draw(gem);
}

void MenuScene::drawTitle(sf::RenderWindow& window) {
    // Quầng sáng vàng phía sau tiêu đề, nhịp thở nhẹ theo thời gian.
    const float breath = 0.85f + 0.15f * std::sin(elapsed * 1.4f);
    Theme::drawGlow(window, sf::Vector2f(WINDOW_WIDTH * 0.5f, TITLE_Y),
                    360.0f, sf::Color(255, 205, 110, static_cast<sf::Uint8>(70.0f * breath)), 1.0f);

    // Bóng đổ phía dưới tiêu đề để chữ tách khỏi nền.
    sf::Text shadow = titleText;
    shadow.setFillColor(sf::Color(0, 0, 0, 190));
    shadow.move(0.0f, 4.0f);
    window.draw(shadow);

    // Lớp chữ lệch màu vàng đậm tạo hiệu ứng viền kim loại.
    sf::Text outline = titleText;
    outline.setFillColor(Theme::withAlpha(Theme::Palette::Gold, 200));
    outline.move(0.0f, 2.0f);
    window.draw(outline);

    window.draw(titleText);
}

void MenuScene::render(sf::RenderWindow& window) {
    // Nền huyền ảo dùng chung.
    sf::Sprite background(game.getAssetManager().getTexture("scene_bg"));
    window.draw(background);

    // Con dấu mờ làm hình nền tiêu đề.
    window.draw(emblemSprite);

    // Sương mù linh khí.
    particleSystem.draw(window, game.getAssetManager().getTexture("particle_spark"));

    drawTitle(window);
    window.draw(hanTitleText);
    window.draw(subtitleText);
    drawDivider(window, 248.0f);

    for (auto& button : buttons) {
        button->draw(window);
    }

    window.draw(hintText);
}

} // namespace BachChien
