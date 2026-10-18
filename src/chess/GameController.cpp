#include "chess/GameController.h"
#include "core/AssetPaths.h"
#include "third_party/nlohmann/json.hpp"
#include <fstream>
#include <sstream>

namespace BachChien {

GameController::GameController() {
    aiPlayer = std::make_unique<AIPlayer>(Difficulty::MEDIUM);
    startNewGame(GameMode::PVE, Side::RED, Difficulty::MEDIUM);
}

void GameController::startNewGame(GameMode m, Side pSide, Difficulty diff) {
    mode = m;
    playerSide = pSide;
    difficulty = diff;
    currentTurn = Side::RED; // Cờ tướng truyền thống Đỏ đi trước
    state = GameState::PLAYING;

    selectedPiece = nullptr;
    legalMovesForSelected.clear();
    lastMove = Move{};
    aiThinking = false;
    aiTimer = 0.0f;

    board.reset();
    aiPlayer->setDifficulty(diff);

    // Nếu người chơi chọn cầm quân Đen trong PvE thì AI (Đỏ) sẽ đi trước
    if (mode == GameMode::PVE && playerSide == Side::BLACK) {
        aiThinking = true;
        aiTimer = 0.0f;
    }
}

bool GameController::handleSquareClick(int row, int col) {
    if (state == GameState::RED_WON || state == GameState::BLACK_WON || state == GameState::DRAW) {
        return false;
    }

    if (mode == GameMode::PVE && currentTurn != playerSide) {
        return false; // Đang lượt của AI
    }

    if (aiThinking) return false;

    if (!board.isValidCoord(row, col)) {
        selectedPiece = nullptr;
        legalMovesForSelected.clear();
        return false;
    }

    Piece* clickedPiece = board.getPiece(row, col);

    // 1. Nếu chưa chọn quân cờ nào
    if (!selectedPiece) {
        if (clickedPiece && clickedPiece->getSide() == currentTurn) {
            selectedPiece = clickedPiece;
            legalMovesForSelected = board.getLegalMovesForPiece(selectedPiece);
            return true;
        }
        return false;
    }

    // 2. Đã có quân cờ được chọn -> Kiểm tra xem ô được click có phải là nước đi hợp lệ không
    for (const auto& m : legalMovesForSelected) {
        if (m.to.row == row && m.to.col == col) {
            bool isCapture = m.isCapture();
            Position capturePos = m.to;

            board.makeMove(m);
            lastMove = m;

            if (onMoveMade) onMoveMade(m);
            if (isCapture && onPieceCaptured) onPieceCaptured(capturePos);

            selectedPiece = nullptr;
            legalMovesForSelected.clear();

            currentTurn = getOpponentSide(currentTurn);

            // Kiểm tra trạng thái ván cờ cho phe kế tiếp
            if (board.isCheckmate(currentTurn)) {
                state = (currentTurn == Side::RED) ? GameState::BLACK_WON : GameState::RED_WON;
                if (onGameOver) onGameOver(currentTurn);
            } else if (board.isInCheck(currentTurn)) {
                state = GameState::CHECK;
                if (onCheck) onCheck(currentTurn);
            } else {
                state = GameState::PLAYING;
            }

            // Kích hoạt AI nếu tới lượt máy
            if (mode == GameMode::PVE && currentTurn != playerSide &&
                state != GameState::RED_WON && state != GameState::BLACK_WON) {
                aiThinking = true;
                aiTimer = 0.0f;
            }

            return true;
        }
    }

    // Nếu không phải nước đi nhưng click vào một quân cùng phe khác -> đổi quân chọn
    if (clickedPiece && clickedPiece->getSide() == currentTurn) {
        selectedPiece = clickedPiece;
        legalMovesForSelected = board.getLegalMovesForPiece(selectedPiece);
        return true;
    }

    // Bỏ chọn nếu click ra ngoài ô hợp lệ
    selectedPiece = nullptr;
    legalMovesForSelected.clear();
    return false;
}

void GameController::update(float dt) {
    if (aiThinking && state != GameState::RED_WON && state != GameState::BLACK_WON) {
        aiTimer += dt;
        if (aiTimer >= AI_DELAY) {
            aiThinking = false;
            Move bestMove = aiPlayer->getBestMove(board, currentTurn);

            if (bestMove.from.isValid() && bestMove.to.isValid()) {
                bool isCapture = bestMove.isCapture();
                Position capturePos = bestMove.to;

                board.makeMove(bestMove);
                lastMove = bestMove;

                if (onMoveMade) onMoveMade(bestMove);
                if (isCapture && onPieceCaptured) onPieceCaptured(capturePos);

                currentTurn = getOpponentSide(currentTurn);

                if (board.isCheckmate(currentTurn)) {
                    state = (currentTurn == Side::RED) ? GameState::BLACK_WON : GameState::RED_WON;
                    if (onGameOver) onGameOver(currentTurn);
                } else if (board.isInCheck(currentTurn)) {
                    state = GameState::CHECK;
                    if (onCheck) onCheck(currentTurn);
                } else {
                    state = GameState::PLAYING;
                }
            } else {
                // Không tìm thấy nước đi -> đầu hàng
                state = (currentTurn == Side::RED) ? GameState::BLACK_WON : GameState::RED_WON;
                if (onGameOver) onGameOver(currentTurn);
            }
        }
    }
}

bool GameController::saveGame(int slot) {
    try {
        std::string filename = assetPath("saves/save_" + std::to_string(slot) + ".json");
        nlohmann::json j;
        j["game_title"] = "Bach Chien Tien Cuc";
        j["version"] = "1.0";
        j["mode"] = static_cast<int>(mode);
        j["player_side"] = static_cast<int>(playerSide);
        j["current_turn"] = static_cast<int>(currentTurn);
        j["difficulty"] = static_cast<int>(difficulty);

        auto boardJson = nlohmann::json::parse(board.serializeToJson());
        j["board_state"] = boardJson;

        std::ofstream file(filename);
        if (!file.is_open()) return false;
        file << j.dump(4);
        return true;
    } catch (...) {
        return false;
    }
}

bool GameController::loadGame(int slot) {
    try {
        std::string filename = assetPath("saves/save_" + std::to_string(slot) + ".json");
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        nlohmann::json j;
        file >> j;

        mode = static_cast<GameMode>(j["mode"].get<int>());
        playerSide = static_cast<Side>(j["player_side"].get<int>());
        currentTurn = static_cast<Side>(j["current_turn"].get<int>());
        difficulty = static_cast<Difficulty>(j["difficulty"].get<int>());

        aiPlayer->setDifficulty(difficulty);
        selectedPiece = nullptr;
        legalMovesForSelected.clear();
        state = GameState::PLAYING;
        aiThinking = false;

        bool success = board.deserializeFromJson(j["board_state"].dump());
        if (success) {
            if (board.isCheckmate(currentTurn)) {
                state = (currentTurn == Side::RED) ? GameState::BLACK_WON : GameState::RED_WON;
            } else if (board.isInCheck(currentTurn)) {
                state = GameState::CHECK;
            }
        }
        return success;
    } catch (...) {
        return false;
    }
}

} // namespace BachChien

