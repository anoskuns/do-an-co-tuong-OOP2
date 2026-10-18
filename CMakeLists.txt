cmake_minimum_required(VERSION 3.16)
project(BachChienTienCuc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# Tự động nạp thư viện SFML từ third_party
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window audio system REQUIRED)

# Sinh header cấu hình chứa đường dẫn tuyệt đối tới assets/ trong mã nguồn
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/BuildConfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/BCT_BuildConfig.h"
    @ONLY
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

set(CHESS_SOURCES
    src/chess/pieces/Piece.cpp
    src/chess/pieces/King.cpp
    src/chess/pieces/Guard.cpp
    src/chess/pieces/Bishop.cpp
    src/chess/pieces/Knight.cpp
    src/chess/pieces/Rook.cpp
    src/chess/pieces/Cannon.cpp
    src/chess/pieces/Pawn.cpp
    src/chess/Board.cpp
    src/chess/AIPlayer.cpp
    src/chess/GameController.cpp
    # Đường dẫn tài nguyên dùng chung cho cả logic lẫn giao diện (lưu/tải ván cờ)
    src/core/AssetPaths.cpp
)

set(GAME_SOURCES
    ${CHESS_SOURCES}
    src/core/SettingsManager.cpp
    src/core/AssetManager.cpp
    src/core/AudioManager.cpp
    src/core/SceneManager.cpp
    src/core/Game.cpp
    src/effects/ParticleSystem.cpp
    src/ui/Button.cpp
    src/ui/Panel.cpp
    src/ui/Theme.cpp
    src/scenes/MenuScene.cpp
    src/scenes/FactionSelectScene.cpp
    src/scenes/GameScene.cpp
    src/scenes/SettingsScene.cpp
    src/scenes/HelpScene.cpp
    src/scenes/ResultScene.cpp
    src/scenes/SaveLoadScene.cpp
    src/main.cpp
)

add_executable(BachChienTienCuc ${GAME_SOURCES})
target_link_libraries(BachChienTienCuc PRIVATE sfml-graphics sfml-window sfml-audio sfml-system)

# Automated Unit Tests
add_executable(ChessLogicTest tests/test_chess_logic.cpp ${CHESS_SOURCES})

# ---------------------------------------------------------------------------
# Sao chép tài nguyên (assets/) vào thư mục chứa file chạy.
# Nhờ vậy game luôn tìm thấy font, hình ảnh, âm thanh và thư mục lưu ván cờ
# dù được khởi chạy từ Explorer, Visual Studio hay dòng lệnh.
# ---------------------------------------------------------------------------
add_custom_command(TARGET BachChienTienCuc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:BachChienTienCuc>/assets"
    COMMENT "Sao chép tài nguyên assets/ vào thư mục chạy game"
)

# Copy các file DLL của SFML vào thư mục chứa file thực thi
file(GLOB SFML_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SFML-2.6.1/bin/*.dll")
foreach(DLL ${SFML_DLLS})
    add_custom_command(TARGET BachChienTienCuc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL}
        $<TARGET_FILE_DIR:BachChienTienCuc>
    )
endforeach()
