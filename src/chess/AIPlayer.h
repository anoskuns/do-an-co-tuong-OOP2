#pragma once

#include "core/Common.h"
#include "chess/Board.h"
#include "chess/Move.h"
#include <random>

namespace BachChien {

class AIPlayer {
private:
    Difficulty difficulty;
    int maxDepth;
    std::mt19937 rng;

    // Đánh giá toàn bộ cục diện bàn cờ cho phe `aiSide`
    int evaluateBoard(const Board& board, Side aiSide) const;

    // Giá trị cơ bản của từng loại quân
    int getBasePieceValue(PieceType type, int r, Side side) const;

    // Điểm thưởng vị trí chiến lược
    int getPositionalBonus(PieceType type, Side side, int r, int c) const;

    // Minimax với cắt tỉa Alpha-Beta
    int minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing, Side aiSide);

    // Sắp xếp thứ tự nước đi (Move Ordering) để tăng tốc độ cắt tỉa Alpha-Beta
    void orderMoves(std::vector<Move>& moves, const Board& board) const;

public:
    AIPlayer(Difficulty diff = Difficulty::MEDIUM);

    void setDifficulty(Difficulty diff);
    Difficulty getDifficulty() const { return difficulty; }

    // Tìm nước đi tốt nhất
    Move getBestMove(Board& board, Side aiSide);
};

} // namespace BachChien

