#include "core/AssetManager.h"
#include "ui/Theme.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include <vector>

namespace BachChien {
namespace {

constexpr float PI = 3.14159265358979f;

// --------------------------------------------------------------------------
//  Các hàm vẽ phụ trợ dùng chung khi sinh texture
// --------------------------------------------------------------------------

// Vẽ đoạn thẳng có độ dày tuỳ ý (SFML chỉ hỗ trợ đường mảnh 1px).
void drawThickLine(sf::RenderTarget& target, sf::Vector2f from, sf::Vector2f to,
                   float thickness, const sf::Color& color) {
    const sf::Vector2f delta = to - from;
    const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    if (length < 0.001f || thickness <= 0.0f) {
        return;
    }
    sf::RectangleShape shape(sf::Vector2f(length, thickness));
    shape.setOrigin(length * 0.5f, thickness * 0.5f);
    shape.setPosition((from.x + to.x) * 0.5f, (from.y + to.y) * 0.5f);
    shape.setRotation(std::atan2(delta.y, delta.x) * 180.0f / PI);
    shape.setFillColor(color);
    target.draw(shape);
}

// Vòng tròn rỗng (vẽ bằng dải tam giác nên độ dày chính xác và mượt).
void drawRing(sf::RenderTarget& target, sf::Vector2f center, float radius,
              float thickness, const sf::Color& color, int segments = 120) {
    const float outer = radius + thickness * 0.5f;
    const float inner = std::max(0.0f, radius - thickness * 0.5f);

    sf::VertexArray strip(sf::TriangleStrip, static_cast<std::size_t>((segments + 1) * 2));
    for (int i = 0; i <= segments; ++i) {
        const float angle = (2.0f * PI) * static_cast<float>(i) / static_cast<float>(segments);
        const float ca = std::cos(angle);
        const float sa = std::sin(angle);
        strip[static_cast<std::size_t>(i * 2 + 0)] =
            sf::Vertex(sf::Vector2f(center.x + ca * outer, center.y + sa * outer), color);
        strip[static_cast<std::size_t>(i * 2 + 1)] =
            sf::Vertex(sf::Vector2f(center.x + ca * inner, center.y + sa * inner), color);
    }
    target.draw(strip);
}

// Hình thoi nhỏ dùng làm hoa văn trang trí.
void drawDiamond(sf::RenderTarget& target, sf::Vector2f center, float radius,
                 const sf::Color& color) {
    sf::CircleShape diamond(radius, 4);
    diamond.setOrigin(radius, radius);
    diamond.setPosition(center);
    diamond.setFillColor(color);
    target.draw(diamond);
}

// Dải chuyển sắc 4 đỉnh (dọc hoặc ngang).
void drawGradientQuad(sf::RenderTarget& target, const sf::FloatRect& rect,
                      const sf::Color& first, const sf::Color& second, bool horizontal) {
    sf::VertexArray quad(sf::Quads, 4);
    if (horizontal) {
        quad[0] = sf::Vertex(sf::Vector2f(rect.left, rect.top), first);
        quad[1] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top), second);
        quad[2] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top + rect.height), second);
        quad[3] = sf::Vertex(sf::Vector2f(rect.left, rect.top + rect.height), first);
    } else {
        quad[0] = sf::Vertex(sf::Vector2f(rect.left, rect.top), first);
        quad[1] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top), first);
        quad[2] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top + rect.height), second);
        quad[3] = sf::Vertex(sf::Vector2f(rect.left, rect.top + rect.height), second);
    }
    target.draw(quad);
}

// Vân gỗ: các đường lượn sóng chạy ngang, tần số thay đổi để trông tự nhiên.
void drawWoodGrain(sf::RenderTarget& target, const sf::FloatRect& rect, float step,
                   float amplitude, sf::Uint8 alpha, float phase, bool lighter) {
    if (step <= 0.5f) {
        step = 0.5f;
    }
    const int points = 72;
    const sf::Color color = lighter ? sf::Color(255, 226, 180, alpha) : sf::Color(28, 16, 8, alpha);

    for (float y = rect.top; y < rect.top + rect.height; y += step) {
        sf::VertexArray line(sf::LineStrip, points);
        for (int i = 0; i < points; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(points - 1);
            const float x = rect.left + rect.width * t;
            const float wobble = amplitude * std::sin(t * 6.4f + phase + y * 0.035f)
                               + amplitude * 0.45f * std::sin(t * 18.5f + phase * 2.3f + y * 0.09f)
                               + amplitude * 0.22f * std::sin(t * 41.0f + phase * 0.7f);
            line[i] = sf::Vertex(sf::Vector2f(x, y + wobble), color);
        }
        target.draw(line);
    }
}

