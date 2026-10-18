#include "scenes/FactionSelectScene.h"
#include "ui/Theme.h"
#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SceneManager.h"
#include "chess/GameController.h"

namespace BachChien {

FactionSelectScene::FactionSelectScene(Game& g) : Scene(g) {
    const sf::Font& font = game.getAssetManager().getUiFont();

    headerText.setFont(font);
    headerText.setString(Theme::toUnicode("CHỌN TÔNG PHÁI & CẢNH GIỚI ĐỐI PHÁP"));
    headerText.setCharacterSize(38);
    headerText.setFillColor(sf::Color(255, 220, 120));
    headerText.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = headerText.getLocalBounds();
    headerText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
    headerText.setPosition(WINDOW_WIDTH / 2.0f, 60.0f);

    promptFactionText.setFont(font);
    promptFactionText.setString(Theme::toUnicode("Chọn Tông Phái Đại Diện:"));
    promptFactionText.setCharacterSize(24);
    promptFactionText.setFillColor(sf::Color(220, 220, 240));
    promptFactionText.setPosition(180.0f, 150.0f);

    // 2 Nút chọn phe
    btnSideRed = std::make_unique<Button>("Thái Hư Huyễn Phái (Tiên Đạo - Đỏ)", font,
                                          sf::Vector2f(180.0f, 200.0f), sf::Vector2f(420.0f, 55.0f), ButtonStyle::GOLD);
    btnSideRed->setOnClick([this]() {
        selectedSide = Side::RED;
        game.getAudioManager().playSound("click");
        updateSelectionVisuals();
    });

    btnSideBlack = std::make_unique<Button>("Trầm Mộc Đan Tông (Ma Đạo - Đen)", font,
                                            sf::Vector2f(680.0f, 200.0f), sf::Vector2f(420.0f, 55.0f), ButtonStyle::CYAN);
    btnSideBlack->setOnClick([this]() {
        selectedSide = Side::BLACK;
        game.getAudioManager().playSound("click");
        updateSelectionVisuals();
    });

    promptDiffText.setFont(font);
    promptDiffText.setString(Theme::toUnicode("Chọn Cảnh Giới AI (Độ Khó):"));
    promptDiffText.setCharacterSize(24);
    promptDiffText.setFillColor(sf::Color(220, 220, 240));
    promptDiffText.setPosition(180.0f, 300.0f);

    // 3 Nút chọn độ khó AI
    btnDiffEasy = std::make_unique<Button>("Luyện Khí (Dễ)", font,
                                           sf::Vector2f(180.0f, 350.0f), sf::Vector2f(280.0f, 50.0f), ButtonStyle::DARK);
    btnDiffEasy->setOnClick([this]() {
        selectedDiff = Difficulty::EASY;
        game.getAudioManager().playSound("click");
        updateSelectionVisuals();
    });

    btnDiffMedium = std::make_unique<Button>("Trúc Cơ (Trung Bình)", font,
                                             sf::Vector2f(500.0f, 350.0f), sf::Vector2f(280.0f, 50.0f), ButtonStyle::GOLD);
    btnDiffMedium->setOnClick([this]() {
        selectedDiff = Difficulty::MEDIUM;
        game.getAudioManager().playSound("click");
        updateSelectionVisuals();
    });

    btnDiffHard = std::make_unique<Button>("Kim Đan (Khó)", font,
                                           sf::Vector2f(820.0f, 350.0f), sf::Vector2f(280.0f, 50.0f), ButtonStyle::DARK);
    btnDiffHard->setOnClick([this]() {
        selectedDiff = Difficulty::HARD;
        game.getAudioManager().playSound("click");
        updateSelectionVisuals();
    });

    // Nút Bắt đầu và Quay lại
    btnStart = std::make_unique<Button>("BẮT ĐẦU ĐẠI CHIẾN", font,
                                        sf::Vector2f(WINDOW_WIDTH / 2.0f - 210.0f, 520.0f), sf::Vector2f(420.0f, 60.0f), ButtonStyle::GOLD);
    btnStart->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.setPendingConfig(game.getPendingMode(), selectedSide, selectedDiff);
        game.getGameController().startNewGame(game.getPendingMode(), selectedSide, selectedDiff);
        game.getSceneManager().changeScene("Game");
    });

    btnBack = std::make_unique<Button>("Quay Lại", font,
                                       sf::Vector2f(60.0f, WINDOW_HEIGHT - 80.0f), sf::Vector2f(180.0f, 45.0f), ButtonStyle::DARK);
    btnBack->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getSceneManager().changeScene("Menu");
    });

    updateSelectionVisuals();
}

void FactionSelectScene::updateSelectionVisuals() {
    // Nhãn nút thể hiện trạng thái được chọn
    btnSideRed->setLabel(selectedSide == Side::RED ? ">>> Thái Hư Huyễn Phái (Đã Chọn) <<<" : "Thái Hư Huyễn Phái (Tiên Đạo)");
    btnSideBlack->setLabel(selectedSide == Side::BLACK ? ">>> Trầm Mộc Đan Tông (Đã Chọn) <<<" : "Trầm Mộc Đan Tông (Ma Đạo)");

    btnDiffEasy->setLabel(selectedDiff == Difficulty::EASY ? "[ Luyện Khí (Dễ) ]" : "Luyện Khí (Dễ)");
    btnDiffMedium->setLabel(selectedDiff == Difficulty::MEDIUM ? "[ Trúc Cơ (Vừa) ]" : "Trúc Cơ (Vừa)");
    btnDiffHard->setLabel(selectedDiff == Difficulty::HARD ? "[ Kim Đan (Khó) ]" : "Kim Đan (Khó)");
}

void FactionSelectScene::onEnter() {
    selectedSide = game.getPendingPlayerSide();
    selectedDiff = game.getPendingDifficulty();
    updateSelectionVisuals();
}

void FactionSelectScene::handleEvent(const sf::Event& event) {
    btnSideRed->handleEvent(event, game.getWindow());
    btnSideBlack->handleEvent(event, game.getWindow());

    if (game.getPendingMode() == GameMode::PVE) {
        btnDiffEasy->handleEvent(event, game.getWindow());
        btnDiffMedium->handleEvent(event, game.getWindow());
        btnDiffHard->handleEvent(event, game.getWindow());
    }

    btnStart->handleEvent(event, game.getWindow());
    btnBack->handleEvent(event, game.getWindow());
}

void FactionSelectScene::update(float dt) {
    btnSideRed->update(dt);
    btnSideBlack->update(dt);
    if (game.getPendingMode() == GameMode::PVE) {
        btnDiffEasy->update(dt);
        btnDiffMedium->update(dt);
        btnDiffHard->update(dt);
    }
    btnStart->update(dt);
    btnBack->update(dt);
}

void FactionSelectScene::render(sf::RenderWindow& window) {
    sf::Sprite background(game.getAssetManager().getTexture("scene_bg"));
    window.draw(background);

    window.draw(headerText);
    window.draw(promptFactionText);

    btnSideRed->draw(window);
    btnSideBlack->draw(window);

    if (game.getPendingMode() == GameMode::PVE) {
        window.draw(promptDiffText);
        btnDiffEasy->draw(window);
        btnDiffMedium->draw(window);
        btnDiffHard->draw(window);
    }

    btnStart->draw(window);
    btnBack->draw(window);
}

} // namespace BachChien

