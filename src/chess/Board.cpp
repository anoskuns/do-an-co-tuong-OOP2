#include "chess/Board.h"
#include "chess/pieces/King.h"
#include "chess/pieces/Guard.h"
#include "chess/pieces/Bishop.h"
#include "chess/pieces/Knight.h"
#include "chess/pieces/Rook.h"
#include "chess/pieces/Cannon.h"
#include "chess/pieces/Pawn.h"
#include "third_party/nlohmann/json.hpp"

#include <cmath>
#include <algorithm>
#include <sstream>

namespace BachChien {

Board::Board() {
    reset();
}

void Board::reset() {
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            grid[r][c] = nullptr;
        }
    }
    allPieces.clear();
    history.clear();

    setupInitialPieces();
}

void Board::setupInitialPieces() {
    auto addPiece = [this](std::unique_ptr<Piece> p) {
        grid[p->getRow()][p->getCol()] = p.get();
        allPieces.push_back(std::move(p));
    };

    // --- Phe Đỏ (Thái Hư Huyễn Phái) ---
    // Hàng 9
    addPiece(std::make_unique<Rook>(9, 0, Side::RED));
    addPiece(std::make_unique<Knight>(9, 1, Side::RED));
    addPiece(std::make_unique<Bishop>(9, 2, Side::RED));
    addPiece(std::make_unique<Guard>(9, 3, Side::RED));
    addPiece(std::make_unique<King>(9, 4, Side::RED));
    addPiece(std::make_unique<Guard>(9, 5, Side::RED));
    addPiece(std::make_unique<Bishop>(9, 6, Side::RED));
    addPiece(std::make_unique<Knight>(9, 7, Side::RED));
    addPiece(std::make_unique<Rook>(9, 8, Side::RED));
    redKingPos = {9, 4};

    // Hàng 7: Pháo
    addPiece(std::make_unique<Cannon>(7, 1, Side::RED));
    addPiece(std::make_unique<Cannon>(7, 7, Side::RED));

    // Hàng 6: Binh
    addPiece(std::make_unique<Pawn>(6, 0, Side::RED));
    addPiece(std::make_unique<Pawn>(6, 2, Side::RED));
    addPiece(std::make_unique<Pawn>(6, 4, Side::RED));
    addPiece(std::make_unique<Pawn>(6, 6, Side::RED));
    addPiece(std::make_unique<Pawn>(6, 8, Side::RED));

    // --- Phe Đen (Trầm Mộc Đan Tông) ---
    // Hàng 0
    addPiece(std::make_unique<Rook>(0, 0, Side::BLACK));
    addPiece(std::make_unique<Knight>(0, 1, Side::BLACK));
    addPiece(std::make_unique<Bishop>(0, 2, Side::BLACK));
    addPiece(std::make_unique<Guard>(0, 3, Side::BLACK));
    addPiece(std::make_unique<King>(0, 4, Side::BLACK));
    addPiece(std::make_unique<Guard>(0, 5, Side::BLACK));
    addPiece(std::make_unique<Bishop>(0, 6, Side::BLACK));
    addPiece(std::make_unique<Knight>(0, 7, Side::BLACK));
    addPiece(std::make_unique<Rook>(0, 8, Side::BLACK));
    blackKingPos = {0, 4};

    // Hàng 2: Pháo
    addPiece(std::make_unique<Cannon>(2, 1, Side::BLACK));
    addPiece(std::make_unique<Cannon>(2, 7, Side::BLACK));

    // Hàng 3: Tốt
    addPiece(std::make_unique<Pawn>(3, 0, Side::BLACK));
    addPiece(std::make_unique<Pawn>(3, 2, Side::BLACK));
    addPiece(std::make_unique<Pawn>(3, 4, Side::BLACK));
    addPiece(std::make_unique<Pawn>(3, 6, Side::BLACK));
    addPiece(std::make_unique<Pawn>(3, 8, Side::BLACK));
}

int Board::countPiecesBetweenStraight(int r1, int c1, int r2, int c2) const {
    if (r1 != r2 && c1 != c2) return -1; // Không phải đường thẳng

    int count = 0;
    if (r1 == r2) {
        int start = std::min(c1, c2) + 1;
        int end = std::max(c1, c2);
        for (int c = start; c < end; ++c) {
            if (grid[r1][c] != nullptr) count++;
        }
    } else {
        int start = std::min(r1, r2) + 1;
        int end = std::max(r1, r2);
        for (int r = start; r < end; ++r) {
            if (grid[r][c1] != nullptr) count++;
        }
    }
    return count;
}

