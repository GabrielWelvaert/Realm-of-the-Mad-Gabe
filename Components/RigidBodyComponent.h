#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H
#include "../../libs/glm/glm.hpp"

struct RidigBodyComponent {
    glm::vec2 velocity;
    
    inline RidigBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0)){
        this->velocity = velocity;
    }

    inline RidigBodyComponent(int x){
        this->velocity.x = x;
        this->velocity.y = x;
    }

    inline RidigBodyComponent(int x, int y){
        this->velocity.x = x;
        this->velocity.y = y;
    }

    inline RidigBodyComponent(float x, float y){
        this->velocity.x = x;
        this->velocity.y = y;
    }

    inline RidigBodyComponent(float x){
        this->velocity.x = x;
        this->velocity.y = x;
    }

};

#endif