#include "scenes/SettingsScene.h"
#include "ui/Theme.h"
#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SettingsManager.h"
#include "core/SceneManager.h"

namespace BachChien {

SettingsScene::SettingsScene(Game& g) : Scene(g) {
    const sf::Font& font = game.getAssetManager().getUiFont();

    headerText.setFont(font);
    headerText.setString(Theme::toUnicode("HUYỀN MÔN CÀN KHÔN (CÀI ĐẶT)"));
    headerText.setCharacterSize(38);
    headerText.setFillColor(sf::Color(255, 220, 120));
    headerText.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = headerText.getLocalBounds();
    headerText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
    headerText.setPosition(WINDOW_WIDTH / 2.0f, 60.0f);

    float startY = 170.0f;
    float gapY = 85.0f;

    // 1. Hiệu ứng âm thanh (SFX)
    sfxLabel.setFont(font);
    sfxLabel.setString(Theme::toUnicode("Âm Lượng Hiệu Ứng (SFX):"));
    sfxLabel.setCharacterSize(24);
    sfxLabel.setFillColor(sf::Color(220, 220, 240));
    sfxLabel.setPosition(320.0f, startY);

    sfxValText.setFont(font);
    sfxValText.setCharacterSize(24);
    sfxValText.setFillColor(sf::Color(255, 215, 0));
    sfxValText.setPosition(740.0f, startY);

    btnSfxDown = std::make_unique<Button>("-", font, sf::Vector2f(670.0f, startY - 5.0f), sf::Vector2f(45.0f, 40.0f), ButtonStyle::DARK);
    btnSfxDown->setOnClick([this]() {
        float val = game.getSettingsManager().getSFXVolume() - 10.0f;
        game.getSettingsManager().setSFXVolume(val);
        game.getAudioManager().setSFXVolume(val);
        game.getAudioManager().playSound("click");
        updateTexts();
    });

    btnSfxUp = std::make_unique<Button>("+", font, sf::Vector2f(810.0f, startY - 5.0f), sf::Vector2f(45.0f, 40.0f), ButtonStyle::DARK);
    btnSfxUp->setOnClick([this]() {
        float val = game.getSettingsManager().getSFXVolume() + 10.0f;
        game.getSettingsManager().setSFXVolume(val);
        game.getAudioManager().setSFXVolume(val);
        game.getAudioManager().playSound("click");
        updateTexts();
    });

    // 2. Nhạc nền (BGM)
    bgmLabel.setFont(font);
    bgmLabel.setString(Theme::toUnicode("Âm Lượng Nhạc Nền (BGM):"));
    bgmLabel.setCharacterSize(24);
    bgmLabel.setFillColor(sf::Color(220, 220, 240));
    bgmLabel.setPosition(320.0f, startY + gapY);

    bgmValText.setFont(font);
    bgmValText.setCharacterSize(24);
    bgmValText.setFillColor(sf::Color(255, 215, 0));
    bgmValText.setPosition(740.0f, startY + gapY);

    btnBgmDown = std::make_unique<Button>("-", font, sf::Vector2f(670.0f, startY + gapY - 5.0f), sf::Vector2f(45.0f, 40.0f), ButtonStyle::DARK);
    btnBgmDown->setOnClick([this]() {
        float val = game.getSettingsManager().getMusicVolume() - 10.0f;
        game.getSettingsManager().setMusicVolume(val);
        game.getAudioManager().setMusicVolume(val);
        game.getAudioManager().playSound("click");
        updateTexts();
    });

    btnBgmUp = std::make_unique<Button>("+", font, sf::Vector2f(810.0f, startY + gapY - 5.0f), sf::Vector2f(45.0f, 40.0f), ButtonStyle::DARK);
    btnBgmUp->setOnClick([this]() {
        float val = game.getSettingsManager().getMusicVolume() + 10.0f;
        game.getSettingsManager().setMusicVolume(val);
        game.getAudioManager().setMusicVolume(val);
        game.getAudioManager().playSound("click");
        updateTexts();
    });

    // 3. Toàn màn hình
    fsLabel.setFont(font);
    fsLabel.setString(Theme::toUnicode("Chế Độ Toàn Màn Hình:"));
    fsLabel.setCharacterSize(24);
    fsLabel.setFillColor(sf::Color(220, 220, 240));
    fsLabel.setPosition(320.0f, startY + gapY * 2);

    btnFsToggle = std::make_unique<Button>("TẮT (Cửa Sổ)", font, sf::Vector2f(670.0f, startY + gapY * 2 - 5.0f), sf::Vector2f(185.0f, 40.0f), ButtonStyle::DARK);
    btnFsToggle->setOnClick([this]() {
        bool current = game.getSettingsManager().isFullscreen();
        game.getSettingsManager().setFullscreen(!current);
        game.getAudioManager().playSound("click");
        updateTexts();
    });

    // Nút Quay Lại
    btnBack = std::make_unique<Button>("Lưu & Quay Lại", font,
                                       sf::Vector2f(WINDOW_WIDTH / 2.0f - 140.0f, 520.0f), sf::Vector2f(280.0f, 50.0f), ButtonStyle::GOLD);
    btnBack->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getSceneManager().changeScene("Menu");
    });

    updateTexts();
}

void SettingsScene::updateTexts() {
    int sfxVal = static_cast<int>(game.getSettingsManager().getSFXVolume());
    sfxValText.setString(std::to_string(sfxVal) + "%");

    int bgmVal = static_cast<int>(game.getSettingsManager().getMusicVolume());
    bgmValText.setString(std::to_string(bgmVal) + "%");

    bool fs = game.getSettingsManager().isFullscreen();
    btnFsToggle->setLabel(fs ? "BẬT (Toàn Màn)" : "TẮT (Cửa Sổ)");
}

void SettingsScene::onEnter() {
    updateTexts();
}

void SettingsScene::handleEvent(const sf::Event& event) {
    btnSfxDown->handleEvent(event, game.getWindow());
    btnSfxUp->handleEvent(event, game.getWindow());
    btnBgmDown->handleEvent(event, game.getWindow());
    btnBgmUp->handleEvent(event, game.getWindow());
    btnFsToggle->handleEvent(event, game.getWindow());
    btnBack->handleEvent(event, game.getWindow());
}

void SettingsScene::update(float dt) {
    btnSfxDown->update(dt);
    btnSfxUp->update(dt);
    btnBgmDown->update(dt);
    btnBgmUp->update(dt);
    btnFsToggle->update(dt);
    btnBack->update(dt);
}

void SettingsScene::render(sf::RenderWindow& window) {
    sf::Sprite background(game.getAssetManager().getTexture("scene_bg"));
    window.draw(background);

    window.draw(headerText);

    window.draw(sfxLabel);
    window.draw(sfxValText);
    btnSfxDown->draw(window);
    btnSfxUp->draw(window);

    window.draw(bgmLabel);
    window.draw(bgmValText);
    btnBgmDown->draw(window);
    btnBgmUp->draw(window);

    window.draw(fsLabel);
    btnFsToggle->draw(window);

    btnBack->draw(window);
}

} // namespace BachChien

