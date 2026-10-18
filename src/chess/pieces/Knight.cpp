#include "chess/pieces/Knight.h"
#include "chess/Board.h"

namespace BachChien {

struct KnightStep {
    int dr, dc;
    int legDr, legDc;
};

std::vector<Move> Knight::getPseudoLegalMoves(const Board& board) const {
    std::vector<Move> moves;

    const KnightStep steps[8] = {
        {-2, -1, -1,  0},
        {-2,  1, -1,  0},
        { 2, -1,  1,  0},
        { 2,  1,  1,  0},
        {-1, -2,  0, -1},
        { 1, -2,  0, -1},
        {-1,  2,  0,  1},
        { 1,  2,  0,  1}
    };

    for (const auto& step : steps) {
        int nr = row + step.dr;
        int nc = col + step.dc;

        if (board.isValidCoord(nr, nc)) {
            int legR = row + step.legDr;
            int legC = col + step.legDc;

            // Kiểm tra cản chân mã
            if (board.isCellEmpty(legR, legC)) {
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

std::string Knight::getCultivationName() const {
    return (side == Side::RED) ? "Ngự Phong Thiên Mã" : "Huyết Sát Quỷ Mã";
}

std::string Knight::getVietnameseName() const {
    return (side == Side::RED) ? "Mã" : "Mã";
}

std::string Knight::getHanTu() const {
    return (side == Side::RED) ? "傌" : "馬";
}

} // namespace BachChien

