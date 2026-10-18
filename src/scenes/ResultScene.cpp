#include "scenes/ResultScene.h"
#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SceneManager.h"
#include "chess/GameController.h"

namespace BachChien {

ResultScene::ResultScene(Game& g) : Scene(g) {
    const sf::Font& font = game.getAssetManager().getUiFont();

    resultTitle.setFont(font);
    resultTitle.setCharacterSize(48);
    resultTitle.setStyle(sf::Text::Bold);

    resultDesc.setFont(font);
    resultDesc.setCharacterSize(24);

    movesCountText.setFont(font);
    movesCountText.setCharacterSize(20);
    movesCountText.setFillColor(sf::Color(220, 200, 150));

    btnRematch = std::make_unique<Button>("Tái Đấu Ván Mới", font,
                                          sf::Vector2f(WINDOW_WIDTH / 2.0f - 200.0f, 440.0f), sf::Vector2f(400.0f, 54.0f), ButtonStyle::GOLD);
    btnRematch->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getGameController().startNewGame(game.getPendingMode(), game.getPendingPlayerSide(), game.getPendingDifficulty());
        game.getSceneManager().changeScene("Game");
    });

    btnMenu = std::make_unique<Button>("Trở Về Menu Chính", font,
                                       sf::Vector2f(WINDOW_WIDTH / 2.0f - 200.0f, 515.0f), sf::Vector2f(400.0f, 54.0f), ButtonStyle::DARK);
    btnMenu->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getSceneManager().changeScene("Menu");
    });
}

void ResultScene::onEnter() {
    GameState state = game.getGameController().getState();
    std::string titleStr;
    std::string descStr;

    if (state == GameState::RED_WON) {
        titleStr = "THÁI HƯ HUYỄN PHÁI ĐẮC ĐẠO CHIẾN THẮNG!";
        descStr = "Chân chính Tiên Đạo quang huy vạn dặm, trấn áp quần ma!";
        resultTitle.setFillColor(sf::Color(255, 215, 60));
        resultDesc.setFillColor(sf::Color(255, 180, 100));
        particleSystem.emitCaptureExplosion(sf::Vector2f(WINDOW_WIDTH / 2.0f, 200.0f), sf::Color(255, 215, 0), 60);
    } else if (state == GameState::BLACK_WON) {
        titleStr = "TRẦM MỘC ĐAN TÔNG ĐOẠT ĐƯỢC THIÊN CƠ!";
        descStr = "U minh ma hỏa chấn nhiếp bát hoang, ma đạo hưng thịnh!";
        resultTitle.setFillColor(sf::Color(90, 220, 255));
        resultDesc.setFillColor(sf::Color(140, 240, 255));
        particleSystem.emitCaptureExplosion(sf::Vector2f(WINDOW_WIDTH / 2.0f, 200.0f), sf::Color(80, 220, 255), 60);
    } else {
        titleStr = "LƯỠNG PHÁI BÌNH HÒA - ĐỐI PHÁP BẤT PHÂN!";
        descStr = "Thiên địa vận chuyển, hai bên tương dung tương khắc.";
        resultTitle.setFillColor(sf::Color(220, 220, 220));
        resultDesc.setFillColor(sf::Color(200, 200, 200));
    }

    resultTitle.setString(sf::String::fromUtf8(titleStr.begin(), titleStr.end()));
    sf::FloatRect tb = resultTitle.getLocalBounds();
    resultTitle.setOrigin(tb.left + tb.width / 2.0f, tb.top);
    resultTitle.setPosition(WINDOW_WIDTH / 2.0f, 160.0f);

    resultDesc.setString(sf::String::fromUtf8(descStr.begin(), descStr.end()));
    sf::FloatRect db = resultDesc.getLocalBounds();
    resultDesc.setOrigin(db.left + db.width / 2.0f, db.top);
    resultDesc.setPosition(WINDOW_WIDTH / 2.0f, 240.0f);

    int totalMoves = static_cast<int>(game.getGameController().getBoard().getHistory().size());
    std::string countStr = "Tổng số nước cờ đã giao tranh: " + std::to_string(totalMoves) + " nước";
    movesCountText.setString(sf::String::fromUtf8(countStr.begin(), countStr.end()));
    sf::FloatRect cb = movesCountText.getLocalBounds();
    movesCountText.setOrigin(cb.left + cb.width / 2.0f, cb.top);
    movesCountText.setPosition(WINDOW_WIDTH / 2.0f, 310.0f);
}

void ResultScene::handleEvent(const sf::Event& event) {
    btnRematch->handleEvent(event, game.getWindow());
    btnMenu->handleEvent(event, game.getWindow());
}

void ResultScene::update(float dt) {
    particleSystem.update(dt);
    btnRematch->update(dt);
    btnMenu->update(dt);
}

void ResultScene::render(sf::RenderWindow& window) {
    sf::Sprite background(game.getAssetManager().getTexture("scene_bg"));
    window.draw(background);

    particleSystem.draw(window, game.getAssetManager().getTexture("particle_spark"));

    window.draw(resultTitle);
    window.draw(resultDesc);
    window.draw(movesCountText);

    btnRematch->draw(window);
    btnMenu->draw(window);
}

} // namespace BachChien

