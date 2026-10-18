#pragma once

#include "effects/Particle.h"
#include <vector>
#include <random>

namespace BachChien {

class ParticleSystem {
private:
    std::vector<Particle> particles;
    std::mt19937 rng;

public:
    ParticleSystem();

    void update(float dt);

    // Vẽ hạt bằng sprite quầng sáng mềm để hiệu ứng tan dần tự nhiên.
    void draw(sf::RenderTarget& target, const sf::Texture& softTexture);

    // Phát sinh sương mù linh khí
    void emitMist(const sf::FloatRect& area, int count = 2);

    // Phát sinh vệt linh quang khi quân cờ di chuyển
    void emitMoveTrail(sf::Vector2f from, sf::Vector2f to, sf::Color color, int count = 18);

    // Phát sinh vụ nổ linh lực khi ăn quân
    void emitCaptureExplosion(sf::Vector2f pos, sf::Color color, int count = 35);

    // Phát sinh vòng cảnh báo chiếu tướng
    void emitCheckAura(sf::Vector2f kingPos, int count = 8);

    void clear() {
        particles.clear();
    }
};

} // namespace BachChien