// Bóng mờ hắt vào bên trong một hình chữ nhật, tạo cảm giác bề mặt hơi lõm.
void drawInnerShadow(sf::RenderTarget& target, const sf::FloatRect& rect, float depth) {
    const sf::Color dark(46, 27, 13, 120);
    const sf::Color clear(46, 27, 13, 0);
    drawGradientQuad(target, sf::FloatRect(rect.left, rect.top, rect.width, depth), dark, clear, false);
    drawGradientQuad(target, sf::FloatRect(rect.left, rect.top + rect.height - depth, rect.width, depth),
                     clear, dark, false);
    drawGradientQuad(target, sf::FloatRect(rect.left, rect.top, depth, rect.height), dark, clear, true);
    drawGradientQuad(target, sf::FloatRect(rect.left + rect.width - depth, rect.top, depth, rect.height),
                     clear, dark, true);
}

void drawTextCentered(sf::RenderTarget& target, const sf::Font& font, const std::string& text,
                      unsigned int characterSize, sf::Vector2f center, const sf::Color& color,
                      sf::Uint32 style = sf::Text::Regular) {
    sf::Text label;
    label.setFont(font);
    label.setString(Theme::toUnicode(text));
    label.setCharacterSize(characterSize);
    label.setStyle(style);
    label.setFillColor(color);

    const sf::FloatRect bounds = label.getLocalBounds();
    label.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    label.setPosition(center);
    target.draw(label);
}

std::string pieceGlyph(PieceType type, Side side) {
    const bool red = (side == Side::RED);
    switch (type) {
        case PieceType::KING:   return red ? "帥" : "將";
        case PieceType::GUARD:  return red ? "仕" : "士";
        case PieceType::BISHOP: return red ? "相" : "象";
        case PieceType::KNIGHT: return red ? "傌" : "馬";
        case PieceType::ROOK:   return red ? "俥" : "車";
        case PieceType::CANNON: return red ? "炮" : "砲";
        case PieceType::PAWN:   return red ? "兵" : "卒";
        default: return "";
    }
}

std::string pieceKeyPrefix(Side side) {
    return (side == Side::RED) ? "red_" : "black_";
}
std::string pieceKeySuffix(PieceType type) {
    switch (type) {
        case PieceType::KING:   return "king";
        case PieceType::GUARD:  return "guard";
        case PieceType::BISHOP: return "bishop";
        case PieceType::KNIGHT: return "knight";
        case PieceType::ROOK:   return "rook";
        case PieceType::CANNON: return "cannon";
        case PieceType::PAWN:   return "pawn";
        default: return "";
    }
}

// Kiểm tra font có thực sự chứa các ký tự có dấu của tiếng Việt hay không.
// Cách làm: so sánh glyph của ký tự cần kiểm tra với glyph ".notdef" (ký tự
// thay thế mà FreeType trả về khi font không có ký tự đó).
bool supportsVietnamese(const sf::Font& font) {
    const sf::Glyph& missingGlyph = font.getGlyph(0xE000, 30, false, 0.0f);
    const sf::Uint32 samples[] = {
        0x1EC7, // ệ
        0x1EAD, // ậ
        0x1ED1, // ố
        0x1EF9, // ỹ
        0x1EA2, // Ả
        0x01B0, // ư
        0x01A1, // ơ
        0x0111, // đ
    };
    for (const sf::Uint32 codePoint : samples) {
        const sf::Glyph& glyph = font.getGlyph(codePoint, 30, false, 0.0f);
        if (glyph.textureRect.width == 0 || glyph.textureRect == missingGlyph.textureRect) {
            return false;
        }
    }
    return true;
}

} // namespace

// ===========================================================================
//  Khởi tạo
// ===========================================================================

AssetManager::AssetManager() = default;

bool AssetManager::init() {
    ensureDirectories();
    loadFonts();
    buildAllTextures();

    if (std::getenv("BCT_EXPORT_PLACEHOLDERS") != nullptr) {
        exportPlaceholderFiles();
    }
    return true;
}

void AssetManager::ensureDirectories() const {
    const char* folders[] = {
        "images/pieces", "images/board", "sounds/bgm", "sounds/sfx", "saves"
    };
    for (const char* folder : folders) {
        std::error_code ec;
        std::filesystem::create_directories(assetPath(folder), ec);
    }
}

