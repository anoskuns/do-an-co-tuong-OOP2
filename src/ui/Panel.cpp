#include "ui/Panel.h"
#include "ui/Theme.h"

#include <algorithm>

namespace BachChien {

namespace {
constexpr float CORNER_RADIUS = 14.0f;
constexpr float HEADER_HEIGHT = 54.0f;
}

Panel::Panel(sf::Vector2f pos, sf::Vector2f panelSize, const std::string& title, const sf::Font* font)
    : position(pos), size(panelSize) {
    if (!title.empty() && font) {
        setTitle(title, *font);
    }
}

void Panel::setTitle(const std::string& title, const sf::Font& font) {
    hasTitle = true;
    titleText.setFont(font);
    titleText.setString(Theme::toUnicode(title));
    titleText.setCharacterSize(23);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(Theme::Palette::GoldBright);
    titleText.setLetterSpacing(1.1f);

    const sf::FloatRect bounds = titleText.getLocalBounds();
    titleText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    titleText.setPosition(position.x + size.x * 0.5f, position.y + HEADER_HEIGHT * 0.5f + 2.0f);
}

void Panel::setPosition(sf::Vector2f pos) {
    const sf::Vector2f offset = pos - position;
    position = pos;
    if (hasTitle) {
        titleText.move(offset);
    }
}

void Panel::draw(sf::RenderTarget& target) const {
    const sf::FloatRect rect(position, size);
    const float radius = std::min(CORNER_RADIUS, std::min(size.x, size.y) * 0.5f);

    // 1. Bóng đổ
    Theme::drawSoftShadow(target, rect, radius, 16.0f, 170);

    // 2. Thân khung: chuyển sắc dọc, hơi trong suốt để nền huyền ảo lộ qua
    Theme::drawRoundedRectGradient(target, rect, radius,
                                   sf::Color(30, 35, 50, 238), sf::Color(13, 16, 24, 248));

    // 3. Ánh sáng mềm ở phía trên khung
    Theme::drawGlow(target, sf::Vector2f(rect.left + rect.width * 0.5f, rect.top),
                    rect.width * 0.55f, sf::Color(120, 150, 210, 40), 1.0f);

    // 4. Viền kim tuyến hai lớp
    Theme::drawRoundedRect(target, rect, radius, sf::Color::Transparent,
                           Theme::withAlpha(Theme::Palette::Gold, 165), 2.0f);
    Theme::drawRoundedRect(target,
                           sf::FloatRect(rect.left + 5.0f, rect.top + 5.0f,
                                         rect.width - 10.0f, rect.height - 10.0f),
                           std::max(0.0f, radius - 5.0f), sf::Color::Transparent,
                           Theme::withAlpha(Theme::Palette::GoldBright, 55), 1.0f);

    // 5. Hoa văn góc: viên ngọc thoi nhỏ ở bốn góc
    const float gemInset = 13.0f;
    for (int cx = 0; cx < 2; ++cx) {
        for (int cy = 0; cy < 2; ++cy) {
            const sf::Vector2f gem(cx == 0 ? rect.left + gemInset : rect.left + rect.width - gemInset,
                                   cy == 0 ? rect.top + gemInset : rect.top + rect.height - gemInset);
            sf::CircleShape diamond(3.6f, 4);
            diamond.setOrigin(3.6f, 3.6f);
            diamond.setPosition(gem);
            diamond.setFillColor(Theme::withAlpha(Theme::Palette::GoldBright, 150));
            target.draw(diamond);
        }
    }

    if (!hasTitle) {
        return;
    }

    // 6. Dải tiêu đề: nền sáng hơn một chút
    Theme::drawRoundedRectGradient(target,
                                   sf::FloatRect(rect.left + 2.0f, rect.top + 2.0f,
                                                 rect.width - 4.0f, HEADER_HEIGHT),
                                   std::max(0.0f, radius - 2.0f),
                                   sf::Color(78, 62, 34, 90), sf::Color(78, 62, 34, 0));

    // 7. Đường phân cách dưới tiêu đề, có viên ngọc ở giữa
    const float ruleY = rect.top + HEADER_HEIGHT + 4.0f;
    const float ruleWidth = rect.width - 60.0f;
    sf::VertexArray ruleLine(sf::Quads, 4);
    const sf::Color ruleEnd = Theme::withAlpha(Theme::Palette::Gold, 0);
    const sf::Color ruleMid = Theme::withAlpha(Theme::Palette::Gold, 150);
    ruleLine[0] = sf::Vertex(sf::Vector2f(rect.left + 30.0f, ruleY), ruleEnd);
    ruleLine[1] = sf::Vertex(sf::Vector2f(rect.left + 30.0f + ruleWidth * 0.5f, ruleY), ruleMid);
    ruleLine[2] = sf::Vertex(sf::Vector2f(rect.left + 30.0f + ruleWidth * 0.5f, ruleY + 1.2f), ruleMid);
    ruleLine[3] = sf::Vertex(sf::Vector2f(rect.left + 30.0f, ruleY + 1.2f), ruleEnd);
    target.draw(ruleLine);

    sf::VertexArray ruleLine2(sf::Quads, 4);
    ruleLine2[0] = sf::Vertex(sf::Vector2f(rect.left + 30.0f + ruleWidth * 0.5f, ruleY), ruleMid);
    ruleLine2[1] = sf::Vertex(sf::Vector2f(rect.left + 30.0f + ruleWidth, ruleY), ruleEnd);
    ruleLine2[2] = sf::Vertex(sf::Vector2f(rect.left + 30.0f + ruleWidth, ruleY + 1.2f), ruleEnd);
    ruleLine2[3] = sf::Vertex(sf::Vector2f(rect.left + 30.0f + ruleWidth * 0.5f, ruleY + 1.2f), ruleMid);
    target.draw(ruleLine2);

    sf::CircleShape ruleGem(4.2f, 4);
    ruleGem.setOrigin(4.2f, 4.2f);
    ruleGem.setPosition(rect.left + rect.width * 0.5f, ruleY + 0.6f);
    ruleGem.setFillColor(Theme::withAlpha(Theme::Palette::GoldBright, 220));
    target.draw(ruleGem);

    // 8. Tiêu đề kèm bóng mờ
    sf::Text shadow = titleText;
    shadow.setFillColor(sf::Color(0, 0, 0, 160));
    shadow.move(0.0f, 1.5f);
    target.draw(shadow);
    target.draw(titleText);
}

} // namespace BachChien
