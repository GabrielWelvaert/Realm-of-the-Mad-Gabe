#ifndef DISPLAYSTATCOMPONENT_H
#define DISPLAYSTATCOMPONENT_H

#include "../Utils/enums.h"
#include <string>

struct DisplayStatComponent{
    std::string lastDisplayed;
    stats statEnum;
    inline DisplayStatComponent() = default;

    inline DisplayStatComponent(stats statEnum): statEnum(statEnum) {}
};

#endif