void AssetManager::loadFonts() {
    // ---------------------------------------------------------------------
    //  1. Font thư pháp cho Hán tự (帥, 將, 楚河, 漢界, 帥...).
    // ---------------------------------------------------------------------
    const std::string hanCandidates[] = {
        assetPath("fonts/calligraphy.ttf"),
        assetPath("fonts/han.ttf"),
        assetPath("fonts/default.ttf"),
        "C:/Windows/Fonts/simkai.ttf",
        "C:/Windows/Fonts/simsun.ttc",
        "C:/Windows/Fonts/msyh.ttc",
    };
    for (const std::string& path : hanCandidates) {
        if (primaryFont.loadFromFile(path)) {
            hasPrimaryFont = true;
            std::cout << "[AssetManager] Font Hán tự: " << path << "\n";
            break;
        }
    }

    // ---------------------------------------------------------------------
    //  2. Font chữ Latin/tiếng Việt cho toàn bộ giao diện.
    //     Đây là bước bắt buộc: font Hán tự ở trên không có ký tự có dấu.
    // ---------------------------------------------------------------------
    const std::string uiCandidates[] = {
        assetPath("fonts/ui.ttf"),
        assetPath("fonts/vietnamese.ttf"),
        "C:/Windows/Fonts/pala.ttf",      // Palatino Linotype — serif thanh lịch
        "C:/Windows/Fonts/georgia.ttf",
        "C:/Windows/Fonts/cambria.ttc",
        assetPath("fonts/default.ttf"),   // Font đi kèm dự án (Arial)
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
    };
    for (const std::string& path : uiCandidates) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) {
            continue;
        }
        sf::Font probe;
        if (!probe.loadFromFile(path)) {
            continue;
        }
        if (!supportsVietnamese(probe)) {
            continue;
        }
        uiFont = probe;
        hasUiFont = true;
        std::cout << "[AssetManager] Font giao diện: " << path << "\n";
        break;
    }

    // ---------------------------------------------------------------------
    //  3. Xử lý khi thiếu font.
    // ---------------------------------------------------------------------
    if (!hasUiFont) {
        // Không tìm được font hỗ trợ tiếng Việt: dùng tạm font Hán tự.
        uiFont = primaryFont;
        hasUiFont = hasPrimaryFont;
        std::cerr << "[Cảnh báo] Không tìm thấy font hỗ trợ tiếng Việt.\n"
                  << "           Hãy đặt một file .ttf vào: " << assetPath("fonts") << "\n";
    }

    if (!hasPrimaryFont && hasUiFont) {
        // Không có font Hán: dùng font giao diện để chữ vẫn hiển thị được.
        primaryFont = uiFont;
        hasPrimaryFont = true;
    }

    if (!hasPrimaryFont && !hasUiFont) {
        std::cerr << "[Cảnh báo] Không nạp được font nào. Toàn bộ chữ sẽ không hiển thị.\n";
    }
}

const sf::Font& AssetManager::getFont() const {
    return primaryFont;
}

const sf::Font& AssetManager::getUiFont() const {
    return uiFont;
}

// ===========================================================================
//  Truy vấn texture
// ===========================================================================

const sf::Texture& AssetManager::getTexture(const std::string& name) const {
    const auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    // Texture trong suốt 1x1: an toàn để vẽ mà không gây lỗi OpenGL.
    static const sf::Texture empty = [] {
        sf::Image image;
        image.create(1, 1, sf::Color::Transparent);
        sf::Texture texture;
        texture.loadFromImage(image);
        return texture;
    }();
    return empty;
}

sf::Texture& AssetManager::getTexture(const std::string& name) {
    return textures[name];
}

bool AssetManager::hasTexture(const std::string& name) const {
    return textures.find(name) != textures.end();
}

bool AssetManager::tryLoadUserTexture(const std::string& key, const std::string& relativePath) {
    std::error_code ec;
    const std::string path = assetPath(relativePath);
    if (!std::filesystem::exists(path, ec)) {
        return false;
    }
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        return false;
    }
    texture.setSmooth(true);
    textures[key] = texture;
    return true;
}

void AssetManager::buildAllTextures() {
    for (const Side side : {Side::RED, Side::BLACK}) {
        for (const PieceType type : {PieceType::KING, PieceType::GUARD, PieceType::BISHOP,
                                     PieceType::KNIGHT, PieceType::ROOK, PieceType::CANNON,
                                     PieceType::PAWN}) {
            buildPieceTexture(type, side, pieceKeyPrefix(side) + pieceKeySuffix(type));
        }
    }

    buildBoardTexture();
    buildSceneBackground();
    buildEmblemTexture();
    buildParticleTexture();
}

// ===========================================================================
//  Quân cờ
// ===========================================================================

