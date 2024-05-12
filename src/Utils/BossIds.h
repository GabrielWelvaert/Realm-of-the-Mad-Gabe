#ifndef BOSSIDS_H
#define BOSSIDS_H

struct BossIds{
    int Id;
    unsigned int CreationId;
    BossIds(int Id, unsigned int CreationId): Id(Id), CreationId(CreationId) {}
};

#endif