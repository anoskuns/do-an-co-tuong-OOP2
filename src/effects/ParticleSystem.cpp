#include "effects/ParticleSystem.h"
#include <chrono>
#include <cmath>

namespace BachChien {

ParticleSystem::ParticleSystem() {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
}

void ParticleSystem::update(float dt) {
    for (auto it = particles.begin(); it != particles.end();) {
        it->lifetime += dt;
        if (!it->isAlive()) {
            it = particles.erase(it);
        } else {
            it->position += it->velocity * dt;

            // Hiệu ứng mờ dần (fade out)
            float progress = it->lifetime / it->maxLifetime;
            float alphaFactor = 1.0f - progress;
            if (alphaFactor < 0.0f) alphaFactor = 0.0f;

            if (it->type == ParticleType::MIST) {
                it->color.a = static_cast<sf::Uint8>(44.0f * alphaFactor);
            } else {
                it->color.a = static_cast<sf::Uint8>(240.0f * alphaFactor);
            }

            ++it;
        }
    }
}

void ParticleSystem::draw(sf::RenderTarget& target, const sf::Texture& softTexture) {
    const sf::Vector2u textureSize = softTexture.getSize();
    if (textureSize.x == 0 || textureSize.y == 0) {
        return;
    }
    const float half = static_cast<float>(textureSize.x) * 0.5f;

    for (const auto& p : particles) {
        // Sương mù cần tán rộng và mềm; tia lửa thì gọn và sáng hơn.
        const float spread = (p.type == ParticleType::MIST) ? 3.4f : 2.4f;

        sf::Sprite sprite(softTexture);
        sprite.setOrigin(half, half);
        const float scale = (p.size * spread) / static_cast<float>(textureSize.x);
        sprite.setScale(scale, scale);
        sprite.setPosition(p.position);
        sprite.setColor(p.color);
        // Tia lửa và vụ nổ dùng chế độ cộng sáng để rực rỡ hơn.
        target.draw(sprite, p.type == ParticleType::MIST ? sf::BlendAlpha : sf::BlendAdd);
    }
}

void ParticleSystem::emitMist(const sf::FloatRect& area, int count) {
    std::uniform_real_distribution<float> distX(area.left, area.left + area.width);
    std::uniform_real_distribution<float> distY(area.top, area.top + area.height);
    std::uniform_real_distribution<float> velX(-12.0f, 12.0f);
    std::uniform_real_distribution<float> velY(-20.0f, -6.0f);
    std::uniform_real_distribution<float> sizeDist(14.0f, 32.0f);
    std::uniform_real_distribution<float> lifeDist(3.0f, 5.5f);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = {distX(rng), distY(rng)};
        p.velocity = {velX(rng), velY(rng)};
        p.color = sf::Color(180, 220, 240, 50); // Sương lam mờ
        p.size = sizeDist(rng);
        p.lifetime = 0.0f;
        p.maxLifetime = lifeDist(rng);
        p.type = ParticleType::MIST;
        particles.push_back(p);
    }
}

void ParticleSystem::emitMoveTrail(sf::Vector2f from, sf::Vector2f to, sf::Color color, int count) {
    std::uniform_real_distribution<float> distOffset(-10.0f, 10.0f);
    std::uniform_real_distribution<float> lifeDist(0.35f, 0.7f);
    std::uniform_real_distribution<float> sizeDist(3.0f, 6.0f);

    sf::Vector2f diff = to - from;

    for (int i = 0; i < count; ++i) {
        float t = static_cast<float>(i) / count;
        Particle p;
        p.position = from + diff * t + sf::Vector2f(distOffset(rng), distOffset(rng));
        p.velocity = sf::Vector2f(distOffset(rng) * 2.0f, distOffset(rng) * 2.0f);
        p.color = color;
        p.size = sizeDist(rng);
        p.lifetime = 0.0f;
        p.maxLifetime = lifeDist(rng);
        p.type = ParticleType::SPARK;
        particles.push_back(p);
    }
}

void ParticleSystem::emitCaptureExplosion(sf::Vector2f pos, sf::Color color, int count) {
    std::uniform_real_distribution<float> angleDist(0.0f, 6.283185f);
    std::uniform_real_distribution<float> speedDist(50.0f, 220.0f);
    std::uniform_real_distribution<float> lifeDist(0.4f, 0.9f);
    std::uniform_real_distribution<float> sizeDist(4.0f, 8.0f);

    for (int i = 0; i < count; ++i) {
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        Particle p;
        p.position = pos;
        p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        p.color = color;
        p.size = sizeDist(rng);
        p.lifetime = 0.0f;
        p.maxLifetime = lifeDist(rng);
        p.type = ParticleType::EXPLOSION;
        particles.push_back(p);
    }
}

void ParticleSystem::emitCheckAura(sf::Vector2f kingPos, int count) {
    std::uniform_real_distribution<float> angleDist(0.0f, 6.283185f);
    std::uniform_real_distribution<float> distR(25.0f, 45.0f);
    std::uniform_real_distribution<float> speedDist(15.0f, 40.0f);

    for (int i = 0; i < count; ++i) {
        float angle = angleDist(rng);
        float r = distR(rng);
        Particle p;
        p.position = kingPos + sf::Vector2f(std::cos(angle) * r, std::sin(angle) * r);
        p.velocity = sf::Vector2f(std::cos(angle) * speedDist(rng), std::sin(angle) * speedDist(rng));
        p.color = sf::Color(255, 40, 40, 220); // Đỏ huyết cảnh báo
        p.size = 5.0f;
        p.lifetime = 0.0f;
        p.maxLifetime = 0.5f;
        p.type = ParticleType::CHECK_AURA;
        particles.push_back(p);
    }
}

} // namespace BachChien

