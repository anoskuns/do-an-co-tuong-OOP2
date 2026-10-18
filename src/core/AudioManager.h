#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <vector>
#include <memory>

namespace BachChien {

class AudioManager {
private:
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::vector<std::unique_ptr<sf::Sound>> soundPool;
    sf::Music currentBgm;
    std::string currentBgmName;

    float sfxVolume = 80.0f;
    float musicVolume = 60.0f;

    // Quét folder âm thanh an toàn
    void loadSoundIfExists(const std::string& name, const std::string& baseFileName);

public:
    AudioManager();

    void init();

    // Phát hiệu ứng âm thanh (SFX) an toàn
    void playSound(const std::string& name);

    // Phát nhạc nền (BGM) an toàn
    void playBGM(const std::string& name, bool loop = true);

    // Dừng nhạc nền
    void stopBGM();

    // Điều chỉnh âm lượng
    void setSFXVolume(float vol);
    void setMusicVolume(float vol);

    float getSFXVolume() const { return sfxVolume; }
    float getMusicVolume() const { return musicVolume; }
};

} // namespace BachChien

