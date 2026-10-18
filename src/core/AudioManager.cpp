#include "core/AudioManager.h"
#include "core/AssetPaths.h"
#include <iostream>
#include <filesystem>

namespace BachChien {

AudioManager::AudioManager() {
    // Khởi tạo pool âm thanh
    for (int i = 0; i < 16; ++i) {
        soundPool.push_back(std::make_unique<sf::Sound>());
    }
}

void AudioManager::loadSoundIfExists(const std::string& name, const std::string& baseFileName) {
    const std::vector<std::string> extensions = {".ogg", ".wav", ".mp3", ".flac"};
    const std::string folder = assetPath("sounds/sfx") + "/";

    for (const auto& ext : extensions) {
        std::string path = folder + baseFileName + ext;
        if (std::filesystem::exists(path)) {
            sf::SoundBuffer buffer;
            if (buffer.loadFromFile(path)) {
                soundBuffers[name] = buffer;
                std::cout << "[Âm thanh] Đã tải hiệu ứng: " << path << "\n";
                return;
            }
        }
    }
}

void AudioManager::init() {
    // Tự động quét các hiệu ứng chuẩn trong assets/sounds/sfx/
    loadSoundIfExists("select", "select");
    loadSoundIfExists("move", "move");
    loadSoundIfExists("capture", "capture");
    loadSoundIfExists("check", "check");
    loadSoundIfExists("victory", "victory");
    loadSoundIfExists("defeat", "defeat");
    loadSoundIfExists("click", "click");
}

void AudioManager::playSound(const std::string& name) {
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) return;

    // Tìm một Sound object đang rảnh trong pool
    for (auto& s : soundPool) {
        if (s->getStatus() != sf::Sound::Playing) {
            s->setBuffer(it->second);
            s->setVolume(sfxVolume);
            s->play();
            return;
        }
    }

    // Nếu tất cả đều bận, lấy cái đầu tiên tái sử dụng
    if (!soundPool.empty()) {
        soundPool[0]->setBuffer(it->second);
        soundPool[0]->setVolume(sfxVolume);
        soundPool[0]->play();
    }
}

void AudioManager::playBGM(const std::string& name, bool loop) {
    if (currentBgmName == name && currentBgm.getStatus() == sf::Music::Playing) {
        return; // Đang phát đúng bài này rồi
    }

    const std::vector<std::string> extensions = {".ogg", ".wav", ".mp3", ".flac"};
    const std::string folder = assetPath("sounds/bgm") + "/";

    for (const auto& ext : extensions) {
        std::string path = folder + name + ext;
        if (std::filesystem::exists(path)) {
            currentBgm.stop();
            if (currentBgm.openFromFile(path)) {
                currentBgmName = name;
                currentBgm.setLoop(loop);
                currentBgm.setVolume(musicVolume);
                currentBgm.play();
                std::cout << "[Nhạc nền] Đang phát BGM: " << path << "\n";
                return;
            }
        }
    }
}

void AudioManager::stopBGM() {
    currentBgm.stop();
    currentBgmName = "";
}

void AudioManager::setSFXVolume(float vol) {
    sfxVolume = vol;
    for (auto& s : soundPool) {
        s->setVolume(sfxVolume);
    }
}

void AudioManager::setMusicVolume(float vol) {
    musicVolume = vol;
    currentBgm.setVolume(musicVolume);
}

} // namespace BachChien

