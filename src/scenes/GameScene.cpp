#include "scenes/GameScene.h"
#include "core/Game.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/SceneManager.h"
#include "chess/GameController.h"
#include "ui/Theme.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace BachChien {

namespace {

// Bố cục bảng điều khiển bên phải (tính từ mép trên của Panel).
constexpr float CARD_TOP = 84.0f;
constexpr float CARD_HEIGHT = 88.0f;
constexpr float BUTTON_TOP = 452.0f;
constexpr float BUTTON_HEIGHT = 46.0f;
constexpr float BUTTON_GAP = 54.0f;

std::string factionName(Side side) {
    return (side == Side::RED) ? "THÁI HƯ HUYỄN PHÁI" : "TRẦM MỘC ĐAN TÔNG";
}

std::string factionTitle(Side side) {
    return (side == Side::RED) ? "Tiên Môn Chính Đạo" : "Ma Đạo U Minh";
}

sf::Color factionColor(Side side) {
    return (side == Side::RED) ? Theme::Palette::CrimsonBright : Theme::Palette::Jade;
}

} // namespace

GameScene::GameScene(Game& g) : Scene(g),
    sidebarPanel(sf::Vector2f(SIDEBAR_X, SIDEBAR_Y),
                 sf::Vector2f(SIDEBAR_WIDTH, SIDEBAR_HEIGHT),
                 "BÁCH CHIẾN TIÊN CỤC", &game.getAssetManager().getUiFont()) {

    // Chữ Latin/tiếng Việt dùng font giao diện; Hán tự dùng font thư pháp.
    const sf::Font& font = game.getAssetManager().getUiFont();

    const float innerX = SIDEBAR_X + 24.0f;
    const float cardY = SIDEBAR_Y + CARD_TOP;

    // --- Nhãn "LƯỢT ĐI" ---
    turnLabelText.setFont(font);
    turnLabelText.setString(Theme::toUnicode("LƯỢT ĐI"));
    turnLabelText.setCharacterSize(14);
    turnLabelText.setLetterSpacing(2.0f);
    turnLabelText.setFillColor(Theme::Palette::TextDim);
    turnLabelText.setPosition(innerX + 22.0f, cardY + 14.0f);

    // --- Tên tông phái đang tới lượt ---
    turnNameText.setFont(font);
    turnNameText.setCharacterSize(25);
    turnNameText.setStyle(sf::Text::Bold);
    turnNameText.setPosition(innerX + 22.0f, cardY + 34.0f);

    // --- Tông phái đối phương ---
    opponentText.setFont(font);
    opponentText.setCharacterSize(14);
    opponentText.setFillColor(Theme::Palette::TextDim);
    opponentText.setPosition(innerX + 22.0f, cardY + 66.0f);

    // --- Trạng thái thế cờ ---
    stateText.setFont(font);
    stateText.setCharacterSize(18);
    stateText.setPosition(innerX, cardY + CARD_HEIGHT + 14.0f);

    // --- AI đang tính toán ---
    aiThinkingText.setFont(font);
    aiThinkingText.setString(Theme::toUnicode("Tiên Cơ đang ngưng thần tính toán..."));
    aiThinkingText.setCharacterSize(16);
    aiThinkingText.setStyle(sf::Text::Italic);
    aiThinkingText.setFillColor(sf::Color(120, 224, 255));
    aiThinkingText.setPosition(innerX, cardY + CARD_HEIGHT + 42.0f);

    // --- Tiêu đề sử ký nước cờ ---
    historyTitleText.setFont(font);
    historyTitleText.setString(Theme::toUnicode("SỬ KÝ ĐỐI PHÁP"));
    historyTitleText.setCharacterSize(15);
    historyTitleText.setLetterSpacing(1.6f);
    historyTitleText.setFillColor(Theme::Palette::Gold);
    historyTitleText.setPosition(innerX, cardY + CARD_HEIGHT + 78.0f);

    // --- 5 dòng lịch sử nước cờ gần nhất ---
    for (int i = 0; i < 5; ++i) {
        sf::Text line;
        line.setFont(font);
        line.setCharacterSize(16);
        line.setFillColor(sf::Color(198, 204, 220));
        line.setPosition(innerX + 14.0f, cardY + CARD_HEIGHT + 110.0f + i * 28.0f);
        historyTexts.push_back(line);
    }

    // --- Ghi chú triết lý tu chân ---
    // Tách thành hai đối tượng chữ vì phần Hán tự và phần tiếng Việt phải
    // dùng hai font khác nhau mới hiển thị đúng.
    const float footerY = SIDEBAR_Y + BUTTON_TOP + BUTTON_GAP * 3.0f + 4.0f;

    footerHanText.setFont(game.getAssetManager().getFont());
    footerHanText.setString(Theme::toUnicode("落 子 无 悔"));
    footerHanText.setCharacterSize(18);
    footerHanText.setFillColor(Theme::withAlpha(Theme::Palette::Gold, 210));
    {
        const sf::FloatRect bounds = footerHanText.getLocalBounds();
        footerHanText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top);
        footerHanText.setPosition(SIDEBAR_X + SIDEBAR_WIDTH * 0.5f, footerY);
    }

    footerText.setFont(font);
    footerText.setString(Theme::toUnicode("Lạc Tử Vô Hối — đã hạ ấn, không hối hận"));
    footerText.setCharacterSize(13);
    footerText.setFillColor(sf::Color(138, 146, 166));
    {
        const sf::FloatRect bounds = footerText.getLocalBounds();
        footerText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top);
        footerText.setPosition(SIDEBAR_X + SIDEBAR_WIDTH * 0.5f, footerY + 24.0f);
    }

    // --- Các nút chức năng (không có nút Hoàn Tác theo đúng triết lý "Lạc tử vô hối") ---
    const float buttonX = SIDEBAR_X + 24.0f;
    const float buttonW = SIDEBAR_WIDTH - 48.0f;
    const float buttonY = SIDEBAR_Y + BUTTON_TOP;

    btnSave = std::make_unique<Button>("Lưu Ván Đấu", font,
                                       sf::Vector2f(buttonX, buttonY),
                                       sf::Vector2f(buttonW, BUTTON_HEIGHT), ButtonStyle::GOLD);
    btnSave->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getSceneManager().changeScene("SaveLoad");
    });

    btnNewGame = std::make_unique<Button>("Khai Cuộc Mới", font,
                                          sf::Vector2f(buttonX, buttonY + BUTTON_GAP),
                                          sf::Vector2f(buttonW, BUTTON_HEIGHT), ButtonStyle::CYAN);
    btnNewGame->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getGameController().startNewGame(game.getPendingMode(), game.getPendingPlayerSide(),
                                              game.getPendingDifficulty());
        particleSystem.clear();
    });

    btnMenu = std::make_unique<Button>("Thoát Về Menu", font,
                                       sf::Vector2f(buttonX, buttonY + BUTTON_GAP * 2.0f),
                                       sf::Vector2f(buttonW, BUTTON_HEIGHT), ButtonStyle::DARK);
    btnMenu->setOnClick([this]() {
        game.getAudioManager().playSound("click");
        game.getSceneManager().changeScene("Menu");
    });

    // --- Liên kết sự kiện từ GameController sang hiệu ứng hình ảnh ---
    auto& controller = game.getGameController();

    controller.setOnMoveMadeCallback([this](const Move& move) {
        game.getAudioManager().playSound("move");
        const sf::Vector2f fromPos = getCellCenter(move.from.row, move.from.col);
        const sf::Vector2f toPos = getCellCenter(move.to.row, move.to.col);
        const sf::Color trailColor = (move.movedSide == Side::RED)
                                   ? sf::Color(255, 130, 70, 210)
                                   : sf::Color(90, 220, 235, 210);
        particleSystem.emitMoveTrail(fromPos, toPos, trailColor, 22);
    });

    controller.setOnPieceCapturedCallback([this](Position pos) {
        game.getAudioManager().playSound("capture");
        particleSystem.emitCaptureExplosion(getCellCenter(pos.row, pos.col),
                                            sf::Color(255, 206, 96), 38);
    });

    controller.setOnCheckCallback([this](Side sideInCheck) {
        game.getAudioManager().playSound("check");
        lastMoveWasCheck = true;
        const Position kingPos = game.getGameController().getBoard().getKingPosition(sideInCheck);
        particleSystem.emitCheckAura(getCellCenter(kingPos.row, kingPos.col), 18);
    });

    controller.setOnGameOverCallback([this](Side winner) {
        game.getAudioManager().playSound(winner == Side::RED ? "victory" : "defeat");
        game.getSceneManager().changeScene("Result");
    });
}

