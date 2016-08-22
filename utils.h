#ifndef _UTILS_H_
#define _UTILS_H_

#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)

static inline float clamp (float x, float m, float M) {
    if (x < m) {
        return m;
    } else if (x > M) {
        return M;
    } else {
        return x;
    }
}

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

#endif

