#ifndef WAVES
#pragma once

#include <iostream>
#include <glm/glm.hpp>

namespace hiWave
{
    struct WaveSettings
    {
        glm::vec2 direction = glm::vec2(1.0f, 1.0f);
        float wavelength = 1.0f;
        float steepness = 1.0f;
        float speed = 1.0f;
        int detail = 2;
    };
}

#endif // !WAVES
