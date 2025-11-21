#ifndef WAVES
#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "../ew/ewMath/ewMath.h"
#include "../shader/shader.h"

namespace hiWave
{
    struct WaveSettings
    {
        glm::vec2 direction = glm::vec2(1.0f, 1.0f);
        float wavelength = 1.0f;
        float steepness = 1.0f;

        WaveSettings(glm::vec2 direction, float wavelength, float steepness) : direction(direction), wavelength(wavelength), steepness(steepness) {}
    };

    class WaveSystem
    {
    public:
        // Constructor & Deocntructor
        WaveSystem() : waveCount(0), decreaseWaves(false) {}
        ~WaveSystem();

        // Getters
        WaveSettings* getWave(int index) { return waves[index]; }
        int getWaveCount() { return waveCount; }
        bool getDecreaseWaves() { return decreaseWaves; }
        bool* getDecreaseWavesAddress() { return &decreaseWaves; }

        // Functions
        void AddRandomWave();
        void AddWave(WaveSettings* wave);
        void RemoveWave(int index);
        void RemoveAllWaves();
        void PassValues(shdr::Shader* shader);

    private:
        int waveCount;
        bool decreaseWaves;
        const int MAX_WAVES = 10;
        std::vector<WaveSettings*> waves;
    };
}

#endif // !WAVES
