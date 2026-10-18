#include "ui/Theme.h"

#include <algorithm>
#include <cmath>

namespace BachChien {
namespace Theme {

namespace {

constexpr float PI = 3.14159265358979f;

// Dựng danh sách điểm chu vi của một hình chữ nhật bo góc, bắt đầu từ góc
// trên-trái và đi theo chiều kim đồng hồ.
std::vector<sf::Vector2f> buildPerimeter(const sf::FloatRect& rect, float radius, int segmentsPerCorner = 8) {
    radius = std::max(0.0f, std::min(radius, std::min(rect.width, rect.height) * 0.5f));

    struct Corner {
        float x;
        float y;
        float startAngle;
    };
    const Corner corners[4] = {
        {rect.left + radius,                rect.top + radius,                 PI},
        {rect.left + rect.width - radius,   rect.top + radius,                 1.5f * PI},
        {rect.left + rect.width - radius,   rect.top + rect.height - radius,   0.0f},
        {rect.left + radius,                rect.top + rect.height - radius,   0.5f * PI}
    };

    std::vector<sf::Vector2f> points;
    points.reserve(static_cast<std::size_t>(4 * (segmentsPerCorner + 1)));
    for (const Corner& corner : corners) {
        for (int s = 0; s <= segmentsPerCorner; ++s) {
            const float angle = corner.startAngle + (PI * 0.5f) * static_cast<float>(s) / static_cast<float>(segmentsPerCorner);
            points.emplace_back(corner.x + std::cos(angle) * radius,
                                corner.y + std::sin(angle) * radius);
        }
    }
    return points;
}

} // namespace

// ---------------------------------------------------------------------------
//  Màu sắc
// ---------------------------------------------------------------------------
sf::Color withAlpha(const sf::Color& color, sf::Uint8 alpha) {
    return sf::Color(color.r, color.g, color.b, alpha);
}

sf::Color mix(const sf::Color& a, const sf::Color& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    auto blend = [t](sf::Uint8 lhs, sf::Uint8 rhs) {
        return static_cast<sf::Uint8>(lhs + (rhs - lhs) * t);
    };
    return sf::Color(blend(a.r, b.r), blend(a.g, b.g), blend(a.b, b.b), blend(a.a, b.a));
}

sf::Color brighten(const sf::Color& color, float amount) {
    return sf::Color(
        static_cast<sf::Uint8>(std::min(255.0f, color.r + 255.0f * amount)),
        static_cast<sf::Uint8>(std::min(255.0f, color.g + 255.0f * amount)),
        static_cast<sf::Uint8>(std::min(255.0f, color.b + 255.0f * amount)),
        color.a);
}

sf::String toUnicode(const std::string& utf8Text) {
    return sf::String::fromUtf8(utf8Text.begin(), utf8Text.end());
}

// ---------------------------------------------------------------------------
//  Hình khối
// ---------------------------------------------------------------------------
void drawRoundedRect(sf::RenderTarget& target, const sf::FloatRect& rect, float radius,
                     const sf::Color& fill, const sf::Color& outline, float outlineThickness) {
    if (rect.width <= 0.0f || rect.height <= 0.0f) {
        return;
    }

    const std::vector<sf::Vector2f> perimeter = buildPerimeter(rect, radius);

    if (fill.a > 0) {
        // Lưu ý: TriangleFan KHÔNG tự khép vòng, nên phải lặp lại điểm đầu tiên
        // ở cuối, nếu không sẽ thiếu mất mảnh tam giác ở cạnh nối điểm cuối với
        // điểm đầu (mảnh bên trái của hình chữ nhật).
        sf::VertexArray fan(sf::TriangleFan, perimeter.size() + 2);
        fan[0] = sf::Vertex(sf::Vector2f(rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f), fill);
        for (std::size_t i = 0; i < perimeter.size(); ++i) {
            fan[i + 1] = sf::Vertex(perimeter[i], fill);
        }
        fan[perimeter.size() + 1] = sf::Vertex(perimeter[0], fill);
        target.draw(fan);
    }

    if (outline.a > 0 && outlineThickness > 0.0f) {
        const float thickness = std::min(outlineThickness,
                                         std::min(rect.width, rect.height) * 0.5f - 0.5f);
        if (thickness <= 0.0f) {
            return;
        }
        const sf::FloatRect innerRect(rect.left + thickness, rect.top + thickness,
                                      rect.width - 2.0f * thickness, rect.height - 2.0f * thickness);
        const std::vector<sf::Vector2f> innerPerimeter =
            buildPerimeter(innerRect, std::max(0.0f, radius - thickness));

        sf::VertexArray ring(sf::TriangleStrip, perimeter.size() * 2 + 2);
        for (std::size_t i = 0; i < perimeter.size(); ++i) {
            ring[i * 2 + 0] = sf::Vertex(perimeter[i], outline);
            ring[i * 2 + 1] = sf::Vertex(innerPerimeter[i], outline);
        }
        ring[perimeter.size() * 2 + 0] = sf::Vertex(perimeter[0], outline);
        ring[perimeter.size() * 2 + 1] = sf::Vertex(innerPerimeter[0], outline);
        target.draw(ring);
    }
}

void drawRoundedRectGradient(sf::RenderTarget& target, const sf::FloatRect& rect, float radius,
                             const sf::Color& top, const sf::Color& bottom) {
    if (rect.width <= 0.0f || rect.height <= 0.0f || rect.height <= 0.0f) {
        return;
    }

    const std::vector<sf::Vector2f> perimeter = buildPerimeter(rect, radius);
    // Màu của mỗi đỉnh được nội suy theo toạ độ y nên cả hình giữ được góc bo tròn.
    auto shadeAt = [&](float y) {
        const float t = std::clamp((y - rect.top) / rect.height, 0.0f, 1.0f);
        return mix(top, bottom, t);
    };

    // Cũng phải lặp lại điểm đầu để khép kín vòng (xem chú thích ở drawRoundedRect).
    sf::VertexArray fan(sf::TriangleFan, perimeter.size() + 2);
    fan[0] = sf::Vertex(sf::Vector2f(rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f),
                        shadeAt(rect.top + rect.height * 0.5f));
    for (std::size_t i = 0; i < perimeter.size(); ++i) {
        fan[i + 1] = sf::Vertex(perimeter[i], shadeAt(perimeter[i].y));
    }
    fan[perimeter.size() + 1] = sf::Vertex(perimeter[0], shadeAt(perimeter[0].y));
    target.draw(fan);
}

void drawVerticalGradient(sf::RenderTarget& target, const sf::FloatRect& rect,
                          const sf::Color& top, const sf::Color& bottom) {    sf::VertexArray quad(sf::Quads, 4);
    quad[0] = sf::Vertex(sf::Vector2f(rect.left, rect.top), top);
    quad[1] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top), top);
    quad[2] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top + rect.height), bottom);
    quad[3] = sf::Vertex(sf::Vector2f(rect.left, rect.top + rect.height), bottom);
    target.draw(quad);
}

