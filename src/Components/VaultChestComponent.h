#ifndef VAULTCHESTCOMPONENT_H
#define VAULTCHESTCOMPONENT_H

struct VaultChestComponent{
    int id;
    inline VaultChestComponent() = default;
    inline VaultChestComponent(int id): id(id) {}
};

#endif