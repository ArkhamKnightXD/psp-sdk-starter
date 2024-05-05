#ifndef PSP_GRAPHICS_H
#define PSP_GRAPHICS_H

#include <pspgu.h>

//The buffer is 512, because the buffer size needs to be the closest power of 2 to the psp resolution,
// and because has to be bigger than 480, is 512 instead of 256  
#define BUFFER_WIDTH 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

//boilerplate code for setting up the psp graphics.
static inline void initGu(char *list) {

    sceGuInit();

    // Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, (void *)0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void *)0x88000, BUFFER_WIDTH);
    sceGuDepthBuffer((void *)0x110000, BUFFER_WIDTH);

    // Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Set some stuff
    sceGuDepthRange(65535, 0); // Use the full buffer for depth testing - buffer is reversed order

    sceGuDepthFunc(GU_GEQUAL);  // Depth buffer is reversed, so GEQUAL instead of LEQUAL
    sceGuEnable(GU_DEPTH_TEST); // Enable depth testing

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

// closing psp graphics.
static inline void endGu()
{
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

#endif /* PSP_GRAPHICS_H */