void GameScene::onEnter() {
    lastMoveWasCheck = false;
    game.getAudioManager().playBGM("battle_bgm");
}

void GameScene::onExit() {
}

sf::Vector2f GameScene::getCellCenter(int row, int col) const {
    return sf::Vector2f(BOARD_OFFSET_X + col * TILE_SIZE, BOARD_OFFSET_Y + row * TILE_SIZE);
}

Position GameScene::getCellFromPixel(float x, float y) const {
    // Xác định ô gần nhất rồi kiểm tra khoảng cách để việc bấm chuột dễ dàng hơn.
    const int col = static_cast<int>(std::lround((x - BOARD_OFFSET_X) / TILE_SIZE));
    const int row = static_cast<int>(std::lround((y - BOARD_OFFSET_Y) / TILE_SIZE));
    const Position candidate{row, col};
    if (!candidate.isValid()) {
        return Position{-1, -1};
    }

    const sf::Vector2f center = getCellCenter(row, col);
    const float dx = x - center.x;
    const float dy = y - center.y;
    if (std::sqrt(dx * dx + dy * dy) <= TILE_SIZE * 0.55f) {
        return candidate;
    }
    return Position{-1, -1};
}

void GameScene::handleEvent(const sf::Event& event) {
    btnSave->handleEvent(event, game.getWindow());
    btnNewGame->handleEvent(event, game.getWindow());
    btnMenu->handleEvent(event, game.getWindow());

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f mousePos =
            game.getWindow().mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
        const Position cell = getCellFromPixel(mousePos.x, mousePos.y);
        if (cell.isValid()) {
            game.getGameController().handleSquareClick(cell.row, cell.col);
        }
    }
}

