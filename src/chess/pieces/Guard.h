#pragma once

#include "chess/pieces/Piece.h"

namespace BachChien {

class Guard : public Piece {
public:
    Guard(int r, int c, Side s) : Piece(r, c, s, PieceType::GUARD) {}

    std::vector<Move> getPseudoLegalMoves(const Board& board) const override;
    std::string getCultivationName() const override;
    std::string getVietnameseName() const override;
    std::string getHanTu() const override;
};

} // namespace BachChien

