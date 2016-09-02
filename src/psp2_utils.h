#ifndef _PSP2_UTILS_H_
#define _PSP2_UTILS_H_

static inline void sleep (float sec) {
    sceKernelDelayThread(sec * 1000 * 1000);
}

#endif