void AssetManager::buildPieceTexture(PieceType type, Side side, const std::string& key) {
    // Người dùng có thể ghi đè bằng ảnh riêng.
    if (tryLoadUserTexture(key, "images/pieces/" + key + ".png")) {
        return;
    }

    const unsigned int size = PIECE_TEXTURE_SIZE;
    const float half = static_cast<float>(size) * 0.5f;
    const float discRadius = half * PIECE_DISC_FRACTION;
    const bool isRed = (side == Side::RED);

    sf::RenderTexture rt;
    if (!rt.create(size, size)) {
        std::cerr << "[Cảnh báo] Không tạo được texture cho quân " << key << "\n";
        return;
    }
    rt.clear(sf::Color::Transparent);

    // ---- Bảng chất liệu: Đỏ = ngọc trắng ngà + chu sa, Đen = trầm mộc + bích ngọc ----
    sf::Color rimOuter, rimInner, bodyEdge, bodyCenter, glyphColor, glyphRelief, ringColor;
    if (isRed) {
        rimOuter    = Theme::Palette::Crimson;                        // (172, 40, 34)
        rimInner    = Theme::Palette::CrimsonBright;
        bodyEdge    = sf::Color(198, 170, 128);                       // rìa ngà sẫm
        bodyCenter  = sf::Color(252, 243, 223);                       // tâm ngà sáng
        glyphColor  = sf::Color(154, 24, 20);
        glyphRelief = sf::Color(255, 253, 246, 210);                  // chữ nổi
        ringColor   = sf::Color(172, 40, 34, 170);
    } else {
        rimOuter    = sf::Color(46, 74, 66);                          // trầm mộc pha bích
        rimInner    = sf::Color(102, 156, 138);
        bodyEdge    = sf::Color(28, 43, 40);
        bodyCenter  = sf::Color(66, 96, 88);
        glyphColor  = sf::Color(178, 240, 214);
        glyphRelief = sf::Color(0, 0, 0, 150);                        // chữ chìm
        ringColor   = sf::Color(120, 190, 166, 150);
    }

    // 1. Bóng đổ phía sau quân cờ.
    Theme::drawGlow(rt, sf::Vector2f(half + discRadius * 0.05f, half + discRadius * 0.09f),
                    discRadius * 1.14f, sf::Color(0, 0, 0, 140), 1.0f);

    // 2. Vành ngoài (viền màu phe).
    sf::CircleShape rim(discRadius);
    rim.setOrigin(discRadius, discRadius);
    rim.setPosition(half, half);
    rim.setFillColor(rimOuter);
    rt.draw(rim);

    // 3. Ánh sáng viền trên-trái, bóng viền dưới-phải (tạo khối nổi).
    Theme::drawGlow(rt, sf::Vector2f(half - discRadius * 0.42f, half - discRadius * 0.46f),
                    discRadius * 0.72f, Theme::withAlpha(rimInner, 130), 1.0f);
    Theme::drawGlow(rt, sf::Vector2f(half + discRadius * 0.40f, half + discRadius * 0.46f),
                    discRadius * 0.70f, sf::Color(0, 0, 0, 120), 1.0f);

    // 4. Thân quân cờ: gradient hướng tâm, tâm sáng lệch lên trên-trái.
    const float bodyRadius = discRadius - discRadius * 0.085f;
    const int gradientLayers = 56;
    for (int i = 0; i < gradientLayers; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(gradientLayers - 1);
        const float radius = bodyRadius * (1.0f - t * 0.985f);
        const sf::Color color = Theme::mix(bodyEdge, bodyCenter, std::pow(t, 0.72f));
        const float offsetX = -discRadius * 0.055f * t;
        const float offsetY = -discRadius * 0.085f * t;

        sf::CircleShape layer(radius);
        layer.setOrigin(radius, radius);
        layer.setPosition(half + offsetX, half + offsetY);
        layer.setFillColor(color);
        rt.draw(layer);
    }

    // 5. Vòng hoa văn khắc trên mặt quân.
    drawRing(rt, sf::Vector2f(half, half), bodyRadius - discRadius * 0.10f,
             discRadius * 0.028f, ringColor);
    drawRing(rt, sf::Vector2f(half, half), bodyRadius - discRadius * 0.175f,
             discRadius * 0.014f, Theme::withAlpha(ringColor, static_cast<sf::Uint8>(ringColor.a * 0.6f)));

    // 6. Chữ Hán: bản khắc chìm phía sau rồi tới nét chính.
    const std::string glyph = pieceGlyph(type, side);
    if (hasPrimaryFont && !glyph.empty()) {
        const unsigned int glyphSize = static_cast<unsigned int>(discRadius * 1.62f);
        drawTextCentered(rt, primaryFont, glyph, glyphSize,
                         sf::Vector2f(half + discRadius * 0.030f, half + discRadius * 0.045f),
                         glyphRelief, sf::Text::Bold);
        drawTextCentered(rt, primaryFont, glyph, glyphSize,
                         sf::Vector2f(half, half), glyphColor, sf::Text::Bold);
    }

    // 7. Điểm sáng phản chiếu mềm ở góc trên-trái.
    Theme::drawGlow(rt, sf::Vector2f(half - discRadius * 0.34f, half - discRadius * 0.40f),
                    discRadius * 0.46f, sf::Color(255, 255, 255, 58), 1.0f);

    // 8. Tướng được điểm thêm một vòng kim tuyến để dễ nhận diện.
    if (type == PieceType::KING) {
        drawRing(rt, sf::Vector2f(half, half), discRadius - discRadius * 0.012f,
                 discRadius * 0.038f, Theme::withAlpha(Theme::Palette::GoldBright, 220));
    }

    rt.display();

    sf::Texture texture = rt.getTexture();
    texture.setSmooth(true);
    textures[key] = texture;
}

