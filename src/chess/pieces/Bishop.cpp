#include "chess/pieces/Bishop.h"
#include "chess/Board.h"

namespace BachChien {

std::vector<Move> Bishop::getPseudoLegalMoves(const Board& board) const {
    std::vector<Move> moves;
    const int dr[4] = {-2, -2, 2, 2};
    const int dc[4] = {-2, 2, -2, 2};

    int minRow = (side == Side::RED) ? 5 : 0;
    int maxRow = (side == Side::RED) ? 9 : 4;

    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];

        // Kiểm tra trong biên và không qua sông
        if (nr >= minRow && nr <= maxRow && nc >= 0 && nc < BOARD_COLS) {
            // Kiểm tra mắt voi
            int eyeR = (row + nr) / 2;
            int eyeC = (col + nc) / 2;

            if (board.isCellEmpty(eyeR, eyeC)) {
                Piece* target = board.getPiece(nr, nc);
                if (!target) {
                    moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, PieceType::NONE, Side::NONE);
                } else if (target->getSide() != side) {
                    moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, target->getType(), target->getSide());
                }
            }
        }
    }

    return moves;
}

std::string Bishop::getCultivationName() const {
    return (side == Side::RED) ? "Đại Tượng Linh Thú" : "U Minh Hắc Tượng";
}

std::string Bishop::getVietnameseName() const {
    return (side == Side::RED) ? "Tượng" : "Tượng";
}

std::string Bishop::getHanTu() const {
    return (side == Side::RED) ? "相" : "象";
}

} // namespace BachChien

