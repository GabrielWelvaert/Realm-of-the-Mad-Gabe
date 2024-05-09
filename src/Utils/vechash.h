#ifndef VECHASH_H
#define VECHASH_H

#include <iostream>
#include "../../libs/glm/glm.hpp"

struct Vec2Comparator { // used in LoadTilemap algorithm
    bool operator()(const glm::vec2& a, const glm::vec2& b) const{
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

struct Vec2Hash { // used in LoadTileMap algorithm
    std::size_t operator()(const glm::vec2& v) const {
        return std::hash<float>{}(v.x) ^ (std::hash<float>{}(v.y) << 1);
    }
};


#endif