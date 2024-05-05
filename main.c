#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include "exit_callback.h" // Include the header file where setup_callback is declared
#include "psp_graphics.h"

//This 2 lines are necessary to setup psp module info.
PSP_MODULE_INFO("gutest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

char list[0x20000] __attribute__((aligned(64)));

void startFrame()
{
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0x00000000); // Black background
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame()
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

typedef struct
{
    unsigned short u, v;
    short x, y, z;
} Vertex;

void drawRect(float x, float y, float w, float h)
{

    Vertex *vertices = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));

    vertices[0].x = x;
    vertices[0].y = y;

    vertices[1].x = x + w; // Adjusted x coordinate to include width
    vertices[1].y = y + h; // Adjusted y coordinate to include height

    sceGuColor(0xFFFFFFFF); // White, colors are ABGR
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
}

typedef struct
{
    int x, y;
    int w, h;
} Rectangle;

_Bool hasCollision(Rectangle player, Rectangle ball)
{
    return player.x < ball.x + ball.w && player.x + player.w > ball.x &&
           player.y < ball.y + ball.h && player.y + player.h > ball.y;
}

int main()
{
    setup_callbacks();

    initGu(list);

    Rectangle player1 = {8, SCREEN_HEIGHT / 2 - 48, 8, 48};
    Rectangle player2 = {SCREEN_WIDTH - 16, SCREEN_HEIGHT / 2 - 48, 8, 48};

    Rectangle ball = {SCREEN_WIDTH / 2 - 16, SCREEN_HEIGHT / 2 - 16, 16, 16};

    int playerSpeed = 6;

    int ballVelocityX = 4;
    int ballVelocityY = 4;

    SceCtrlData pad;

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_UP && player1.y > 0)
            player1.y -= playerSpeed;

        else if (pad.Buttons & PSP_CTRL_DOWN && player1.y < SCREEN_HEIGHT - 48)
            player1.y += playerSpeed;

        if (pad.Buttons & PSP_CTRL_TRIANGLE && player2.y > 0)
            player2.y -= playerSpeed;

        else if (pad.Buttons & PSP_CTRL_CROSS && player2.y < SCREEN_HEIGHT - 48)
            player2.y += playerSpeed;

        if (ball.y < 0 || ball.y > SCREEN_HEIGHT - 16)
            ballVelocityY *= -1;
        
        if (ball.x > SCREEN_WIDTH + 16 || ball.x < -16)
        {
            ball.x = SCREEN_WIDTH / 2 - 16;
            ball.y = SCREEN_HEIGHT / 2 - 16;
        }

        if (hasCollision(player1, ball) || hasCollision(player2, ball))
            ballVelocityX *= -1;
        
        ball.x += ballVelocityX;
        ball.y += ballVelocityY;

        startFrame();

        drawRect(player1.x, player1.y, player1.w, player1.h); // Use xPos and yPos as the coordinates

        drawRect(ball.x, ball.y, ball.w, ball.h);

        drawRect(player2.x, player2.y, player2.w, player2.h);

        endFrame();

        sceKernelDelayThread(10000); // Delay for smoother animation, adjust as needed
    }

    endGu();
    return 0;
}