void GameScene::update(float dt) {
    elapsed += dt;
    game.getGameController().update(dt);
    particleSystem.update(dt);

    btnSave->update(dt);
    btnNewGame->update(dt);
    btnMenu->update(dt);

    auto& controller = game.getGameController();

    // --- Thẻ thông tin lượt đi ---
    const Side turn = controller.getCurrentTurn();
    const Side waiting = getOpponentSide(turn);
    turnNameText.setString(Theme::toUnicode(factionName(turn)));
    turnNameText.setFillColor(factionColor(turn));
    opponentText.setString(Theme::toUnicode("Đối thủ: " + factionName(waiting) + "  •  " + factionTitle(waiting)));

    // --- Trạng thái thế cờ ---
    const GameState state = controller.getState();
    switch (state) {
        case GameState::CHECK:
            stateText.setString(Theme::toUnicode("CẢNH BÁO: ĐANG BỊ CHIẾU TƯỚNG!"));
            stateText.setFillColor(sf::Color(255, 96, 84));
            break;
        case GameState::RED_WON:
            stateText.setString(Theme::toUnicode("THÁI HƯ HUYỄN PHÁI CHIẾN THẮNG!"));
            stateText.setFillColor(Theme::Palette::GoldBright);
            break;
        case GameState::BLACK_WON:
            stateText.setString(Theme::toUnicode("TRẦM MỘC ĐAN TÔNG CHIẾN THẮNG!"));
            stateText.setFillColor(Theme::Palette::JadeBright);
            break;
        case GameState::DRAW:
            stateText.setString(Theme::toUnicode("HÒA CỜ — Cân tài cân sức"));
            stateText.setFillColor(Theme::Palette::TextLight);
            break;
        default:
            stateText.setString(Theme::toUnicode("Thế cờ: giao tranh quyết liệt"));
            stateText.setFillColor(sf::Color(176, 216, 176));
            break;
    }

    // --- 5 nước cờ gần nhất ---
    const auto& history = controller.getBoard().getHistory();
    const int total = static_cast<int>(history.size());
    for (int i = 0; i < 5; ++i) {
        const int index = total - 5 + i;
        if (index >= 0 && index < total) {
            historyTexts[i].setString(Theme::toUnicode(history[index].notation));
            historyTexts[i].setFillColor(i == 4 ? Theme::Palette::GoldBright
                                                : sf::Color(198, 204, 220));
        } else if (total == 0 && i == 0) {
            // Chưa hạ nước nào: hiển thị lời nhắc thay vì các dòng trống.
            historyTexts[i].setString(Theme::toUnicode("Chưa có nước cờ nào được hạ..."));
            historyTexts[i].setFillColor(sf::Color(110, 118, 136));
        } else {
            historyTexts[i].setString("");
        }
    }
}

