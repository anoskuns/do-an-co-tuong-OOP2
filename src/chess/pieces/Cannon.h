#pragma once

#include "chess/pieces/Piece.h"

namespace BachChien {

class Cannon : public Piece {
public:
    Cannon(int r, int c, Side s) : Piece(r, c, s, PieceType::CANNON) {}

    std::vector<Move> getPseudoLegalMoves(const Board& board) const override;
    std::string getCultivationName() const override;
    std::string getVietnameseName() const override;
    std::string getHanTu() const override;
};

} // namespace BachChien

