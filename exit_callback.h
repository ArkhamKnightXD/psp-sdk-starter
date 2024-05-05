#ifndef EXIT_CALLBACK_H
#define EXIT_CALLBACK_H

#include <pspkernel.h>

// With this setup, you can include this header file in your main file, and the functions will be defined inline.
//However, remember that this approach may increase the size of your compiled code, as the functions will be duplicated wherever they are used. 
// Use this method judiciously, especially in embedded systems with limited memory.

//These 3 functions are for setting up the exit button.
static inline int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

static inline int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

static inline int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

#endif /* EXIT_CALLBACK_H */
