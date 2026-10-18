#include "core/SceneManager.h"
#include <iostream>

namespace BachChien {

void SceneManager::addScene(const std::string& name, std::unique_ptr<Scene> scene) {
    scenes[name] = std::move(scene);
}

bool SceneManager::changeScene(const std::string& name) {
    auto it = scenes.find(name);
    if (it == scenes.end()) {
        std::cerr << "[Lỗi Cảnh] Không tìm thấy Scene: " << name << "\n";
        return false;
    }

    if (currentScene) {
        currentScene->onExit();
    }

    currentScene = it->second.get();
    currentSceneName = name;
    currentScene->onEnter();
    return true;
}

void SceneManager::handleEvent(const sf::Event& event) {
    if (currentScene) {
        currentScene->handleEvent(event);
    }
}

void SceneManager::update(float dt) {
    if (currentScene) {
        currentScene->update(dt);
    }
}

void SceneManager::render(sf::RenderWindow& window) {
    if (currentScene) {
        currentScene->render(window);
    }
}

} // namespace BachChien

