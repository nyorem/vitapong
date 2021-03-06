#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstdlib>

#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

static inline float clamp (float x, float m, float M) {
    if (x < m) {
        return m;
    } else if (x > M) {
        return M;
    } else {
        return x;
    }
}

static inline float rf (float a, float b) {
    float r = float(rand()) / RAND_MAX;
    return a + (b - a) * r;
}

static inline int ri (int a, int b) {
    return a + rand() % (b - a + 1);
}

#endif

