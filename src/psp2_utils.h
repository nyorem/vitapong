#ifndef _PSP2_UTILS_H_
#define _PSP2_UTILS_H_

#include <psp2/kernel/processmgr.h>

static inline void sleep (float sec) {
    sceKernelDelayThread(sec * 1000 * 1000);
}

#endif

