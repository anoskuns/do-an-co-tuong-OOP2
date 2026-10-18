#include "chess/pieces/Cannon.h"
#include "chess/Board.h"

namespace BachChien {

std::vector<Move> Cannon::getPseudoLegalMoves(const Board& board) const {
    std::vector<Move> moves;
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};

    for (int i = 0; i < 4; ++i) {
        int nr = row + dr[i];
        int nc = col + dc[i];
        bool jumpedScreen = false;

        while (board.isValidCoord(nr, nc)) {
            Piece* target = board.getPiece(nr, nc);

            if (!jumpedScreen) {
                if (!target) {
                    // Nước đi không ăn quân
                    moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, PieceType::NONE, Side::NONE);
                } else {
                    // Gặp quân làm ngòi (screen)
                    jumpedScreen = true;
                }
            } else {
                // Đã có ngòi, tìm quân để ăn
                if (target) {
                    if (target->getSide() != side) {
                        moves.emplace_back(Position{row, col}, Position{nr, nc}, type, side, target->getType(), target->getSide());
                    }
                    break; // Sau khi gặp quân thứ 2 (dù địch hay ta), dừng tia
                }
            }

            nr += dr[i];
            nc += dc[i];
        }
    }

    return moves;
}

std::string Cannon::getCultivationName() const {
    return (side == Side::RED) ? "Thiên Hỏa Linh Pháo" : "Cửu U Âm Pháo";
}

std::string Cannon::getVietnameseName() const {
    return (side == Side::RED) ? "Pháo" : "Pháo";
}

std::string Cannon::getHanTu() const {
    return (side == Side::RED) ? "炮" : "砲";
}

} // namespace BachChien