// ===========================================================================
//  Bàn cờ
// ===========================================================================

void AssetManager::buildBoardTexture() {
    if (tryLoadUserTexture("board", "images/board/board_bg.png")) {
        return;
    }

    const float scale = BOARD_TEXTURE_SCALE;
    const float tile = TILE_SIZE * scale;
    const float margin = BOARD_MARGIN * scale;
    const float gridWidth = (BOARD_COLS - 1) * tile;
    const float gridHeight = (BOARD_ROWS - 1) * tile;

    const unsigned int textureWidth = static_cast<unsigned int>(gridWidth + margin * 2.0f);
    const unsigned int textureHeight = static_cast<unsigned int>(gridHeight + margin * 2.0f);

    sf::RenderTexture rt;
    if (!rt.create(textureWidth, textureHeight)) {
        std::cerr << "[Cảnh báo] Không tạo được texture bàn cờ.\n";
        return;
    }
    rt.clear(sf::Color::Transparent);

    const float originX = margin;
    const float originY = margin;
    const sf::FloatRect panel(0.0f, 0.0f,
                              static_cast<float>(textureWidth), static_cast<float>(textureHeight));
    const sf::FloatRect grid(originX, originY, gridWidth, gridHeight);

    // ---------------- 1. Khung gỗ sẫm bao quanh ----------------
    Theme::drawVerticalGradient(rt, panel, sf::Color(92, 62, 38), sf::Color(41, 26, 15));
    drawWoodGrain(rt, panel, 7.0f * scale, 1.7f * scale, 26, 0.0f, false);
    drawWoodGrain(rt, panel, 14.0f * scale, 1.1f * scale, 18, 2.1f, true);

    // ---------------- 2. Mặt bàn gỗ sáng ----------------
    const float faceInset = margin * 0.30f;
    const sf::FloatRect face(grid.left - faceInset, grid.top - faceInset,
                             gridWidth + faceInset * 2.0f, gridHeight + faceInset * 2.0f);
    Theme::drawVerticalGradient(rt, face, sf::Color(219, 182, 128), sf::Color(166, 121, 70));
    drawWoodGrain(rt, face, 9.0f * scale, 1.3f * scale, 14, 1.2f, false);
    drawWoodGrain(rt, face, 21.0f * scale, 0.8f * scale, 10, 3.4f, true);
    drawInnerShadow(rt, face, 10.0f * scale);

    // ---------------- 3. Đường viền kim tuyến ----------------
    const sf::Color goldTrim = Theme::withAlpha(Theme::Palette::Gold, 205);
    const sf::Color goldThin = Theme::withAlpha(Theme::Palette::GoldBright, 130);
    const float trimInset = 5.0f * scale;

    Theme::drawRoundedRect(rt, panel, 0.0f, sf::Color::Transparent, goldTrim, 2.4f * scale);
    Theme::drawRoundedRect(rt,
                           sf::FloatRect(trimInset, trimInset,
                                         static_cast<float>(textureWidth) - trimInset * 2.0f,
                                         static_cast<float>(textureHeight) - trimInset * 2.0f),
                           0.0f, sf::Color::Transparent, goldThin, 1.0f * scale);

    // Hoa văn góc: hai thanh ngắn ôm góc và một viên ngọc thoi.
    const float cornerArm = 26.0f * scale;
    const float cornerEdge = 12.0f * scale;
    const float gemInset = 30.0f * scale;
    for (int cx = 0; cx < 2; ++cx) {
        for (int cy = 0; cy < 2; ++cy) {
            const float dirX = (cx == 0) ? 1.0f : -1.0f;
            const float dirY = (cy == 0) ? 1.0f : -1.0f;
            const float baseX = (cx == 0) ? cornerEdge : static_cast<float>(textureWidth) - cornerEdge;
            const float baseY = (cy == 0) ? cornerEdge : static_cast<float>(textureHeight) - cornerEdge;

            drawThickLine(rt, sf::Vector2f(baseX, baseY + dirY * cornerArm),
                          sf::Vector2f(baseX, baseY), 2.0f * scale, goldTrim);
            drawThickLine(rt, sf::Vector2f(baseX, baseY),
                          sf::Vector2f(baseX + dirX * cornerArm, baseY), 2.0f * scale, goldTrim);
            drawDiamond(rt,
                        sf::Vector2f((cx == 0) ? gemInset : static_cast<float>(textureWidth) - gemInset,
                                     (cy == 0) ? gemInset : static_cast<float>(textureHeight) - gemInset),
                        5.5f * scale, Theme::withAlpha(Theme::Palette::GoldBright, 190));
        }
    }

    // ---------------- 4. Lưới cờ ----------------
    const sf::Color lineColor(76, 46, 22, 235);
    const float lineWidth = 1.6f * scale;
    const float borderWidth = 2.6f * scale;

    // 10 đường ngang
    for (int row = 0; row < BOARD_ROWS; ++row) {
        const float y = originY + row * tile;
        const float thickness = (row == 0 || row == BOARD_ROWS - 1) ? borderWidth : lineWidth;
        drawThickLine(rt, sf::Vector2f(originX, y), sf::Vector2f(originX + gridWidth, y),
                      thickness, lineColor);
    }

    // 9 đường dọc: hai biên liền mạch, các cột giữa bị ngắt quãng bởi con sông.
    for (int col = 0; col < BOARD_COLS; ++col) {
        const float x = originX + col * tile;
        if (col == 0 || col == BOARD_COLS - 1) {
            drawThickLine(rt, sf::Vector2f(x, originY), sf::Vector2f(x, originY + gridHeight),
                          borderWidth, lineColor);
        } else {
            drawThickLine(rt, sf::Vector2f(x, originY),
                          sf::Vector2f(x, originY + 4.0f * tile), lineWidth, lineColor);
            drawThickLine(rt, sf::Vector2f(x, originY + 5.0f * tile),
                          sf::Vector2f(x, originY + gridHeight), lineWidth, lineColor);
        }
    }

    // Đường chéo Cửu Cung (cung của mỗi bên).
    for (int half = 0; half < 2; ++half) {
        const float topRow = (half == 0) ? 0.0f : 7.0f;
        drawThickLine(rt, sf::Vector2f(originX + 3.0f * tile, originY + topRow * tile),
                      sf::Vector2f(originX + 5.0f * tile, originY + (topRow + 2.0f) * tile),
                      lineWidth, lineColor);
        drawThickLine(rt, sf::Vector2f(originX + 5.0f * tile, originY + topRow * tile),
                      sf::Vector2f(originX + 3.0f * tile, originY + (topRow + 2.0f) * tile),
                      lineWidth, lineColor);
    }

    // ---------------- 5. Hoa văn định vị (điểm Pháo và điểm Binh/Tốt) ----------------
    const float markGap = tile * 0.115f;
    const float markLength = tile * 0.155f;
    const float markWidth = 1.5f * scale;

    auto drawPositionMark = [&](int row, int col) {
        const float cx = originX + col * tile;
        const float cy = originY + row * tile;
        for (int sx = -1; sx <= 1; sx += 2) {
            if ((sx < 0 && col == 0) || (sx > 0 && col == BOARD_COLS - 1)) {
                continue;
            }
            for (int sy = -1; sy <= 1; sy += 2) {
                const float cornerX = cx + sx * markGap;
                const float cornerY = cy + sy * markGap;
                drawThickLine(rt, sf::Vector2f(cornerX, cornerY + sy * markLength),
                              sf::Vector2f(cornerX, cornerY), markWidth, lineColor);
                drawThickLine(rt, sf::Vector2f(cornerX, cornerY),
                              sf::Vector2f(cornerX + sx * markLength, cornerY), markWidth, lineColor);
            }
        }
    };

    // Điểm Pháo
    drawPositionMark(2, 1);
    drawPositionMark(2, 7);
    drawPositionMark(7, 1);
    drawPositionMark(7, 7);
    // Điểm Binh / Tốt
    for (int col = 0; col < BOARD_COLS; col += 2) {
        drawPositionMark(3, col);
        drawPositionMark(6, col);
    }

    // ---------------- 6. Con sông: chữ "楚 河" và "漢 界" ----------------
    if (hasPrimaryFont) {
        const unsigned int riverSize = static_cast<unsigned int>(tile * 0.60f);
        const sf::Color riverColor(84, 52, 26, 225);
        drawTextCentered(rt, primaryFont, "楚 河", riverSize,
                         sf::Vector2f(originX + 2.0f * tile, originY + 4.5f * tile), riverColor);
        drawTextCentered(rt, primaryFont, "漢 界", riverSize,
                         sf::Vector2f(originX + 6.0f * tile, originY + 4.5f * tile), riverColor);
    }

    // ---------------- 7. Toạ độ cột ----------------
    if (hasPrimaryFont) {
        const unsigned int coordSize = static_cast<unsigned int>(tile * 0.24f);
        const sf::Color coordColor(214, 178, 118, 200);
        const char* chineseNumerals[9] = {"一", "二", "三", "四", "五", "六", "七", "八", "九"};

        // Phía Đỏ (dưới): số Hán đọc từ phải sang trái theo lối cổ.
        for (int col = 0; col < BOARD_COLS; ++col) {
            drawTextCentered(rt, primaryFont, chineseNumerals[BOARD_COLS - 1 - col], coordSize,
                             sf::Vector2f(originX + col * tile, originY + gridHeight + margin * 0.70f),
                             coordColor);
        }
        // Phía Đen (trên): chữ số Ả Rập.
        for (int col = 0; col < BOARD_COLS; ++col) {
            drawTextCentered(rt, primaryFont, std::to_string(col + 1), coordSize,
                             sf::Vector2f(originX + col * tile, originY - margin * 0.70f),
                             coordColor);
        }
    }

    rt.display();

    sf::Texture texture = rt.getTexture();
    texture.setSmooth(true);
    textures["board"] = texture;
}

