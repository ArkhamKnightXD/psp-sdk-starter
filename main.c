#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>

PSP_MODULE_INFO("gutest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT BUFFER_HEIGHT

char list[0x20000] __attribute__((aligned(64)));

int xPos = 32; // Initial x position of the square
int yPos = 32; // Initial y position of the square
int speed = 5; 

void initGu() {

    sceGuInit();

    //Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,(void*)0x88000,BUFFER_WIDTH);
    sceGuDepthBuffer((void*)0x110000,BUFFER_WIDTH);

    //Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Set some stuff
    sceGuDepthRange(65535, 0); //Use the full buffer for depth testing - buffer is reversed order

    sceGuDepthFunc(GU_GEQUAL); //Depth buffer is reversed, so GEQUAL instead of LEQUAL
    sceGuEnable(GU_DEPTH_TEST); //Enable depth testing

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu(){
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame(){
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0x00000000); // Black background
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame(){
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

typedef struct {
    unsigned short u, v;
    short x, y, z;
} Vertex;

void drawRect(float x, float y, float w, float h) {

    Vertex* vertices = (Vertex*)sceGuGetMemory(2 * sizeof(Vertex));

    vertices[0].x = x;
    vertices[0].y = y;

    vertices[1].x = x + w; // Adjusted x coordinate to include width
    vertices[1].y = y + h; // Adjusted y coordinate to include height

    sceGuColor(0xFFFFFFFF); // White, colors are ABGR
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
}

int main() {

    initGu();

    SceCtrlData pad;

    int running = 1;

    while(running){

        sceCtrlReadBufferPositive(&pad, 1);
        
        if (pad.Buttons & PSP_CTRL_UP && yPos > 0)
            yPos -= speed;

        else if (pad.Buttons & PSP_CTRL_DOWN && yPos < SCREEN_HEIGHT - 32)
            yPos += speed;

        else if (pad.Buttons & PSP_CTRL_LEFT && xPos > 0)
            xPos -= speed;

        else if (pad.Buttons & PSP_CTRL_RIGHT && xPos < SCREEN_WIDTH - 32)
            xPos +=speed;

        startFrame();

        drawRect(xPos, yPos, 32, 32); // Use xPos and yPos as the coordinates

        endFrame();

        sceKernelDelayThread(10000); // Delay for smoother animation, adjust as needed
    }

    return 0;
}
