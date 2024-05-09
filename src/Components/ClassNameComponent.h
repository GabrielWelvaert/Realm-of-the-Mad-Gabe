#ifndef CLASSNAMECOMPONENT_H
#define CLASSNAMECOMPONENT_H

#include "../Utils/enums.h"

struct ClassNameComponent{
    classes classname;

    inline ClassNameComponent() = default; // compiler wants a default!

    inline ClassNameComponent(classes classname): classname(classname) {}
};

#endif 