// ===========================================================================
//  Nền huyền ảo dùng chung cho mọi màn hình
// ===========================================================================

void AssetManager::buildSceneBackground() {
    sf::RenderTexture rt;
    if (!rt.create(WINDOW_WIDTH, WINDOW_HEIGHT)) {
        std::cerr << "[Cảnh báo] Không tạo được texture nền.\n";
        return;
    }
    rt.clear(Theme::Palette::Ink);

    const sf::FloatRect full(0.0f, 0.0f,
                             static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));

    // 1. Bầu trời đêm chuyển sắc.
    drawGradientQuad(rt, full, sf::Color(23, 29, 46), sf::Color(7, 9, 15), false);

    // 2. Quầng linh khí phía sau khu vực nội dung.
    Theme::drawGlow(rt, sf::Vector2f(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.40f),
                    660.0f, sf::Color(56, 86, 142, 105), 1.0f);
    Theme::drawGlow(rt, sf::Vector2f(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.72f),
                    470.0f, sf::Color(146, 108, 52, 62), 1.0f);

    // 3. Sao trời lác đác.
    std::mt19937 rng(20261018u);
    std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(WINDOW_WIDTH));
    std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(WINDOW_HEIGHT) * 0.72f);
    std::uniform_real_distribution<float> distAlpha(30.0f, 150.0f);
    for (int i = 0; i < 220; ++i) {
        const float x = distX(rng);
        const float y = distY(rng);
        const float radius = (i % 9 == 0) ? 1.6f : 0.9f;
        Theme::drawGlow(rt, sf::Vector2f(x, y), radius * 3.2f,
                        sf::Color(214, 228, 255, static_cast<sf::Uint8>(distAlpha(rng))), 1.0f);
    }

    // 4. Ba dải núi xa tạo chiều sâu.
    auto drawRidge = [&](float baseY, float amplitude, float phase, const sf::Color& color) {
        const int points = 160;
        sf::VertexArray strip(sf::TriangleStrip, static_cast<std::size_t>((points + 1) * 2));
        for (int i = 0; i <= points; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(points);
            const float x = t * static_cast<float>(WINDOW_WIDTH);
            const float height = amplitude * (0.55f * std::sin(t * 3.1f + phase)
                                            + 0.30f * std::sin(t * 7.7f + phase * 1.7f)
                                            + 0.15f * std::sin(t * 15.3f + phase * 2.6f));
            const float y = baseY - height;
            strip[static_cast<std::size_t>(i * 2 + 0)] = sf::Vertex(sf::Vector2f(x, y), color);
            strip[static_cast<std::size_t>(i * 2 + 1)] =
                sf::Vertex(sf::Vector2f(x, static_cast<float>(WINDOW_HEIGHT)), color);
        }
        rt.draw(strip);
    };

    drawRidge(560.0f, 96.0f, 0.8f, sf::Color(26, 33, 52));
    drawRidge(646.0f, 84.0f, 2.4f, sf::Color(18, 24, 38));
    drawRidge(724.0f, 62.0f, 4.1f, sf::Color(10, 14, 23));

    // 5. Sương mù quyện trên sườn núi.
    drawGradientQuad(rt, sf::FloatRect(0.0f, 430.0f, static_cast<float>(WINDOW_WIDTH), 210.0f),
                     sf::Color(96, 130, 175, 0), sf::Color(96, 130, 175, 34), false);

    // 6. Vignette làm tối bốn phía, hướng mắt vào trung tâm.
    const sf::Color edgeDark(0, 0, 0, 165);
    const sf::Color edgeClear(0, 0, 0, 0);
    drawGradientQuad(rt, sf::FloatRect(0.0f, 0.0f, static_cast<float>(WINDOW_WIDTH), 190.0f),
                     edgeDark, edgeClear, false);
    drawGradientQuad(rt, sf::FloatRect(0.0f, static_cast<float>(WINDOW_HEIGHT) - 200.0f,
                                      static_cast<float>(WINDOW_WIDTH), 200.0f),
                     edgeClear, sf::Color(0, 0, 0, 200), false);
    drawGradientQuad(rt, sf::FloatRect(0.0f, 0.0f, 210.0f, static_cast<float>(WINDOW_HEIGHT)),
                     sf::Color(0, 0, 0, 150), edgeClear, true);
    drawGradientQuad(rt, sf::FloatRect(static_cast<float>(WINDOW_WIDTH) - 210.0f, 0.0f,
                                      210.0f, static_cast<float>(WINDOW_HEIGHT)),
                     edgeClear, sf::Color(0, 0, 0, 150), true);

    rt.display();

    sf::Texture texture = rt.getTexture();
    texture.setSmooth(true);
    textures["scene_bg"] = texture;
}

