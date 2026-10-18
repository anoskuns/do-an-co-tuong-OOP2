#pragma once

#include "core/AssetPaths.h"

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <iostream>

namespace BachChien {

constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

// Kích thước bàn cờ chuẩn Cờ Tướng: 10 hàng x 9 cột
constexpr int BOARD_ROWS = 10;
constexpr int BOARD_COLS = 9;

// Kích thước ô cờ và căn lề hiển thị bàn cờ trên màn hình 1280x720
constexpr float TILE_SIZE = 62.0f;
constexpr float BOARD_OFFSET_X = 117.0f; // Tâm ô (0,0) — bàn cờ nằm cân đối ở nửa trái
constexpr float BOARD_OFFSET_Y = 81.0f;
// Bề rộng khung gỗ bao quanh lưới cờ. Phải đủ lớn để hàng quân ngoài cùng
// không che mất các nhãn toạ độ nằm trong khung.
constexpr float BOARD_MARGIN = 56.0f;

// Vùng thông tin (bảng điều khiển) bên phải bàn cờ
constexpr float SIDEBAR_X = 752.0f;
constexpr float SIDEBAR_Y = 28.0f;
constexpr float SIDEBAR_WIDTH = 480.0f;
constexpr float SIDEBAR_HEIGHT = 664.0f;

// --- Thông số sinh hình ảnh (AssetManager) ---
// Quân cờ được vẽ ở độ phân giải cao rồi thu nhỏ, giúp viền và chữ Hán sắc nét.
constexpr unsigned int PIECE_TEXTURE_SIZE = 256;
// Tỉ lệ đường kính đĩa cờ so với cạnh texture (phần còn lại dành cho bóng đổ).
constexpr float PIECE_DISC_FRACTION = 0.80f;
// Bàn cờ cũng được vẽ ở độ phân giải gấp đôi rồi thu nhỏ.
constexpr float BOARD_TEXTURE_SCALE = 2.0f;
// Đường kính quân cờ khi hiển thị, tính theo tỉ lệ ô cờ.
constexpr float PIECE_DISPLAY_RATIO = 0.90f;

enum class Side {
    NONE = 0,
    RED = 1,    // Tiên Đạo - Thái Hư Huyễn Phái (Đi trước)
    BLACK = 2   // Ma Đạo - Trầm Mộc Đan Tông (Đi sau)
};

inline Side getOpponentSide(Side s) {
    if (s == Side::RED) return Side::BLACK;
    if (s == Side::BLACK) return Side::RED;
    return Side::NONE;
}

enum class PieceType {
    NONE = 0,
    KING,      // Tướng / Soái
    GUARD,     // Sĩ / Sĩ
    BISHOP,    // Tượng / Tượng
    KNIGHT,    // Mã / Mã
    ROOK,      // Xe / Xe
    CANNON,    // Pháo / Pháo
    PAWN       // Tốt / Binh
};

enum class GameMode {
    PVP,       // Hai người cùng một máy
    PVE        // Người chơi vs AI Tiên Cơ
};

enum class Difficulty {
    EASY = 1,   // Cảnh giới: Luyện Khí (Depth 1)
    MEDIUM = 3, // Cảnh giới: Trúc Cơ (Depth 3)
    HARD = 5    // Cảnh giới: Kim Đan (Depth 5)
};

enum class GameState {
    PLAYING,
    CHECK,      // Đang bị chiếu tướng
    RED_WON,    // Thái Hư Huyễn Phái chiến thắng
    BLACK_WON,  // Trầm Mộc Đan Tông chiến thắng
    DRAW        // Hòa cờ
};

enum class Faction {
    THAI_HU = 0,   // Thái Hư Huyễn Phái (Phe Đỏ)
    TRAM_MOC = 1   // Trầm Mộc Đan Tông (Phe Đen)
};

// Vị trí tọa độ hàng, cột trên bàn cờ
struct Position {
    int row = -1;
    int col = -1;

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
    bool isValid() const {
        return row >= 0 && row < BOARD_ROWS && col >= 0 && col < BOARD_COLS;
    }
};

} // namespace BachChien

