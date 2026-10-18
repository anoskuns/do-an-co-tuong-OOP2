#include "scenes/SaveLoadScene.h"
#include "ui/Theme.h"
#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SceneManager.h"
#include "chess/GameController.h"
#include "third_party/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

namespace BachChien {

SaveLoadScene::SaveLoadScene(Game& g) : Scene(g) {
    const sf::Font& font = game.getAssetManager().getUiFont();

    headerText.setFont(font);
    headerText.setString(Theme::toUnicode("SỬ KÝ VÁN ĐẤU (LƯU & TẢI VÁN CỜ)"));
    headerText.setCharacterSize(38);
    headerText.setFillColor(sf::Color(255, 220, 120));
    headerText.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = headerText.getLocalBounds();
    headerText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
    headerText.setPosition(WINDOW_WIDTH / 2.0f, 50.0f);

    statusText.setFont(font);
    statusText.setCharacterSize(20);
    statusText.setFillColor(sf::Color(120, 255, 160));
    statusText.setPosition(WINDOW_WIDTH / 2.0f - 150.0f, 110.0f);

    float startY = 160.0f;
    float gapY = 115.0f;
    float panelX = 220.0f;
    float panelW = 840.0f;
    float panelH = 95.0f;

    for (int i = 1; i <= 3; ++i) {
        SlotUI ui;
        ui.panel = std::make_unique<Panel>(sf::Vector2f(panelX, startY + (i - 1) * gapY),
                                           sf::Vector2f(panelW, panelH),
                                           "Mật Thất Ván Cờ #" + std::to_string(i), &font);

        ui.infoText.setFont(font);
        ui.infoText.setCharacterSize(18);
        ui.infoText.setFillColor(sf::Color(220, 220, 230));
        ui.infoText.setPosition(panelX + 25.0f, startY + (i - 1) * gapY + 50.0f);

        // Nút Lưu
        ui.btnSave = std::make_unique<Button>("Lưu Ván", font,
                                              sf::Vector2f(panelX + 540.0f, startY + (i - 1) * gapY + 30.0f),
                                              sf::Vector2f(130.0f, 42.0f), ButtonStyle::GOLD);
        int slotIdx = i;
        ui.btnSave->setOnClick([this, slotIdx]() {
            bool ok = game.getGameController().saveGame(slotIdx);
            if (ok) {
                game.getAudioManager().playSound("select");
                statusText.setString(Theme::toUnicode(
                    "Đã lưu ván đấu vào Slot #" + std::to_string(slotIdx) + " thành công!"));
                refreshSlotInfos();
            }
        });

        // Nút Tải
        ui.btnLoad = std::make_unique<Button>("Tải Ván", font,
                                              sf::Vector2f(panelX + 685.0f, startY + (i - 1) * gapY + 30.0f),
                                              sf::Vector2f(130.0f, 42.0f), ButtonStyle::CYAN);
        ui.btnLoad->setOnClick([this, slotIdx]() {
            bool ok = game.getGameController().loadGame(slotIdx);
            if (ok) {
                game.getAudioManager().playSound("select");
                game.getSceneManager().changeScene("Game");
            } else {
                statusText.setString(Theme::toUnicode(
                    "Slot #" + std::to_string(slotIdx) + " chưa có dữ liệu lưu trữ!"));
            }
        });

        slots.push_back(std::move(ui));
    }

    btnBack = std::make_unique<Button>("Quay Lại", font,
                                       sf::Vector2f(WINDOW_WIDTH / 2.0f - 130.0f, 550.0f), sf::Vector2f(260.0f, 48.0f), ButtonStyle::DARK);
    btnBack->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getSceneManager().changeScene("Menu");
    });
}

void SaveLoadScene::refreshSlotInfos() {
    for (size_t i = 0; i < slots.size(); ++i) {
        std::string filePath = assetPath("saves/save_" + std::to_string(i + 1) + ".json");
        if (std::filesystem::exists(filePath)) {
            try {
                std::ifstream f(filePath);
                nlohmann::json j;
                f >> j;

                int modeInt = j.value("mode", 0);
                std::string modeStr = (modeInt == 0) ? "PvP 2 Người" : "PvE vs AI";
                int turnInt = j.value("current_turn", 1);
                std::string turnStr = (turnInt == 1) ? "Thái Hư" : "Trầm Mộc";

                std::string info = "Trạng thái: " + modeStr + " | Lượt kế: " + turnStr;
                slots[i].infoText.setString(sf::String::fromUtf8(info.begin(), info.end()));
            } catch (...) {
                slots[i].infoText.setString(Theme::toUnicode("Dữ liệu hư tổn"));
            }
        } else {
            slots[i].infoText.setString(Theme::toUnicode("Trạng thái: [Trống]"));
        }
    }
}

void SaveLoadScene::onEnter() {
    statusText.setString("");
    refreshSlotInfos();
}

void SaveLoadScene::handleEvent(const sf::Event& event) {
    for (auto& s : slots) {
        s.btnSave->handleEvent(event, game.getWindow());
        s.btnLoad->handleEvent(event, game.getWindow());
    }
    btnBack->handleEvent(event, game.getWindow());
}

void SaveLoadScene::update(float dt) {
    for (auto& s : slots) {
        s.btnSave->update(dt);
        s.btnLoad->update(dt);
    }
    btnBack->update(dt);
}

void SaveLoadScene::render(sf::RenderWindow& window) {
    sf::Sprite background(game.getAssetManager().getTexture("scene_bg"));
    window.draw(background);

    window.draw(headerText);
    window.draw(statusText);

    for (auto& s : slots) {
        s.panel->draw(window);
        window.draw(s.infoText);
        s.btnSave->draw(window);
        s.btnLoad->draw(window);
    }

    btnBack->draw(window);
}

} // namespace BachChien

