#ifndef DISPLAYSTATCOMPONENT_H
#define DISPLAYSTATCOMPONENT_H

#include "../Utils/enums.h"
#include <string>

struct DisplayStatComponent{
    std::string lastDisplayed;
    stats statEnum;
    DisplayStatComponent() = default;

    DisplayStatComponent(stats statEnum): statEnum(statEnum) {}
};

#endif