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
        float speed = 1.0f;

        WaveSettings(glm::vec2 direction, float wavelength, float steepness, float speed) : direction(direction), wavelength(wavelength), steepness(steepness), speed(speed) {}
    };

    class WaveSystem
    {
    public:
        // Constructor & Deocntructor
        WaveSystem() : waveCount(0) {}
        ~WaveSystem();

        // Getters
        WaveSettings* getWave(int index) { return waves[index]; }
        int getWaveCount() { return waveCount; }

        // Functions
        void AddRandomWave();
        void AddWave(WaveSettings* wave);
        void RemoveWave(int index);
        void PassValues(shdr::Shader* shader);

    private:
        int waveCount;
        const int MAX_WAVES = 10;
        std::vector<WaveSettings*> waves;
    };
}

#endif // !WAVES
