#include "core/Game.h"
#include <iostream>

int main() {
    std::cout << "========================================\n";
    std::cout << "  BÁCH CHIẾN TIÊN CỤC \n";
    std::cout << "  Cờ Tướng Tu Tiên Huyền Huyễn OOP C++\n";
    std::cout << "========================================\n";

    BachChien::Game game;
    if (!game.init()) {
        std::cerr << "[Lỗi Khởi Động] Không thể khởi động Bách Chiến Tiên Cục!\n";
        return -1;
    }

    game.run();

    return 0;
}