bool Board::areKingsFacing() const {
    if (redKingPos.col != blackKingPos.col) return false;
    return countPiecesBetweenStraight(redKingPos.row, redKingPos.col, blackKingPos.row, blackKingPos.col) == 0;
}

bool Board::isInCheck(Side side) const {
    Position kingPos = getKingPosition(side);
    Side opponent = getOpponentSide(side);

    // 1. Kiểm tra luật lộ mặt tướng
    if (areKingsFacing()) return true;

    // 2. Kiểm tra có quân đối phương nào đang tấn công vào Tướng không
    for (const auto& piece : allPieces) {
        if (piece->isAlive() && piece->getSide() == opponent) {
            auto moves = piece->getPseudoLegalMoves(*this);
            for (const auto& m : moves) {
                if (m.to == kingPos) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Board::makeMove(const Move& move) {
    Piece* movingPiece = grid[move.from.row][move.from.col];
    if (!movingPiece) return;

    Piece* targetPiece = grid[move.to.row][move.to.col];
    if (targetPiece) {
        targetPiece->setAlive(false);
    }

    grid[move.from.row][move.from.col] = nullptr;
    grid[move.to.row][move.to.col] = movingPiece;
    movingPiece->setPosition(move.to);

    if (movingPiece->getType() == PieceType::KING) {
        if (movingPiece->getSide() == Side::RED) redKingPos = move.to;
        else blackKingPos = move.to;
    }

    std::string notation = generateNotation(move);
    history.push_back({move, notation});
}

void Board::undoMoveForAI(const Move& move, Piece* restoredPiece) {
    Piece* movingPiece = grid[move.to.row][move.to.col];
    if (!movingPiece) return;

    grid[move.to.row][move.to.col] = restoredPiece;
    if (restoredPiece) {
        restoredPiece->setAlive(true);
        restoredPiece->setPosition(move.to);
    }

    grid[move.from.row][move.from.col] = movingPiece;
    movingPiece->setPosition(move.from);

    if (movingPiece->getType() == PieceType::KING) {
        if (movingPiece->getSide() == Side::RED) redKingPos = move.from;
        else blackKingPos = move.from;
    }
}

std::vector<Move> Board::getAllPseudoLegalMoves(Side side) const {
    std::vector<Move> moves;
    for (const auto& p : allPieces) {
        if (p->isAlive() && p->getSide() == side) {
            auto pieceMoves = p->getPseudoLegalMoves(*this);
            moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        }
    }
    return moves;
}

std::vector<Move> Board::getAllLegalMoves(Side side) {
    std::vector<Move> legalMoves;
    auto pseudoMoves = getAllPseudoLegalMoves(side);

    for (const auto& m : pseudoMoves) {
        Piece* movingPiece = grid[m.from.row][m.from.col];
        Piece* targetPiece = grid[m.to.row][m.to.col];

        // Giả lập nước đi
        if (targetPiece) targetPiece->setAlive(false);
        grid[m.from.row][m.from.col] = nullptr;
        grid[m.to.row][m.to.col] = movingPiece;
        movingPiece->setPosition(m.to);

        Position oldKingPos = (movingPiece->getType() == PieceType::KING) ?
            ((side == Side::RED) ? redKingPos : blackKingPos) : Position{-1, -1};
        if (movingPiece->getType() == PieceType::KING) {
            if (side == Side::RED) redKingPos = m.to;
            else blackKingPos = m.to;
        }

        bool inCheck = isInCheck(side);

        // Phục hồi bàn cờ
        grid[m.to.row][m.to.col] = targetPiece;
        if (targetPiece) targetPiece->setAlive(true);
        grid[m.from.row][m.from.col] = movingPiece;
        movingPiece->setPosition(m.from);

        if (movingPiece->getType() == PieceType::KING) {
            if (side == Side::RED) redKingPos = oldKingPos;
            else blackKingPos = oldKingPos;
        }

        if (!inCheck) {
            legalMoves.push_back(m);
        }
    }

    return legalMoves;
}

std::vector<Move> Board::getLegalMovesForPiece(const Piece* piece) {
    std::vector<Move> legalMoves;
    if (!piece || !piece->isAlive()) return legalMoves;

    Side side = piece->getSide();
    auto pseudoMoves = piece->getPseudoLegalMoves(*this);

    for (const auto& m : pseudoMoves) {
        Piece* movingPiece = grid[m.from.row][m.from.col];
        Piece* targetPiece = grid[m.to.row][m.to.col];

        if (targetPiece) targetPiece->setAlive(false);
        grid[m.from.row][m.from.col] = nullptr;
        grid[m.to.row][m.to.col] = movingPiece;
        movingPiece->setPosition(m.to);

        Position oldKingPos = (movingPiece->getType() == PieceType::KING) ?
            ((side == Side::RED) ? redKingPos : blackKingPos) : Position{-1, -1};
        if (movingPiece->getType() == PieceType::KING) {
            if (side == Side::RED) redKingPos = m.to;
            else blackKingPos = m.to;
        }

        bool inCheck = isInCheck(side);

        grid[m.to.row][m.to.col] = targetPiece;
        if (targetPiece) targetPiece->setAlive(true);
        grid[m.from.row][m.from.col] = movingPiece;
        movingPiece->setPosition(m.from);

        if (movingPiece->getType() == PieceType::KING) {
            if (side == Side::RED) redKingPos = oldKingPos;
            else blackKingPos = oldKingPos;
        }

        if (!inCheck) {
            legalMoves.push_back(m);
        }
    }

    return legalMoves;
}

bool Board::isCheckmate(Side side) {
    return getAllLegalMoves(side).empty();
}

std::string Board::generateNotation(const Move& move) const {
    Piece* p = grid[move.to.row][move.to.col];
    std::string pieceName = p ? p->getVietnameseName() : "Quân";
    std::string sideStr = (move.movedSide == Side::RED) ? "[Thái Hư]" : "[Trầm Mộc]";

    std::stringstream ss;
    ss << sideStr << " " << pieceName << " (" << move.from.row << "," << move.from.col << ") -> ("
       << move.to.row << "," << move.to.col << ")";
    if (move.isCapture()) {
        ss << " [Trảm!]";
    }
    return ss.str();
}

std::string Board::serializeToJson() const {
    nlohmann::json j;
    j["board"] = nlohmann::json::array();

    for (const auto& p : allPieces) {
        if (p->isAlive()) {
            nlohmann::json pieceJson;
            pieceJson["type"] = static_cast<int>(p->getType());
            pieceJson["side"] = static_cast<int>(p->getSide());
            pieceJson["row"] = p->getRow();
            pieceJson["col"] = p->getCol();
            j["board"].push_back(pieceJson);
        }
    }

    j["history_count"] = history.size();
    return j.dump(4);
}

bool Board::deserializeFromJson(const std::string& jsonStr) {
    try {
        auto j = nlohmann::json::parse(jsonStr);
        if (!j.contains("board") || !j["board"].is_array()) return false;

        for (int r = 0; r < BOARD_ROWS; ++r) {
            for (int c = 0; c < BOARD_COLS; ++c) {
                grid[r][c] = nullptr;
            }
        }
        allPieces.clear();
        history.clear();

        for (const auto& item : j["board"]) {
            int typeInt = item["type"];
            int sideInt = item["side"];
            int r = item["row"];
            int c = item["col"];

            PieceType type = static_cast<PieceType>(typeInt);
            Side side = static_cast<Side>(sideInt);

            std::unique_ptr<Piece> newPiece = nullptr;
            switch (type) {
                case PieceType::KING:
                    newPiece = std::make_unique<King>(r, c, side);
                    if (side == Side::RED) redKingPos = {r, c};
                    else blackKingPos = {r, c};
                    break;
                case PieceType::GUARD:
                    newPiece = std::make_unique<Guard>(r, c, side);
                    break;
                case PieceType::BISHOP:
                    newPiece = std::make_unique<Bishop>(r, c, side);
                    break;
                case PieceType::KNIGHT:
                    newPiece = std::make_unique<Knight>(r, c, side);
                    break;
                case PieceType::ROOK:
                    newPiece = std::make_unique<Rook>(r, c, side);
                    break;
                case PieceType::CANNON:
                    newPiece = std::make_unique<Cannon>(r, c, side);
                    break;
                case PieceType::PAWN:
                    newPiece = std::make_unique<Pawn>(r, c, side);
                    break;
                default:
                    break;
            }

            if (newPiece) {
                grid[r][c] = newPiece.get();
                allPieces.push_back(std::move(newPiece));
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace BachChien

