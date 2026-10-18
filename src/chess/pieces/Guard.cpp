#include "chess/pieces/Guard.h"
#include "chess/Board.h"

namespace BachChien {

std::vector<Move> Guard::getPseudoLegalMoves(const Board& board) const {
    std::vector<Move> moves;
    const int dr[4] = {-1, -1, 1, 1};
    const int dc[4] = {-1, 1, -1, 1};

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

std::string Guard::getCultivationName() const {
    return (side == Side::RED) ? "Hộ Pháp Chân Nhân" : "Hắc Vệ Ma Quân";
}

std::string Guard::getVietnameseName() const {
    return (side == Side::RED) ? "Sĩ" : "Sĩ";
}

std::string Guard::getHanTu() const {
    return (side == Side::RED) ? "仕" : "士";
}

} // namespace BachChien