void GameScene::drawBoard(sf::RenderWindow& window) {
    auto& assets = game.getAssetManager();
    const sf::Texture& boardTexture = assets.getTexture("board");
    const sf::Vector2u textureSize = boardTexture.getSize();
    if (textureSize.x == 0 || textureSize.y == 0) {
        return;
    }

    // Texture bàn cờ bao gồm cả khung gỗ, nên gốc vẽ lùi vào một khoảng bằng
    // bề rộng khung so với gốc lưới cờ.
    const float boardWidth = (BOARD_COLS - 1) * TILE_SIZE + BOARD_MARGIN * 2.0f;
    const float boardHeight = (BOARD_ROWS - 1) * TILE_SIZE + BOARD_MARGIN * 2.0f;

    sf::Sprite board(boardTexture);
    board.setPosition(BOARD_OFFSET_X - BOARD_MARGIN, BOARD_OFFSET_Y - BOARD_MARGIN);
    board.setScale(boardWidth / static_cast<float>(textureSize.x),
                   boardHeight / static_cast<float>(textureSize.y));
    window.draw(board);
}

void GameScene::drawHighlights(sf::RenderWindow& window) {
    const auto& controller = game.getGameController();

    // --- 1. Nước đi vừa rồi: hai ô sáng mờ + khung góc kim tuyến ---
    const Move& lastMove = controller.getLastMove();
    if (lastMove.from.isValid() && lastMove.to.isValid()) {
        const sf::Vector2f fromCenter = getCellCenter(lastMove.from.row, lastMove.from.col);
        const sf::Vector2f toCenter = getCellCenter(lastMove.to.row, lastMove.to.col);

        auto drawMoveMarker = [&](sf::Vector2f center, sf::Uint8 alpha) {
            Theme::drawRoundedRect(window,
                                   sf::FloatRect(center.x - TILE_SIZE * 0.42f, center.y - TILE_SIZE * 0.42f,
                                                 TILE_SIZE * 0.84f, TILE_SIZE * 0.84f),
                                   8.0f, sf::Color(255, 216, 120, static_cast<sf::Uint8>(alpha / 5)),
                                   sf::Color(255, 216, 120, alpha), 1.6f);
        };

        drawMoveMarker(fromCenter, 90);
        drawMoveMarker(toCenter, 165);
        Theme::drawGlow(window, toCenter, TILE_SIZE * 0.75f, sf::Color(255, 208, 110, 60), 1.0f);
    }

    // --- 2. Quân đang chọn: quầng sáng nhấp nháy nhẹ ---
    const Piece* selected = controller.getSelectedPiece();
    if (selected == nullptr) {
        return;
    }

    const sf::Vector2f selectedCenter = getCellCenter(selected->getRow(), selected->getCol());
    const float pulse = 0.5f + 0.5f * std::sin(elapsed * 5.0f);

    Theme::drawGlow(window, selectedCenter, TILE_SIZE * (0.82f + 0.06f * pulse),
                    sf::Color(255, 226, 120, static_cast<sf::Uint8>(80.0f + 50.0f * pulse)), 1.0f);
    Theme::drawRoundedRect(window,
                           sf::FloatRect(selectedCenter.x - TILE_SIZE * 0.44f,
                                         selectedCenter.y - TILE_SIZE * 0.44f,
                                         TILE_SIZE * 0.88f, TILE_SIZE * 0.88f),
                           10.0f, sf::Color::Transparent,
                           sf::Color(255, 236, 150, 235), 2.4f);

    // --- 3. Các nước đi hợp lệ của quân đang chọn ---
    for (const Move& move : controller.getLegalMovesForSelected()) {
        const sf::Vector2f target = getCellCenter(move.to.row, move.to.col);

        if (move.isCapture()) {
            // Ô có thể ăn quân: vòng tròn đỏ huyết nhấp nháy.
            const sf::Color captureColor(255, 74, 62);
            Theme::drawGlow(window, target, TILE_SIZE * 0.70f,
                            Theme::withAlpha(captureColor, static_cast<sf::Uint8>(70.0f + 60.0f * pulse)), 1.0f);

            const float ringRadius = TILE_SIZE * 0.40f;
            for (int i = 0; i < 96; ++i) {
                const float a0 = (2.0f * 3.14159265f) * i / 96.0f;
                const float a1 = (2.0f * 3.14159265f) * (i + 1) / 96.0f;
                sf::Vertex quad[4];
                const sf::Color color = Theme::withAlpha(captureColor,
                                                         static_cast<sf::Uint8>(200.0f + 45.0f * pulse));
                quad[0] = sf::Vertex(target + sf::Vector2f(std::cos(a0), std::sin(a0)) * ringRadius, color);
                quad[1] = sf::Vertex(target + sf::Vector2f(std::cos(a1), std::sin(a1)) * ringRadius, color);
                quad[2] = sf::Vertex(target + sf::Vector2f(std::cos(a1), std::sin(a1)) * (ringRadius - 3.0f), color);
                quad[3] = sf::Vertex(target + sf::Vector2f(std::cos(a0), std::sin(a0)) * (ringRadius - 3.0f), color);
                window.draw(quad, 4, sf::Quads);
            }
        } else {
            // Ô trống: chấm ngọc lam nhỏ, có quầng sáng.
            Theme::drawGlow(window, target, TILE_SIZE * 0.36f, sf::Color(90, 240, 200, 80), 1.0f);
            sf::CircleShape dot(TILE_SIZE * 0.115f, 28);
            dot.setOrigin(TILE_SIZE * 0.115f, TILE_SIZE * 0.115f);
            dot.setPosition(target);
            dot.setFillColor(sf::Color(150, 255, 226, 235));
            dot.setOutlineThickness(1.4f);
            dot.setOutlineColor(sf::Color(40, 190, 160, 200));
            window.draw(dot);
        }
    }
}

