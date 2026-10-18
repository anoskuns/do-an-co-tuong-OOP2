#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

namespace BachChien {

// ===========================================================================
//  Theme — Bộ "design token" dùng chung cho toàn bộ giao diện.
//  Mọi màn hình đều lấy màu sắc và hình khối từ đây để giao diện đồng nhất.
// ===========================================================================
namespace Theme {

// ---------------------- Bảng màu Tu Tiên Huyền Huyễn ----------------------
namespace Palette {
inline const sf::Color Ink          (10, 12, 18);    // Nền đêm sâu thẳm
inline const sf::Color Night        (18, 22, 34);    // Nền phụ
inline const sf::Color Indigo       (28, 35, 58);    // Sương lam
inline const sf::Color WoodDark     (44, 31, 22);    // Gỗ mun
inline const sf::Color WoodMid      (78, 54, 36);    // Gỗ trầm
inline const sf::Color WoodLight    (122, 86, 54);   // Gỗ sáng (viền)
inline const sf::Color GoldDark     (138, 102, 46);
inline const sf::Color Gold         (201, 163, 78);  // Hoàng kim
inline const sf::Color GoldBright   (240, 210, 132);
inline const sf::Color GoldPale     (255, 236, 176);
inline const sf::Color Jade         (72, 208, 190);  // Lam ngọc
inline const sf::Color JadeBright   (142, 240, 224);
inline const sf::Color Crimson      (172, 40, 34);   // Chu sa
inline const sf::Color CrimsonBright(232, 86, 62);
inline const sf::Color Ivory        (243, 229, 202); // Ngọc trắng ngà
inline const sf::Color Steel        (28, 40, 50);    // Huyền thiết
inline const sf::Color SteelBright  (140, 214, 232);
inline const sf::Color TextDim      (146, 155, 174);
inline const sf::Color TextLight    (228, 232, 242);
} // namespace Palette

// --------------------------- Tiện ích màu sắc -----------------------------
sf::Color withAlpha(const sf::Color& color, sf::Uint8 alpha);
sf::Color mix(const sf::Color& a, const sf::Color& b, float t);
sf::Color brighten(const sf::Color& color, float amount);

// Chuyển chuỗi UTF-8 (tiếng Việt có dấu, Hán tự) sang sf::String.
sf::String toUnicode(const std::string& utf8Text);

// ------------------------------ Hình khối ---------------------------------
// Hình chữ nhật bo góc (dùng VertexArray nên vẽ được góc tròn mượt).
void drawRoundedRect(sf::RenderTarget& target, const sf::FloatRect& rect, float radius,
                     const sf::Color& fill,
                     const sf::Color& outline = sf::Color::Transparent,
                     float outlineThickness = 0.0f);

// Hình chữ nhật bo góc tô chuyển sắc theo chiều dọc (giữ nguyên góc bo tròn).
void drawRoundedRectGradient(sf::RenderTarget& target, const sf::FloatRect& rect, float radius,
                             const sf::Color& top, const sf::Color& bottom);

// Dải màu chuyển sắc theo chiều dọc.
void drawVerticalGradient(sf::RenderTarget& target, const sf::FloatRect& rect,
                          const sf::Color& top, const sf::Color& bottom);

// Đổ bóng mềm phía sau một hình chữ nhật bo góc.
void drawSoftShadow(sf::RenderTarget& target, const sf::FloatRect& rect, float radius,
                    float spread, sf::Uint8 alpha);

// Quầng sáng tỏa tròn (dùng texture gradient hoàn chỉnh, không bị vân bậc thang).
void drawGlow(sf::RenderTarget& target, sf::Vector2f center, float radius,
              const sf::Color& color, float intensity = 1.0f);

// Texture gradient tròn trắng, tái sử dụng cho quầng sáng / bóng đổ.
const sf::Texture& glowTexture();

} // namespace Theme

} // namespace BachChien
