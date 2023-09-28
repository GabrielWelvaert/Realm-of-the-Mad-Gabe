#pragma once 

#include "../Utils/enums.h"
#include <vector>
#include <map>

struct ItemTableComponent{
    std::map<int, std::vector<items>> dropTable;
    bool hasAlreadySpawnedBag = false;

    ItemTableComponent() = default;

    ItemTableComponent(sprites spriteEnum){
        const auto& data = spriteEnumToItemTableComponentData.at(spriteEnum);
        this->dropTable = data.dropTable;
    }
};