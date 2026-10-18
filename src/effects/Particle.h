#pragma once

#include <SFML/Graphics.hpp>

namespace BachChien {

enum class ParticleType {
    MIST,       // Sương mù linh khí
    SPARK,      // Tia linh quang
    EXPLOSION,  // Nổ linh lực ăn quân
    CHECK_AURA  // Hào quang cảnh báo chiếu tướng
};

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float size;
    float lifetime;
    float maxLifetime;
    ParticleType type;

    bool isAlive() const {
        return lifetime < maxLifetime;
    }
};

} // namespace BachChien

