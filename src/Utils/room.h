#ifndef ROOM_H
#define ROOM_H

struct room{
    int id;
    int x,y,w,h;
    int parentId;

    inline room() = default;
    inline room(int id, int parentId, int x, int y, int w, int h):
        id(id), parentId(parentId), x(x), y(y), w(w), h(h) {}

};


#endif