void GameScene::drawPieces(sf::RenderWindow& window) {
    auto& assets = game.getAssetManager();

    for (const auto& piece : game.getGameController().getBoard().getPieces()) {
        if (!piece->isAlive()) {
            continue;
        }

        const std::string prefix = (piece->getSide() == Side::RED) ? "red_" : "black_";
        std::string suffix;
        switch (piece->getType()) {
            case PieceType::KING:   suffix = "king"; break;
            case PieceType::GUARD:  suffix = "guard"; break;
            case PieceType::BISHOP: suffix = "bishop"; break;
            case PieceType::KNIGHT: suffix = "knight"; break;
            case PieceType::ROOK:   suffix = "rook"; break;
            case PieceType::CANNON: suffix = "cannon"; break;
            case PieceType::PAWN:   suffix = "pawn"; break;
            default: break;
        }
        if (suffix.empty()) {
            continue;
        }

        const sf::Texture& texture = assets.getTexture(prefix + suffix);
        const sf::Vector2u size = texture.getSize();
        if (size.x == 0 || size.y == 0) {
            continue;
        }

        const sf::Vector2f center = getCellCenter(piece->getRow(), piece->getCol());

        // Bóng đổ mềm dưới chân quân cờ, giúp quân "nổi" khỏi mặt bàn.
        Theme::drawGlow(window, sf::Vector2f(center.x + 2.0f, center.y + 5.0f),
                        TILE_SIZE * 0.44f, sf::Color(0, 0, 0, 95), 1.0f);

        // Tỉ lệ được tính sao cho đường kính đĩa cờ chiếm PIECE_DISPLAY_RATIO ô cờ.
        const float discDiameter = TILE_SIZE * PIECE_DISPLAY_RATIO;
        const float scale = discDiameter / (static_cast<float>(size.x) * PIECE_DISC_FRACTION);

        sf::Sprite sprite(texture);
        sprite.setOrigin(size.x * 0.5f, size.y * 0.5f);
        sprite.setScale(scale, scale);
        sprite.setPosition(center);
        window.draw(sprite);
    }
}

