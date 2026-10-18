#pragma once

#include "core/Common.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

namespace BachChien {

class SceneManager;
class AssetManager;
class AudioManager;
class SettingsManager;
class GameController;

class Game {
private:
    sf::RenderWindow window;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<AudioManager> audioManager;
    std::unique_ptr<SettingsManager> settingsManager;
    std::unique_ptr<GameController> gameController;

    // Cấu hình tạm thời khi chuyển từ FactionSelect sang GameScene
    GameMode pendingMode = GameMode::PVE;
    Side pendingPlayerSide = Side::RED;
    Difficulty pendingDifficulty = Difficulty::MEDIUM;

    void processEvents();
    void update(float dt);
    void render();

    // Chế độ chụp ảnh giao diện: lần lượt dựng từng màn hình rồi lưu ra PNG.
    // Kích hoạt bằng biến môi trường BCT_SCREENSHOT=<thư mục đầu ra>.
    void captureScreenshots(const std::string& outputDir);

public:
    Game();
    ~Game();

    bool init();
    void run();
    void quit();

    sf::RenderWindow& getWindow() { return window; }
    SceneManager& getSceneManager() { return *sceneManager; }
    AssetManager& getAssetManager() { return *assetManager; }
    AudioManager& getAudioManager() { return *audioManager; }
    SettingsManager& getSettingsManager() { return *settingsManager; }
    GameController& getGameController() { return *gameController; }

    void setPendingConfig(GameMode m, Side s, Difficulty d) {
        pendingMode = m;
        pendingPlayerSide = s;
        pendingDifficulty = d;
    }
    GameMode getPendingMode() const { return pendingMode; }
    Side getPendingPlayerSide() const { return pendingPlayerSide; }
    Difficulty getPendingDifficulty() const { return pendingDifficulty; }
};

} // namespace BachChien

