#pragma once

#include "core/Common.h"

namespace BachChien {

struct Move {
    Position from;
    Position to;
    PieceType movedType = PieceType::NONE;
    Side movedSide = Side::NONE;
    PieceType capturedType = PieceType::NONE;
    Side capturedSide = Side::NONE;
    int score = 0; // Điểm đánh giá dùng sắp xếp nước đi trong AI

    Move() = default;

    Move(Position f, Position t, PieceType mType = PieceType::NONE, Side mSide = Side::NONE,
         PieceType cType = PieceType::NONE, Side cSide = Side::NONE)
        : from(f), to(t), movedType(mType), movedSide(mSide), capturedType(cType), capturedSide(cSide) {}

    bool isCapture() const {
        return capturedType != PieceType::NONE;
    }

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to;
    }
};

} // namespace BachChien

