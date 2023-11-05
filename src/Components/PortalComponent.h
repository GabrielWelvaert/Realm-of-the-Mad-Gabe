#ifndef PORTALCOMPONENT_H
#define PORTALCOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"

struct PortalComponent{
    wallTheme area;
    inline PortalComponent() = default;
    inline PortalComponent(wallTheme area): area(area) {}
};

#endif