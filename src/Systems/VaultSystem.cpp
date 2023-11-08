#include "VaultSystem.h"

VaultSystem::VaultSystem(){
    RequireComponent<VaultChestComponent>();
}

// The character manager calls registry->GetSystem<VaultSystem>().GetSystemEntities()