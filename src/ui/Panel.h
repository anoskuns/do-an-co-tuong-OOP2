#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace BachChien {

// ===========================================================================
//  Panel — Khung nội dung bo góc, chuyển sắc, viền kim tuyến và hoa văn góc.
// ===========================================================================
class Panel {
private:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Text titleText;
    bool hasTitle = false;

public:
    Panel(sf::Vector2f position, sf::Vector2f size, const std::string& title = "", const sf::Font* font = nullptr);

    void setPosition(sf::Vector2f pos);
    void setTitle(const std::string& title, const sf::Font& font);
    void draw(sf::RenderTarget& target) const;
};

} // namespace BachChien
