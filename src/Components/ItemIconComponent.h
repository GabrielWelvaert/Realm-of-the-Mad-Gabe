#ifndef ITEMICONCOMPONENT_H
#define ITEMICONCOMPONENT_H

#include "../../libs/glm/glm.hpp"

struct ItemIconComponent{
    glm::vec2 position;
    glm::vec2 widthHeight;
    textureEnums textureEnum;
    inline ItemIconComponent() = default;
    inline ItemIconComponent(const glm::vec2& position, const glm::vec2 widthHeight, const textureEnums& textureEnum):
        position(position), widthHeight(widthHeight), textureEnum(textureEnum) {}
};

#endif