// ===========================================================================
//  Huy hiệu (con dấu) của trò chơi
// ===========================================================================

void AssetManager::buildEmblemTexture() {
    const unsigned int size = 512;
    const float half = size * 0.5f;

    sf::RenderTexture rt;
    if (!rt.create(size, size)) {
        return;
    }
    rt.clear(sf::Color::Transparent);

    const sf::Color gold = Theme::withAlpha(Theme::Palette::Gold, 215);
    const sf::Color goldBright = Theme::withAlpha(Theme::Palette::GoldBright, 235);

    // Quầng sáng và các vòng tròn đồng tâm.
    Theme::drawGlow(rt, sf::Vector2f(half, half), half * 0.98f,
                    Theme::withAlpha(Theme::Palette::Gold, 58), 1.0f);
    drawRing(rt, sf::Vector2f(half, half), half * 0.86f, 5.0f, gold);
    drawRing(rt, sf::Vector2f(half, half), half * 0.76f, 1.8f, goldBright);
    drawRing(rt, sf::Vector2f(half, half), half * 0.55f, 1.2f,
             Theme::withAlpha(Theme::Palette::Gold, 120));

    // Tám viên ngọc thoi điểm quanh vòng ngoài.
    for (int i = 0; i < 8; ++i) {
        const float angle = (2.0f * PI) * static_cast<float>(i) / 8.0f;
        drawDiamond(rt, sf::Vector2f(half + std::cos(angle) * half * 0.81f,
                                     half + std::sin(angle) * half * 0.81f),
                    5.0f, goldBright);
    }

    if (hasPrimaryFont) {
        drawTextCentered(rt, primaryFont, "棋", static_cast<unsigned int>(half * 0.62f),
                         sf::Vector2f(half, half * 1.02f), goldBright, sf::Text::Bold);
    }

    rt.display();

    sf::Texture texture = rt.getTexture();
    texture.setSmooth(true);
    textures["emblem"] = texture;
}

