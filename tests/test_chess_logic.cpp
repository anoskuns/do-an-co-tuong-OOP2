#include "core/Common.h"
#include "chess/Board.h"
#include "chess/AIPlayer.h"
#include <iostream>
#include <cassert>

using namespace BachChien;

void testInitialBoard() {
    Board board;
    // Kiểm tra số lượng quân cờ ban đầu: 32 quân
    int aliveCount = 0;
    for (const auto& p : board.getPieces()) {
        if (p->isAlive()) aliveCount++;
    }
    assert(aliveCount == 32);

    // Kiểm tra vị trí Tướng ban đầu
    assert(board.getKingPosition(Side::RED) == (Position{9, 4}));
    assert(board.getKingPosition(Side::BLACK) == (Position{0, 4}));

    // Ban đầu không ai bị chiếu
    assert(!board.isInCheck(Side::RED));
    assert(!board.isInCheck(Side::BLACK));

    // Hai tướng ban đầu không lộ mặt vì có tốt và chốt chắn giữa
    assert(!board.areKingsFacing());

    std::cout << "[PASS] testInitialBoard\n";
}

void testPawnMovement() {
    Board board;
    // Lấy tốt đỏ tại (6, 4)
    Piece* redPawn = board.getPiece(6, 4);
    assert(redPawn != nullptr);
    assert(redPawn->getType() == PieceType::PAWN);

    auto moves = board.getLegalMovesForPiece(redPawn);
    // Tốt đỏ chưa qua sông chỉ có đúng 1 nước tiến về phía trước: (6,4) -> (5,4)
    assert(moves.size() == 1);
    assert(moves[0].to == (Position{5, 4}));

    std::cout << "[PASS] testPawnMovement\n";
}

void testKnightLegBlocking() {
    Board board;
    // Mã đỏ tại (9, 1). Ban đầu có quân tốt tại (6, 0), (6, 2)
    Piece* knight = board.getPiece(9, 1);
    assert(knight != nullptr);

    auto moves = board.getLegalMovesForPiece(knight);
    // Mã tại (9, 1) có thể nhảy lên (7, 0) và (7, 2) vì chân mã tại (8, 1) trống
    bool canReach70 = false;
    bool canReach72 = false;
    for (const auto& m : moves) {
        if (m.to == (Position{7, 0})) canReach70 = true;
        if (m.to == (Position{7, 2})) canReach72 = true;
    }
    assert(canReach70);
    assert(canReach72);

    std::cout << "[PASS] testKnightLegBlocking\n";
}

void testCannonScreen() {
    Board board;
    // Pháo đỏ tại (7, 1). Ngòi là tốt đen tại (3, 1) hoặc tốt đỏ tại (6, 0)...
    // Bắn thẳng lên cột 1: Pháo (7, 1) có thể ăn Mã đen tại (0, 1) nếu có đúng 1 ngòi giữa đường!
    // Ở cột 1 ban đầu:
    // (7, 1) Pháo đỏ
    // (6, 1) trống
    // (5, 1) trống
    // (4, 1) trống
    // (3, 1) trống (tốt đen ở cột 0, 2, 4, 6, 8)
    // (2, 1) Pháo đen (ngòi)
    // (1, 1) trống
    // (0, 1) Mã đen!
    // -> Giữa (7,1) và (0,1) chỉ có duy nhất (2,1) làm ngòi!
    // Do đó Pháo đỏ (7, 1) CÓ THỂ ăn Mã đen (0, 1)!
    Piece* cannon = board.getPiece(7, 1);
    assert(cannon != nullptr);

    auto moves = board.getLegalMovesForPiece(cannon);
    bool canCaptureKnight = false;
    for (const auto& m : moves) {
        if (m.to == (Position{0, 1}) && m.isCapture()) {
            canCaptureKnight = true;
            break;
        }
    }
    assert(canCaptureKnight);

    std::cout << "[PASS] testCannonScreen\n";
}

void testFlyingGenerals() {
    Board board;
    // Giả lập dọn sạch cột 4 để 2 tướng đối mặt
    Piece* redPawn = board.getPiece(6, 4);
    Piece* blackPawn = board.getPiece(3, 4);
    if (redPawn) redPawn->setAlive(false);
    if (blackPawn) blackPawn->setAlive(false);

    // Di chuyển quân ra khỏi cột 4 trên grid
    // Tự kiểm tra hàm areKingsFacing
    // Tạo bàn cờ mới đơn giản chỉ có 2 tướng
    std::string emptyBoardJson = R"({
        "board": [
            {"type": 1, "side": 1, "row": 9, "col": 4},
            {"type": 1, "side": 2, "row": 0, "col": 4}
        ],
        "history_count": 0
    })";

    Board duelBoard;
    assert(duelBoard.deserializeFromJson(emptyBoardJson));
    assert(duelBoard.areKingsFacing());
    assert(duelBoard.isInCheck(Side::RED));
    assert(duelBoard.isInCheck(Side::BLACK));

    std::cout << "[PASS] testFlyingGenerals\n";
}

void testAIFastMove() {
    Board board;
    AIPlayer ai(Difficulty::MEDIUM);

    // AI phe Đen tìm nước đi phản hồi
    Move bestMove = ai.getBestMove(board, Side::BLACK);
    assert(bestMove.from.isValid());
    assert(bestMove.to.isValid());

    std::cout << "[PASS] testAIFastMove\n";
}

void testSaveLoadJSON() {
    Board board;
    // Đi thử nước đầu tiên: Pháo 2 bình 5 (7, 1) -> (7, 4)
    Move m{Position{7, 1}, Position{7, 4}, PieceType::CANNON, Side::RED};
    board.makeMove(m);

    std::string jsonStr = board.serializeToJson();

    Board loadedBoard;
    bool ok = loadedBoard.deserializeFromJson(jsonStr);
    assert(ok);

    Piece* p = loadedBoard.getPiece(7, 4);
    assert(p != nullptr);
    assert(p->getType() == PieceType::CANNON);
    assert(p->getSide() == Side::RED);
    assert(loadedBoard.getPiece(7, 1) == nullptr);

    std::cout << "[PASS] testSaveLoadJSON\n";
}

int main() {
    std::cout << "--- BẮT ĐẦU KIỂM THỬ TỰ ĐỘNG BÁCH CHIẾN TIÊN CỤC ---\n";
    testInitialBoard();
    testPawnMovement();
    testKnightLegBlocking();
    testCannonScreen();
    testFlyingGenerals();
    testAIFastMove();
    testSaveLoadJSON();
    std::cout << "--- TẤT CẢ TEST CASES ĐỀU THÀNH CÔNG RỰC RỠ! ---\n";
    return 0;
}