void drawSoftShadow(sf::RenderTarget& target, const sf::FloatRect& rect, float radius,
                    float spread, sf::Uint8 alpha) {
    const int layers = 14;
    for (int i = layers; i >= 1; --i) {
        const float t = static_cast<float>(i) / static_cast<float>(layers);
        const float grow = spread * t;
        const auto layerAlpha = static_cast<sf::Uint8>(alpha * (1.0f - t) * (1.0f - t) / layers * 3.0f);
        if (layerAlpha == 0) {
            continue;
        }
        drawRoundedRect(target,
                        sf::FloatRect(rect.left - grow, rect.top - grow + spread * 0.25f,
                                      rect.width + grow * 2.0f, rect.height + grow * 2.0f),
                        radius + grow,
                        sf::Color(0, 0, 0, layerAlpha));
    }
}

const sf::Texture& glowTexture() {
    static sf::Texture texture = [] {
        const unsigned int size = 256;
        sf::Image image;
        image.create(size, size, sf::Color::Transparent);

        const float half = size * 0.5f;
        for (unsigned int y = 0; y < size; ++y) {
            for (unsigned int x = 0; x < size; ++x) {
                const float dx = (static_cast<float>(x) + 0.5f - half) / half;
                const float dy = (static_cast<float>(y) + 0.5f - half) / half;
                const float distance = std::sqrt(dx * dx + dy * dy);
                if (distance >= 1.0f) {
                    continue;
                }
                // Đường cong falloff mũ để quầng sáng tan dần tự nhiên.
                const float falloff = std::pow(1.0f - distance, 2.2f);
                image.setPixel(x, y, sf::Color(255, 255, 255,
                                               static_cast<sf::Uint8>(falloff * 255.0f)));
            }
        }

        sf::Texture result;
        result.loadFromImage(image);
        result.setSmooth(true);
        return result;
    }();
    return texture;
}

void drawGlow(sf::RenderTarget& target, sf::Vector2f center, float radius,
              const sf::Color& color, float intensity) {
    const sf::Texture& texture = glowTexture();
    const float size = static_cast<float>(texture.getSize().x);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setOrigin(size * 0.5f, size * 0.5f);
    const float scale = (radius * 2.0f) / size;
    sprite.setScale(scale, scale);
    sprite.setPosition(center);

    const auto alpha = static_cast<sf::Uint8>(std::clamp(color.a * intensity, 0.0f, 255.0f));
    sprite.setColor(sf::Color(color.r, color.g, color.b, alpha));
    target.draw(sprite);
}

} // namespace Theme
} // namespace BachChien
