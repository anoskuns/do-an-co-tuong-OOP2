#pragma once

#include "core/Scene.h"
#include <map>
#include <string>
#include <memory>

namespace BachChien {

class SceneManager {
private:
    std::map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* currentScene = nullptr;
    std::string currentSceneName;

public:
    SceneManager() = default;

    void addScene(const std::string& name, std::unique_ptr<Scene> scene);
    bool changeScene(const std::string& name);

    Scene* getCurrentScene() { return currentScene; }
    const std::string& getCurrentSceneName() const { return currentSceneName; }

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);
};

} // namespace BachChien

