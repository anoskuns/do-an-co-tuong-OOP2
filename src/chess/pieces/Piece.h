#pragma once

#include "core/Common.h"
#include "chess/Move.h"
#include <vector>
#include <string>

namespace BachChien {

class Board;

class Piece {
protected:
    int row;
    int col;
    Side side;
    PieceType type;
    bool alive = true;

public:
    Piece(int r, int c, Side s, PieceType t)
        : row(r), col(c), side(s), type(t), alive(true) {}

    virtual ~Piece() = default;

    int getRow() const { return row; }
    int getCol() const { return col; }
    Position getPos() const { return {row, col}; }
    Side getSide() const { return side; }
    PieceType getType() const { return type; }
    bool isAlive() const { return alive; }

    void setPosition(int r, int c) {
        row = r;
        col = c;
    }
    void setPosition(Position pos) {
        row = pos.row;
        col = pos.col;
    }
    void setAlive(bool a) {
        alive = a;
    }

    // Nước đi giả định (chưa lọc tự chiếu tướng)
    virtual std::vector<Move> getPseudoLegalMoves(const Board& board) const = 0;

    // Tên theo chủ đề Tu Tiên (Thái Hư Huyễn Phái / Trầm Mộc Đan Tông)
    virtual std::string getCultivationName() const = 0;

    // Tên quân cờ thuần Việt (Tướng, Sĩ, Tượng, Xe, Mã, Pháo, Tốt)
    virtual std::string getVietnameseName() const = 0;

    // Chữ Hán truyền thống (帥, 將, 仕, 士, 相, 象, 車, 俥, 馬, 傌, 炮, 砲, 兵, 卒)
    virtual std::string getHanTu() const = 0;
};

} // namespace BachChien

