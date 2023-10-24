#ifndef ITEMTABLECOMPONENT_H
#define ITEMTABLECOMPONENT_H

#include "../Utils/enums.h"
#include <vector>
#include <map>

struct ItemTableComponent{
    std::map<int, std::vector<items>> dropTable;
    bool hasAlreadySpawnedBag = false;

    inline ItemTableComponent() = default;

    inline ItemTableComponent(sprites spriteEnum){
        const auto& data = spriteEnumToItemTableComponentData.at(spriteEnum);
        this->dropTable = data.dropTable;
    }
};

#endif