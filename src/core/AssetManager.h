#pragma once

#include "core/Common.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

namespace BachChien {

// ===========================================================================
//  AssetManager — Quản lý và tự sinh toàn bộ tài nguyên đồ họa của trò chơi.
//
//  Toàn bộ hình ảnh (bàn cờ, 14 quân cờ, nền huyền ảo, huy hiệu, hạt sáng)
//  được vẽ bằng thuật toán procedural nên game không phụ thuộc vào file ảnh
//  ngoài. Nếu người dùng muốn thay ảnh riêng, chỉ cần đặt file PNG cùng tên
//  vào assets/images/pieces/ hoặc assets/images/board/ — game sẽ ưu tiên ảnh đó.
// ===========================================================================
class AssetManager {
private:
    std::map<std::string, sf::Texture> textures;

    sf::Font primaryFont;    // Font thư pháp (KaiTi) — dùng cho Hán tự & con dấu
    sf::Font uiFont;         // Font chữ Latin — hỗ trợ đầy đủ dấu tiếng Việt
    bool hasPrimaryFont = false;
    bool hasUiFont = false;

    void loadFonts();
    void ensureDirectories() const;

    void buildAllTextures();
    void buildPieceTexture(PieceType type, Side side, const std::string& key);
    void buildBoardTexture();
    void buildSceneBackground();
    void buildEmblemTexture();
    void buildParticleTexture();

    // Nạp ảnh do người dùng cung cấp; trả về false nếu không có file.
    bool tryLoadUserTexture(const std::string& key, const std::string& relativePath);

public:
    AssetManager();

    bool init();

    // Font thư pháp dùng cho Hán tự (帥, 將, 楚河, 漢界...).
    // Lưu ý: font Hán KHÔNG chứa đầy đủ ký tự có dấu của tiếng Việt.
    const sf::Font& getFont() const;

    // Font dùng cho toàn bộ chữ Latin/tiếng Việt trên giao diện.
    const sf::Font& getUiFont() const;

    bool hasFont() const { return hasPrimaryFont || hasUiFont; }

    // Lấy Texture an toàn (nếu không có thì trả về texture trong suốt 1x1)
    const sf::Texture& getTexture(const std::string& name) const;
    sf::Texture& getTexture(const std::string& name);

    bool hasTexture(const std::string& name) const;

    // Xuất các texture đang dùng ra file PNG để người dùng tiện chỉnh sửa.
    // Chỉ chạy khi biến môi trường BCT_EXPORT_PLACEHOLDERS=1.
    void exportPlaceholderFiles() const;
};

} // namespace BachChien
