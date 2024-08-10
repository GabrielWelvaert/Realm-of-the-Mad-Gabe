#ifndef ISSHOOTINGCOMPONENT_H
#define ISSHOOTINGCOMPONENT_H

struct isShootingComponent{
    bool isShooting = false;
    isShootingComponent() = default;
    isShootingComponent(bool shooting): isShooting(shooting) {
    }
};

#endif