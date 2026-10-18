#include "chess/AIPlayer.h"
#include <climits>
#include <algorithm>
#include <chrono>

namespace BachChien {

AIPlayer::AIPlayer(Difficulty diff) : difficulty(diff) {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
    setDifficulty(diff);
}

void AIPlayer::setDifficulty(Difficulty diff) {
    difficulty = diff;
    switch (diff) {
        case Difficulty::EASY:
            maxDepth = 1;
            break;
        case Difficulty::MEDIUM:
            maxDepth = 3;
            break;
        case Difficulty::HARD:
            maxDepth = 4; // Depth 4 với cắt tỉa Alpha-Beta và Move Ordering phản hồi cực nhanh ~0.5s
            break;
    }
}

int AIPlayer::getBasePieceValue(PieceType type, int r, Side side) const {
    switch (type) {
        case PieceType::KING:   return 10000;
        case PieceType::ROOK:   return 1000;
        case PieceType::CANNON: return 450;
        case PieceType::KNIGHT: return 400;
        case PieceType::BISHOP: return 200;
        case PieceType::GUARD:  return 200;
        case PieceType::PAWN: {
            bool hasCrossed = (side == Side::RED) ? (r < 5) : (r > 4);
            return hasCrossed ? 200 : 100;
        }
        default: return 0;
    }
}

int AIPlayer::getPositionalBonus(PieceType type, Side side, int r, int c) const {
    // Chuẩn hóa hàng theo góc nhìn của phe (0 là hàng nhà, 9 là hàng địch)
    int rowFromBase = (side == Side::RED) ? (9 - r) : r;

    int bonus = 0;
    switch (type) {
        case PieceType::KNIGHT:
            // Mã thích kiểm soát trung lộ (cột 2..6, hàng 2..7)
            if (c >= 2 && c <= 6) bonus += 20;
            if (rowFromBase >= 2 && rowFromBase <= 7) bonus += 20;
            // Phạt mã ở biên
            if (c == 0 || c == 8) bonus -= 30;
            break;

        case PieceType::ROOK:
            // Xe thích chiếm hàng ngang đối phương và các cột thoáng trung tâm
            if (rowFromBase >= 6) bonus += 30;
            if (c == 4) bonus += 20;
            break;

        case PieceType::CANNON:
            // Pháo trấn giữ trung lộ
            if (c == 4) bonus += 30;
            break;

        case PieceType::PAWN:
            // Tốt càng tiến sâu vào cung đối phương càng nguy hiểm
            if (rowFromBase >= 5) {
                bonus += 50 + (rowFromBase - 5) * 20;
                if (c >= 3 && c <= 5) bonus += 30; // Đâm thẳng vào cung
            }
            break;

        default:
            break;
    }
    return bonus;
}

int AIPlayer::evaluateBoard(const Board& board, Side aiSide) const {
    int score = 0;
    Side opponent = getOpponentSide(aiSide);

    for (const auto& piece : board.getPieces()) {
        if (!piece->isAlive()) continue;

        int val = getBasePieceValue(piece->getType(), piece->getRow(), piece->getSide())
                + getPositionalBonus(piece->getType(), piece->getSide(), piece->getRow(), piece->getCol());

        if (piece->getSide() == aiSide) {
            score += val;
        } else {
            score -= val;
        }
    }

    return score;
}

void AIPlayer::orderMoves(std::vector<Move>& moves, const Board& board) const {
    // Sắp xếp nước đi: Nước ăn quân giá trị cao bằng quân giá trị thấp (MVV-LVA) lên trước
    for (auto& m : moves) {
        if (m.isCapture()) {
            int victimVal = getBasePieceValue(m.capturedType, m.to.row, m.capturedSide);
            int attackerVal = getBasePieceValue(m.movedType, m.from.row, m.movedSide);
            m.score = victimVal * 10 - attackerVal; // Ưu tiên cao
        } else {
            m.score = 0;
        }
    }

    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}

int AIPlayer::minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing, Side aiSide) {
    Side currentSide = isMaximizing ? aiSide : getOpponentSide(aiSide);

    if (depth == 0) {
        return evaluateBoard(board, aiSide);
    }

    auto legalMoves = board.getAllLegalMoves(currentSide);
    if (legalMoves.empty()) {
        // Không còn nước đi: nếu bị chiếu thì thua cực nặng, nếu không bị chiếu thì cũng tính là thua
        return isMaximizing ? -99999 + (maxDepth - depth) : 99999 - (maxDepth - depth);
    }

    orderMoves(legalMoves, board);

    if (isMaximizing) {
        int maxEval = -1000000;
        for (const auto& m : legalMoves) {
            Piece* target = board.getPiece(m.to.row, m.to.col);
            board.makeMove(m);

            int eval = minimax(board, depth - 1, alpha, beta, false, aiSide);

            board.undoMoveForAI(m, target);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Cắt tỉa nhánh
        }
        return maxEval;
    } else {
        int minEval = 1000000;
        for (const auto& m : legalMoves) {
            Piece* target = board.getPiece(m.to.row, m.to.col);
            board.makeMove(m);

            int eval = minimax(board, depth - 1, alpha, beta, true, aiSide);

            board.undoMoveForAI(m, target);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Cắt tỉa nhánh
        }
        return minEval;
    }
}

Move AIPlayer::getBestMove(Board& board, Side aiSide) {
    auto legalMoves = board.getAllLegalMoves(aiSide);
    if (legalMoves.empty()) return Move{};

    orderMoves(legalMoves, board);

    struct ScoredMove {
        Move move;
        int score;
    };
    std::vector<ScoredMove> scoredMoves;

    int bestVal = -1000000;
    int alpha = -1000000;
    int beta = 1000000;

    for (const auto& m : legalMoves) {
        Piece* target = board.getPiece(m.to.row, m.to.col);
        board.makeMove(m);

        int moveScore = minimax(board, maxDepth - 1, alpha, beta, false, aiSide);

        board.undoMoveForAI(m, target);

        scoredMoves.push_back({m, moveScore});

        if (moveScore > bestVal) {
            bestVal = moveScore;
        }
        alpha = std::max(alpha, bestVal);
    }

    // Sắp xếp các nước theo điểm số giảm dần
    std::sort(scoredMoves.begin(), scoredMoves.end(), [](const ScoredMove& a, const ScoredMove& b) {
        return a.score > b.score;
    });

    if (difficulty == Difficulty::EASY) {
        // Cảnh giới Luyện Khí: Chọn ngẫu nhiên trong top 3 nước tốt nhất
        int candidates = std::min(static_cast<int>(scoredMoves.size()), 3);
        std::uniform_int_distribution<int> dist(0, candidates - 1);
        return scoredMoves[dist(rng)].move;
    }

    // Cảnh giới Trúc Cơ & Kim Đan: Chọn nước đi tối ưu nhất
    return scoredMoves.front().move;
}

} // namespace BachChien

