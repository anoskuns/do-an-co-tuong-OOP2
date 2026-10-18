#pragma once

#include <string>

namespace BachChien {

class SettingsManager {
private:
    float sfxVolume = 80.0f;
    float musicVolume = 60.0f;
    bool fullscreen = false;
    std::string configFile;

public:
    SettingsManager();

    float getSFXVolume() const { return sfxVolume; }
    void setSFXVolume(float val);

    float getMusicVolume() const { return musicVolume; }
    void setMusicVolume(float val);

    bool isFullscreen() const { return fullscreen; }
    void setFullscreen(bool fs);

    bool save();
    bool load();
};

} // namespace BachChien

