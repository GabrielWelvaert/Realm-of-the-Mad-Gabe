#ifndef VAULTITEMKILLER_H
#define VAULTITEMKILLER_H

/*
Registry->KillAllEntities() failed to kill vault items
This is because, unless vault chest is viewed, the items will not have a MouseBoxComponent, and thus
wont have a component signature that matches any system. This system exists solely to keep track of these items
It exists solely to kill items. It will also kill lootbag items which may have avoided their demise.
*/

#include "../ECS/ECS.h"
#include "../Components/ItemComponent.h"

class VaultItemSystem: public System{
    public:
        VaultItemSystem();

};

#endif