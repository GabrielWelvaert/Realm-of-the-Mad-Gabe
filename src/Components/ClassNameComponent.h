#ifndef CLASSNAMECOMPONENT_H
#define CLASSNAMECOMPONENT_H

#include "../Utils/enums.h"

struct ClassNameComponent{
    classes classname;

    ClassNameComponent() : classname(EGG) {}; // compiler wants a default!

    ClassNameComponent(classes classname): classname(classname) {}
};

#endif 