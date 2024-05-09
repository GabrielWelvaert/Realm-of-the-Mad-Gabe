#ifndef ROTATIONSYSTEM_H
#define ROTATIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RotationComponent.h"

class RotationSystem: public System{
    private:

    public:
        RotationSystem();
        void Update(const double& deltaTime);


};

#endif