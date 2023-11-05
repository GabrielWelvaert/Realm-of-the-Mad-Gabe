#include "CameraMovementSystem.h"

CameraMovementSystem::CameraMovementSystem(){
    RequireComponent<CameraFollowComponent>();
    RequireComponent<TransformComponent>();
}

void CameraMovementSystem::Update(SDL_Rect& camera, int mapheight, int mapwidth){
    for(auto entity: GetSystemEntities()){
        const auto& transform = entity.GetComponent<TransformComponent>();

        // change camera x and y based off of entity transform x y
        if (transform.position.x + (camera.w/2) < mapheight){ // previoulsy < Game::mapwidth 
            camera.x = transform.position.x - (camera.w/2 - 20);
        }
        if (transform.position.y + (camera.h/2) < mapwidth){ // previoulsy < Game::mapwidth 
            camera.y = transform.position.y - (camera.h/2 - 20);
            
        }
        // camera.x = camera.x < 0 ? 0 : camera.x; // if cam.x < 0 make it 0 otherwise leave it. 
        // camera.y = camera.y < 0 ? 0 : camera.y;
        // camera.x = camera.x > camera.w ? camera.w : camera.x;
        // camera.y = camera.y > camera.h ? camera.h : camera.y;

    }
}