#ifndef OSCILLATINGPROJECTILECOMPONENT_H
#define OSCILLATINGPROJECTILECOMPONENT_H

#include "../../libs/glm/glm.hpp"

struct OscillatingProjectileComponent{

    float amplitude;
    float frequency;
    glm::vec2 linearPosition;
    bool inverse;

    inline OscillatingProjectileComponent() = default;

    // amplitude, freq
    inline OscillatingProjectileComponent(float amplitude, float frequency, glm::vec2 linearPosition, bool inverse = false):
        amplitude(amplitude), frequency(frequency), linearPosition(linearPosition), inverse(inverse) {}

};


#endif