#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SettingsManager.h"
#include "core/SceneManager.h"
#include "chess/GameController.h"
#include "ui/Theme.h"

#include "scenes/MenuScene.h"
#include "scenes/FactionSelectScene.h"
#include "scenes/GameScene.h"
#include "scenes/SettingsScene.h"
#include "scenes/HelpScene.h"
#include "scenes/ResultScene.h"
#include "scenes/SaveLoadScene.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>

namespace BachChien {

Game::Game() {
    assetManager = std::make_unique<AssetManager>();
    audioManager = std::make_unique<AudioManager>();
    settingsManager = std::make_unique<SettingsManager>();
    gameController = std::make_unique<GameController>();
    sceneManager = std::make_unique<SceneManager>();
}

Game::~Game() = default;

bool Game::init() {
    // 1. Tạo cửa sổ SFML 1280x720
    const sf::Uint32 style = settingsManager->isFullscreen()
                           ? sf::Style::Fullscreen
                           : (sf::Style::Titlebar | sf::Style::Close);
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                  Theme::toUnicode("Bách Chiến Tiên Cục — Cờ Tướng Tu Tiên"),
                  style);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // 2. Khởi tạo tài nguyên đồ họa và âm thanh
    if (!assetManager->init()) {
        return false;
    }

    // Biểu tượng cửa sổ lấy từ con dấu của trò chơi.
    if (assetManager->hasTexture("emblem")) {
        const sf::Image icon = assetManager->getTexture("emblem").copyToImage();
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    audioManager->init();
    audioManager->setSFXVolume(settingsManager->getSFXVolume());
    audioManager->setMusicVolume(settingsManager->getMusicVolume());

    // 3. Khởi tạo và liên kết toàn bộ 7 Cảnh (Scenes)
    sceneManager->addScene("Menu", std::make_unique<MenuScene>(*this));
    sceneManager->addScene("FactionSelect", std::make_unique<FactionSelectScene>(*this));
    sceneManager->addScene("Game", std::make_unique<GameScene>(*this));
    sceneManager->addScene("Settings", std::make_unique<SettingsScene>(*this));
    sceneManager->addScene("Help", std::make_unique<HelpScene>(*this));
    sceneManager->addScene("Result", std::make_unique<ResultScene>(*this));
    sceneManager->addScene("SaveLoad", std::make_unique<SaveLoadScene>(*this));

    // 4. Mở màn hình chính
    sceneManager->changeScene("Menu");

    return true;
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        sceneManager->handleEvent(event);
    }
}

void Game::update(float dt) {
    sceneManager->update(dt);
}

void Game::render() {
    window.clear(Theme::Palette::Ink);
    sceneManager->render(window);
    window.display();
}

void Game::captureScreenshots(const std::string& outputDir) {
    std::error_code ec;
    std::filesystem::create_directories(outputDir, ec);

    const std::vector<std::string> sceneNames = {
        "Menu", "FactionSelect", "Game", "Settings", "Help", "Result", "SaveLoad"
    };

    for (const std::string& name : sceneNames) {
        sceneManager->changeScene(name);

        // Ở màn hình ván cờ, chọn sẵn một quân để ảnh chụp thể hiện cả
        // phần gợi ý nước đi hợp lệ.
        if (name == "Game") {
            gameController->handleSquareClick(9, 0);
        }

        // Chạy vài khung hình để các hiệu ứng chuyển động ổn định.
        for (int frame = 0; frame < 40; ++frame) {
            processEvents();
            update(1.0f / 60.0f);
        }

        // Vẽ vào bộ đệm sau rồi sao chép ra ảnh TRƯỚC khi hoán đổi bộ đệm,
        // nếu không nội dung đọc được sẽ không xác định.
        window.clear(Theme::Palette::Ink);
        sceneManager->render(window);

        sf::Texture frameTexture;
        if (frameTexture.create(window.getSize().x, window.getSize().y)) {
            frameTexture.update(window);
            const std::string filePath = outputDir + "/" + name + ".png";
            if (frameTexture.copyToImage().saveToFile(filePath)) {
                std::cout << "[Ảnh chụp] Đã lưu " << filePath << "\n";
            } else {
                std::cerr << "[Ảnh chụp] Không ghi được " << filePath << "\n";
            }
        }
        window.display();
    }

    window.close();
}

void Game::run() {
    if (const char* screenshotDir = std::getenv("BCT_SCREENSHOT")) {
        captureScreenshots(screenshotDir);
        return;
    }

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; // Giới hạn dt phòng trường hợp lag/giật

        processEvents();
        update(dt);
        render();
    }
}

void Game::quit() {
    window.close();
}

} // namespace BachChien
