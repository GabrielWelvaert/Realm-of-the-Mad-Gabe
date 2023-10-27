#ifndef XOSHIRO256_H
#define XOSHIRO256_H

#include <chrono>
#include <cstdint> 
#include <unistd.h>

// fast RNG used in various places
// https://en.wikipedia.org/wiki/Xorshift#xoshiro256**

class Xoshiro256{ 
private:
	uint64_t s[4];
    
	inline uint64_t rotl(uint64_t x, int k){
        return (x << k) | (x >> (64 - k));
    }

    // declaring inline in header file: https://stackoverflow.com/a/1421730/20080198
    inline uint64_t next(){
        uint64_t const result = rotl(s[1] * 5, 7) * 9;
        uint64_t const t = s[1] << 17;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], 45);
        return result;
    }
    
public:
	inline Xoshiro256(){
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count() ^ getpid();
        s[0] = seed;
        s[1] = seed + 1;
        s[2] = seed + 2;
        s[3] = seed + 3;
        next();
        next();
    }

	inline int randomFromRange(int minX, int maxX){
        double normalized = static_cast<double>(next()) / UINT64_MAX;
        return static_cast<int>(minX + normalized * (maxX - minX + 1));
    }

};

#endif 