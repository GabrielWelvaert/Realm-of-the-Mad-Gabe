#ifndef INTERACTUICOMPONENT_H
#define INTERACTUICOMPONENT_H

struct InteractUIComponent{
    int InteractUIId;
    inline InteractUIComponent() = default;
    inline InteractUIComponent(int InteractUIId): InteractUIId(InteractUIId) {}
};

#endif