// ===========================================================================
//  Hạt sáng (hiệu ứng linh lực)
// ===========================================================================

void AssetManager::buildParticleTexture() {
    const unsigned int size = 64;
    const float half = size * 0.5f;

    sf::RenderTexture rt;
    if (!rt.create(size, size)) {
        return;
    }
    rt.clear(sf::Color::Transparent);

    Theme::drawGlow(rt, sf::Vector2f(half, half), half * 0.95f, sf::Color(255, 255, 255, 235), 1.0f);
    sf::CircleShape core(half * 0.26f);
    core.setOrigin(half * 0.26f, half * 0.26f);
    core.setPosition(half, half);
    core.setFillColor(sf::Color(255, 255, 255, 245));
    rt.draw(core);

    rt.display();

    sf::Texture texture = rt.getTexture();
    texture.setSmooth(true);
    textures["particle_spark"] = texture;
}

// ===========================================================================
//  Xuất ảnh ra đĩa để người dùng chỉnh sửa (tuỳ chọn)
// ===========================================================================

void AssetManager::exportPlaceholderFiles() const {
    int written = 0;

    for (const auto& entry : textures) {
        const std::string& name = entry.first;
        std::string outPath;

        if (name.rfind("red_", 0) == 0 || name.rfind("black_", 0) == 0) {
            outPath = assetPath("images/pieces/" + name + ".png");
        } else if (name == "board") {
            outPath = assetPath("images/board/board_bg.png");
        } else if (name == "scene_bg") {
            outPath = assetPath("images/background/scene_bg.png");
        } else if (name == "emblem") {
            outPath = assetPath("images/ui/emblem.png");
        } else {
            continue;
        }

        std::error_code ec;
        std::filesystem::create_directories(std::filesystem::path(outPath).parent_path(), ec);
        if (entry.second.copyToImage().saveToFile(outPath)) {
            ++written;
        }
    }

    std::cout << "[AssetManager] Đã xuất " << written << " file ảnh vào " << assetRoot() << "\n";
}

} // namespace BachChien
