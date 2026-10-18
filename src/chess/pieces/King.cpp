#include "chess/pieces/King.h"
#include "chess/Board.h"

namespace BachChien {

std::vector<Move> King::getPseudoLegalMoves(const Board& board) const {
    std::vector<Move> moves;
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};

    int minRow = (side == Side::RED) ? 7 : 0;
    int maxRow = (side == Side::RED) ? 9 : 2;
    int minCol = 3;
    int maxCol = 5;

    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (nr >= minRow && nr <= maxRow && nc >= minCol && nc <= maxCol) {
            Piece* target = board.getPiece(nr, nc);
            if (!target) {
                moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, PieceType::NONE, Side::NONE);
            } else if (target->getSide() != side) {
                moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, target->getType(), target->getSide());
            }
        }
    }

    return moves;
}

std::string King::getCultivationName() const {
    return (side == Side::RED) ? "Huyễn Thiên Tông Chủ" : "Trầm Mộc Đan Tổ";
}

std::string King::getVietnameseName() const {
    return (side == Side::RED) ? "Soái" : "Tướng";
}

std::string King::getHanTu() const {
    return (side == Side::RED) ? "帥" : "將";
}

} // namespace BachChien

