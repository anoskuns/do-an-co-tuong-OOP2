#pragma once

#include "core/Common.h"
#include "chess/Move.h"
#include "chess/pieces/Piece.h"
#include <vector>
#include <memory>
#include <array>
#include <string>

namespace BachChien {

struct MoveRecord {
    Move move;
    std::string notation; // Ký hiệu ván cờ (VD: Xa 1 tiến 2 / Pháo 2 bình 5)
};

class Board {
private:
    std::array<std::array<Piece*, BOARD_COLS>, BOARD_ROWS> grid{};
    std::vector<std::unique_ptr<Piece>> allPieces;
    std::vector<MoveRecord> history;

    Position redKingPos = {9, 4};
    Position blackKingPos = {0, 4};

    void setupInitialPieces();

public:
    Board();
    ~Board() = default;

    // Reset bàn cờ về vị trí ban đầu
    void reset();

    // Truy xuất ô cờ
    bool isValidCoord(int r, int c) const {
        return r >= 0 && r < BOARD_ROWS && c >= 0 && c < BOARD_COLS;
    }
    bool isValidCoord(Position p) const {
        return isValidCoord(p.row, p.col);
    }

    Piece* getPiece(int r, int c) const {
        if (!isValidCoord(r, c)) return nullptr;
        return grid[r][c];
    }
    Piece* getPiece(Position p) const {
        return getPiece(p.row, p.col);
    }

    bool isCellEmpty(int r, int c) const {
        return getPiece(r, c) == nullptr;
    }
    bool isCellEmpty(Position p) const {
        return isCellEmpty(p.row, p.col);
    }

    bool isOpponent(int r, int c, Side mySide) const {
        Piece* p = getPiece(r, c);
        return p != nullptr && p->getSide() != mySide;
    }

    // Đếm số lượng quân trên đường thẳng giữa 2 điểm (không tính 2 điểm đầu mút)
    int countPiecesBetweenStraight(int r1, int c1, int r2, int c2) const;

    // Kiểm tra đường thẳng giữa 2 điểm có hoàn toàn trống không
    bool isStraightClear(int r1, int c1, int r2, int c2) const {
        return countPiecesBetweenStraight(r1, c1, r2, c2) == 0;
    }

    Position getKingPosition(Side side) const {
        return (side == Side::RED) ? redKingPos : blackKingPos;
    }

    // Kiểm tra quy tắc Lộ Mặt Tướng (Flying Generals)
    bool areKingsFacing() const;

    // Kiểm tra bên `side` có đang bị chiếu tướng không
    bool isInCheck(Side side) const;

    // Thực hiện nước đi trên bàn cờ
    void makeMove(const Move& move);

    // Hoàn tác nước đi (chỉ dùng nội bộ cho thuật toán tìm kiếm AI Minimax)
    void undoMoveForAI(const Move& move, Piece* restoredPiece);

    // Lấy tất cả nước đi giả định của một phe
    std::vector<Move> getAllPseudoLegalMoves(Side side) const;

    // Lấy tất cả nước đi hợp lệ tuyệt đối (đã lọc các nước tự đưa tướng vào thế chiếu)
    std::vector<Move> getAllLegalMoves(Side side);

    // Lấy các nước đi hợp lệ cho một quân cờ cụ thể
    std::vector<Move> getLegalMovesForPiece(const Piece* piece);

    // Kiểm tra phe `side` có bị chiếu bí (không còn nước hợp lệ)
    bool isCheckmate(Side side);

    // Danh sách toàn bộ quân cờ đang sống
    const std::vector<std::unique_ptr<Piece>>& getPieces() const {
        return allPieces;
    }

    // Lịch sử nước đi (chỉ dùng hiển thị / lưu trữ)
    const std::vector<MoveRecord>& getHistory() const {
        return history;
    }

    // Tạo ký hiệu nước đi truyền thống
    std::string generateNotation(const Move& move) const;

    // Lưu & Tải JSON
    std::string serializeToJson() const;
    bool deserializeFromJson(const std::string& jsonStr);
};

} // namespace BachChien

