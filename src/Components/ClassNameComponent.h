#ifndef CLASSNAMECOMPONENT_H
#define CLASSNAMECOMPONENT_H

#include "../Utils/enums.h"

struct ClassNameComponent{
    classes classname;

    inline ClassNameComponent() : classname(EGG) {}; // compiler wants a default!

    inline ClassNameComponent(classes classname): classname(classname) {}
};

#endif 