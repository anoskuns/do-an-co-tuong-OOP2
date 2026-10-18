#include "chess/pieces/Pawn.h"
#include "chess/Board.h"

namespace BachChien {

std::vector<Move> Pawn::getPseudoLegalMoves(const Board& board) const {
    std::vector<Move> moves;

    int forwardDir = (side == Side::RED) ? -1 : 1;
    bool hasCrossedRiver = (side == Side::RED) ? (row < 5) : (row > 4);

    // Đi thẳng tiến về phía trước 1 ô
    int forwardRow = row + forwardDir;
    if (board.isValidCoord(forwardRow, col)) {
        Piece* target = board.getPiece(forwardRow, col);
        if (!target) {
            moves.emplace_back(Position{row, col}, Position{forwardRow, col}, type, side, PieceType::NONE, Side::NONE);
        } else if (target->getSide() != side) {
            moves.emplace_back(Position{row, col}, Position{forwardRow, col}, type, side, target->getType(), target->getSide());
        }
    }

    // Đã qua sông thì có thể đi ngang trái/phải 1 ô
    if (hasCrossedRiver) {
        const int dc[2] = {-1, 1};
        for (int i = 0; i < 2; ++i) {
            int nc = col + dc[i];
            if (board.isValidCoord(row, nc)) {
                Piece* target = board.getPiece(row, nc);
                if (!target) {
                    moves.emplace_back(Position{row, col}, Position{row, nc}, type, side, PieceType::NONE, Side::NONE);
                } else if (target->getSide() != side) {
                    moves.emplace_back(Position{row, col}, Position{row, nc}, type, side, target->getType(), target->getSide());
                }
            }
        }
    }

    return moves;
}

std::string Pawn::getCultivationName() const {
    return (side == Side::RED) ? "Thái Hư Linh Tốt" : "Trầm Mộc Hắc Tốt";
}

std::string Pawn::getVietnameseName() const {
    return (side == Side::RED) ? "Binh" : "Tốt";
}

std::string Pawn::getHanTu() const {
    return (side == Side::RED) ? "兵" : "卒";
}

} // namespace BachChien

