#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H

#include <bitset>
#include "../../libs/SDL2/SDL.h"

// this enum is used to access bitset of KeyBoardInput struct
enum movementKeys {
    W,A,S,D,MB
};

enum utilityKeys {
    SPACE,SHIFT,H,T
};

struct KeyBoardInput{
    std::bitset<8> movementKeys; // W,A,S,D,MB
    std::bitset<8> utilityKeys; // SPACE,SHIFT,H,T
    std::bitset<8> inventoryUses; // keys 1-8
    Uint32 timeOfLastScroll = 0;
};

#endif