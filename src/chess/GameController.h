#pragma once

#include "core/Common.h"
#include "chess/Board.h"
#include "chess/AIPlayer.h"
#include <functional>

namespace BachChien {

class GameController {
private:
    Board board;
    std::unique_ptr<AIPlayer> aiPlayer;

    GameMode mode = GameMode::PVE;
    Side playerSide = Side::RED; // Phe của người chơi khi chơi PvE
    Side currentTurn = Side::RED;
    GameState state = GameState::PLAYING;
    Difficulty difficulty = Difficulty::MEDIUM;

    Piece* selectedPiece = nullptr;
    std::vector<Move> legalMovesForSelected;
    Move lastMove;

    // AI timer & delay
    bool aiThinking = false;
    float aiTimer = 0.0f;
    const float AI_DELAY = 0.4f; // Trì hoãn 0.4s để tạo cảm giác tự nhiên

    // Callbacks thông báo sự kiện cho Scene / Sound / Effects
    std::function<void(const Move&)> onMoveMade;
    std::function<void(Position)> onPieceCaptured;
    std::function<void(Side)> onCheck;
    std::function<void(Side)> onGameOver;

public:
    GameController();

    void startNewGame(GameMode m, Side pSide, Difficulty diff);

    // Xử lý click chuột vào tọa độ hàng, cột trên bàn cờ
    bool handleSquareClick(int row, int col);

    // Cập nhật AI và trạng thái game
    void update(float dt);

    // Đăng ký callback sự kiện
    void setOnMoveMadeCallback(std::function<void(const Move&)> cb) { onMoveMade = cb; }
    void setOnPieceCapturedCallback(std::function<void(Position)> cb) { onPieceCaptured = cb; }
    void setOnCheckCallback(std::function<void(Side)> cb) { onCheck = cb; }
    void setOnGameOverCallback(std::function<void(Side)> cb) { onGameOver = cb; }

    // Getters
    Board& getBoard() { return board; }
    const Board& getBoard() const { return board; }
    Side getCurrentTurn() const { return currentTurn; }
    GameState getState() const { return state; }
    GameMode getMode() const { return mode; }
    Side getPlayerSide() const { return playerSide; }
    Difficulty getDifficulty() const { return difficulty; }
    const Piece* getSelectedPiece() const { return selectedPiece; }
    const std::vector<Move>& getLegalMovesForSelected() const { return legalMovesForSelected; }
    const Move& getLastMove() const { return lastMove; }
    bool isAIThinking() const { return aiThinking; }

    // Lưu & Tải 3 slot JSON
    bool saveGame(int slot);
    bool loadGame(int slot);
};

} // namespace BachChien

