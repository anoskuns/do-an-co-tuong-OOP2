#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace BachChien {

enum class ButtonStyle {
    GOLD,   // Phong cách Tiên gia Hoàng Kim
    CYAN,   // Phong cách U Minh Hàn Băng
    DANGER, // Phong cách Huyết Sát
    DARK    // Phong cách Trầm Mặc
};

// ===========================================================================
//  Button — Nút bấm bo góc, có đổ bóng, chuyển sắc và quầng sáng khi rê chuột.
// ===========================================================================
class Button {
private:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Text text;
    ButtonStyle style;
    std::function<void()> onClick;

    bool isHovered = false;
    bool isPressed = false;
    float hoverAmount = 0.0f;  // 0 -> 1, nội suy mượt theo thời gian

    sf::Color accent() const;
    sf::Color bodyTop() const;
    sf::Color bodyBottom() const;
    sf::Color labelColor() const;
    void centerText();

public:
    Button(const std::string& label, const sf::Font& font, sf::Vector2f position, sf::Vector2f size,
           ButtonStyle btnStyle = ButtonStyle::GOLD);

    void setOnClick(std::function<void()> callback) { onClick = callback; }
    void setPosition(sf::Vector2f pos);
    void setLabel(const std::string& label);

    bool handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(float dt);
    void draw(sf::RenderTarget& target) const;
};

} // namespace BachChien
