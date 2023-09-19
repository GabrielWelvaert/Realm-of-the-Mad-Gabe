#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H
#include "../../libs/glm/glm.hpp"

struct RidigBodyComponent {
    glm::vec2 velocity;
    
    RidigBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0)){
        this->velocity = velocity;
    }

    RidigBodyComponent(int x){
        this->velocity.x = x;
        this->velocity.y = x;
    }

    RidigBodyComponent(int x, int y){
        this->velocity.x = x;
        this->velocity.y = y;
    }

    RidigBodyComponent(float x, float y){
        this->velocity.x = x;
        this->velocity.y = y;
    }

    RidigBodyComponent(float x){
        this->velocity.x = x;
        this->velocity.y = x;
    }

};

#endif