void GameScene::drawSidebar(sf::RenderWindow& window) {
    sidebarPanel.draw(window);

    const float innerX = SIDEBAR_X + 24.0f;
    const float innerW = SIDEBAR_WIDTH - 48.0f;
    const float cardY = SIDEBAR_Y + CARD_TOP;

    const Side turn = game.getGameController().getCurrentTurn();
    const sf::Color accent = factionColor(turn);

    // --- Thẻ lượt đi: nền chuyển sắc + thanh màu phe bên trái ---
    const sf::FloatRect card(innerX, cardY, innerW, CARD_HEIGHT);
    Theme::drawSoftShadow(window, card, 10.0f, 10.0f, 130);
    Theme::drawRoundedRectGradient(window, card, 10.0f,
                                   sf::Color(38, 44, 62, 225), sf::Color(20, 24, 36, 235));

    Theme::drawGlow(window, sf::Vector2f(innerX + 40.0f, cardY + CARD_HEIGHT * 0.5f),
                    100.0f, Theme::withAlpha(accent, 55), 1.0f);

    // Thanh màu tông phái
    Theme::drawRoundedRect(window,
                           sf::FloatRect(card.left + 8.0f, card.top + 12.0f, 5.0f, CARD_HEIGHT - 24.0f),
                           2.5f, accent);
    Theme::drawRoundedRect(window, card, 10.0f, sf::Color::Transparent,
                           Theme::withAlpha(accent, 90), 1.4f);

    window.draw(turnLabelText);
    window.draw(turnNameText);
    window.draw(opponentText);

    // Vòng tròn huy hiệu nhỏ mang chữ Hán của phe đang đi.
    const sf::Vector2f badgeCenter(card.left + card.width - 42.0f, card.top + CARD_HEIGHT * 0.5f);
    Theme::drawGlow(window, badgeCenter, 34.0f, Theme::withAlpha(accent, 90), 1.0f);
    sf::CircleShape badge(21.0f, 48);
    badge.setOrigin(21.0f, 21.0f);
    badge.setPosition(badgeCenter);
    badge.setFillColor(Theme::withAlpha(sf::Color(16, 20, 30), 235));
    badge.setOutlineThickness(1.8f);
    badge.setOutlineColor(Theme::withAlpha(accent, 210));
    window.draw(badge);

    sf::Text badgeGlyph;
    badgeGlyph.setFont(game.getAssetManager().getFont());
    badgeGlyph.setString(Theme::toUnicode(turn == Side::RED ? "帥" : "將"));
    badgeGlyph.setCharacterSize(24);
    badgeGlyph.setStyle(sf::Text::Bold);
    badgeGlyph.setFillColor(Theme::withAlpha(accent, 240));
    {
        const sf::FloatRect bounds = badgeGlyph.getLocalBounds();
        badgeGlyph.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        badgeGlyph.setPosition(badgeCenter);
    }
    window.draw(badgeGlyph);

    window.draw(stateText);

    if (game.getGameController().isAIThinking()) {
        const float dots = 0.4f + 0.6f * std::abs(std::sin(elapsed * 3.0f));
        aiThinkingText.setFillColor(sf::Color(120, 224, 255, static_cast<sf::Uint8>(130.0f + 125.0f * dots)));
        window.draw(aiThinkingText);
    }

    window.draw(historyTitleText);

    // Nền sọc xen kẽ cho các dòng sử ký, giúp dễ đọc hơn.
    // Chỉ vẽ nền cho những dòng thực sự có nội dung.
    for (std::size_t i = 0; i < historyTexts.size(); ++i) {
        if (historyTexts[i].getString().isEmpty()) {
            continue;
        }
        if (i % 2 == 0) {
            Theme::drawRoundedRect(window,
                                   sf::FloatRect(innerX, historyTexts[i].getPosition().y - 5.0f,
                                                 innerW, 26.0f),
                                   6.0f, sf::Color(255, 255, 255, 10));
        }
        window.draw(historyTexts[i]);
    }

    btnSave->draw(window);
    btnNewGame->draw(window);
    btnMenu->draw(window);
    window.draw(footerHanText);
    window.draw(footerText);
}

void GameScene::render(sf::RenderWindow& window) {
    // Nền huyền ảo dùng chung cho toàn bộ trò chơi.
    sf::Sprite background(game.getAssetManager().getTexture("scene_bg"));
    window.draw(background);

    drawBoard(window);
    drawHighlights(window);
    drawPieces(window);
    particleSystem.draw(window, game.getAssetManager().getTexture("particle_spark"));
    drawSidebar(window);

    // Khi tướng đang bị chiếu, phủ một lớp đỏ mờ nhấp nháy quanh viền màn hình.
    if (game.getGameController().getState() == GameState::CHECK) {
        const float pulse = 0.5f + 0.5f * std::sin(elapsed * 6.0f);
        Theme::drawGlow(window, sf::Vector2f(WINDOW_WIDTH * 0.5f, BOARD_OFFSET_Y + 4.5f * TILE_SIZE),
                        520.0f, sf::Color(220, 30, 24, static_cast<sf::Uint8>(26.0f + 26.0f * pulse)), 1.0f);
    }
}

} // namespace BachChien
