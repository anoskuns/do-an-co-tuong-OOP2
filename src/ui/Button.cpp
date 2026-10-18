#include "ui/Button.h"
#include "ui/Theme.h"

#include <algorithm>
#include <cmath>

namespace BachChien {

namespace {
constexpr float CORNER_RADIUS = 10.0f;
}

Button::Button(const std::string& label, const sf::Font& font, sf::Vector2f pos, sf::Vector2f buttonSize,
               ButtonStyle btnStyle)
    : position(pos), size(buttonSize), style(btnStyle) {
    text.setFont(font);
    text.setString(Theme::toUnicode(label));
    text.setCharacterSize(21);
    text.setStyle(sf::Text::Bold);
    centerText();
}

void Button::centerText() {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    text.setPosition(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
}

void Button::setPosition(sf::Vector2f pos) {
    position = pos;
    centerText();
}

void Button::setLabel(const std::string& label) {
    text.setString(Theme::toUnicode(label));
    centerText();
}

sf::Color Button::accent() const {
    switch (style) {
        case ButtonStyle::GOLD:   return Theme::Palette::Gold;
        case ButtonStyle::CYAN:   return Theme::Palette::Jade;
        case ButtonStyle::DANGER: return Theme::Palette::CrimsonBright;
        case ButtonStyle::DARK:   return sf::Color(150, 156, 176);
    }
    return Theme::Palette::Gold;
}

sf::Color Button::bodyTop() const {
    switch (style) {
        case ButtonStyle::GOLD:   return sf::Color(74, 54, 22);
        case ButtonStyle::CYAN:   return sf::Color(22, 58, 74);
        case ButtonStyle::DANGER: return sf::Color(80, 26, 22);
        case ButtonStyle::DARK:   return sf::Color(50, 53, 64);
    }
    return sf::Color(74, 54, 22);
}

sf::Color Button::bodyBottom() const {
    switch (style) {
        case ButtonStyle::GOLD:   return sf::Color(34, 24, 10);
        case ButtonStyle::CYAN:   return sf::Color(10, 30, 42);
        case ButtonStyle::DANGER: return sf::Color(40, 11, 11);
        case ButtonStyle::DARK:   return sf::Color(26, 28, 36);
    }
    return sf::Color(34, 24, 10);
}

sf::Color Button::labelColor() const {
    switch (style) {
        case ButtonStyle::GOLD:   return Theme::Palette::GoldPale;
        case ButtonStyle::CYAN:   return Theme::Palette::JadeBright;
        case ButtonStyle::DANGER: return sf::Color(255, 202, 192);
        case ButtonStyle::DARK:   return sf::Color(216, 221, 236);
    }
    return Theme::Palette::GoldPale;
}

bool Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    const sf::FloatRect bounds(position, size);

    if (event.type == sf::Event::MouseMoved) {
        const sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        isHovered = bounds.contains(mousePos);
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            const sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            if (bounds.contains(mousePos)) {
                isPressed = true;
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && isPressed) {
            isPressed = false;
            const sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            if (bounds.contains(mousePos)) {
                if (onClick) onClick();
                return true;
            }
        }
    }
    return false;
}

void Button::update(float dt) {
    // Nội suy mượt trạng thái rê chuột để nút "sáng dần" thay vì nhảy tức thời.
    const float target = isHovered ? 1.0f : 0.0f;
    const float speed = std::min(1.0f, dt * 12.0f);
    hoverAmount += (target - hoverAmount) * speed;
    if (std::abs(hoverAmount - target) < 0.01f) {
        hoverAmount = target;
    }
}

void Button::draw(sf::RenderTarget& target) const {
    // Nút bị nhấn thì hạ xuống 2px để tạo cảm giác "ấn".
    const float pressOffset = isPressed ? 2.0f : 0.0f;
    const sf::FloatRect rect(position.x, position.y + pressOffset, size.x, size.y);

    const sf::Color accentColor = accent();

    // 1. Bóng đổ
    Theme::drawSoftShadow(target, rect, CORNER_RADIUS, 10.0f, 150);

    // 2. Quầng sáng khi rê chuột
    if (hoverAmount > 0.01f) {
        Theme::drawGlow(target,
                        sf::Vector2f(rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f),
                        std::max(rect.width, rect.height) * 0.92f,
                        Theme::withAlpha(accentColor, static_cast<sf::Uint8>(120.0f * hoverAmount)),
                        1.0f);
    }

    // 3. Thân nút: chuyển sắc dọc + sáng dần khi rê chuột
    const sf::Color top = Theme::mix(bodyTop(), Theme::brighten(bodyTop(), 0.10f), hoverAmount);
    const sf::Color bottom = Theme::mix(bodyBottom(), Theme::brighten(bodyBottom(), 0.06f), hoverAmount);

    Theme::drawRoundedRectGradient(target, rect, CORNER_RADIUS,
                                   Theme::withAlpha(top, 242), Theme::withAlpha(bottom, 250));

    // 4. Viền ngoài + đường viền trong mảnh
    Theme::drawRoundedRect(target, rect, CORNER_RADIUS, sf::Color::Transparent,
                           Theme::withAlpha(accentColor, static_cast<sf::Uint8>(150.0f + 90.0f * hoverAmount)),
                           1.8f);
    Theme::drawRoundedRect(target,
                           sf::FloatRect(rect.left + 3.0f, rect.top + 3.0f,
                                         rect.width - 6.0f, rect.height - 6.0f),
                           CORNER_RADIUS - 3.0f, sf::Color::Transparent,
                           Theme::withAlpha(Theme::Palette::GoldBright,
                                            static_cast<sf::Uint8>(22.0f + 40.0f * hoverAmount)),
                           1.0f);

    // 5. Vệt sáng mảnh ở cạnh trên, gợi chất liệu kim loại.
    sf::RectangleShape sheen(sf::Vector2f(rect.width - 26.0f, 1.4f));
    sheen.setPosition(rect.left + 13.0f, rect.top + 3.4f);
    sheen.setFillColor(Theme::withAlpha(Theme::Palette::GoldPale,
                                        static_cast<sf::Uint8>(30.0f + 60.0f * hoverAmount)));
    target.draw(sheen);

    // 6. Chữ trên nút (kèm bóng mờ để luôn rõ trên nền tối)
    sf::Text label = text;
    label.move(0.0f, pressOffset);
    label.setFillColor(sf::Color(0, 0, 0, 130));
    label.move(0.0f, 1.5f);
    target.draw(label);

    label.move(0.0f, -1.5f);
    label.setFillColor(Theme::mix(labelColor(), sf::Color::White, hoverAmount * 0.45f));
    target.draw(label);
}

} // namespace BachChien
