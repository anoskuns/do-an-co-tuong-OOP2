#include "chess/pieces/Rook.h"
#include "chess/Board.h"

namespace BachChien {

std::vector<Move> Rook::getPseudoLegalMoves(const Board& board) const {
    std::vector<Move> moves;
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};

    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];

        while (board.isValidCoord(nr, nc)) {
            Piece* target = board.getPiece(nr, nc);
            if (!target) {
                moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, PieceType::NONE, Side::NONE);
            } else {
                if (target->getSide() != side) {
                    moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, target->getType(), target->getSide());
                }
                break; // Bị chặn đường bởi quân cờ
            }
            nr += dr[i];
            nc += dc[i];
        }
    }

    return moves;
}

std::string Rook::getCultivationName() const {
    return (side == Side::RED) ? "Cửu Tiêu Linh Xa" : "U Hồn Hắc Xa";
}

std::string Rook::getVietnameseName() const {
    return (side == Side::RED) ? "Xe" : "Xe";
}

std::string Rook::getHanTu() const {
    return (side == Side::RED) ? "俥" : "車";
}

} // namespace BachChien

