#include "core/SettingsManager.h"
#include "core/AssetPaths.h"
#include "third_party/nlohmann/json.hpp"
#include <fstream>
#include <algorithm>

namespace BachChien {

SettingsManager::SettingsManager() : configFile(assetPath("settings.json")) {
    if (!load()) {
        save();
    }
}

void SettingsManager::setSFXVolume(float val) {
    sfxVolume = std::clamp(val, 0.0f, 100.0f);
    save();
}

void SettingsManager::setMusicVolume(float val) {
    musicVolume = std::clamp(val, 0.0f, 100.0f);
    save();
}

void SettingsManager::setFullscreen(bool fs) {
    fullscreen = fs;
    save();
}

bool SettingsManager::save() {
    try {
        nlohmann::json j;
        j["sfx_volume"] = sfxVolume;
        j["music_volume"] = musicVolume;
        j["fullscreen"] = fullscreen;

        std::ofstream file(configFile);
        if (!file.is_open()) return false;
        file << j.dump(4);
        return true;
    } catch (...) {
        return false;
    }
}

bool SettingsManager::load() {
    try {
        std::ifstream file(configFile);
        if (!file.is_open()) return false;

        nlohmann::json j;
        file >> j;
        if (j.contains("sfx_volume")) sfxVolume = j["sfx_volume"];
        if (j.contains("music_volume")) musicVolume = j["music_volume"];
        if (j.contains("fullscreen")) fullscreen = j["fullscreen"];
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace BachChien

