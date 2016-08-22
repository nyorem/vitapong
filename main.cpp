#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include "constants.h"
#include "utils.h"

struct Sprite {
    Sprite (float x0, float y0) : x(x0), y(y0) {
    }

    virtual void render (uint32_t colour) const = 0;

    void move (float dx, float dy) {
        x += dx;
        y += dy;
    }

    void moveX (float dx) {
        move(dx, 0.0f);
    }

    void moveY (float dy) {
        move(0.0f, dy);
    }

    float x, y;
};

struct Rectangle : Sprite {
    Rectangle (float x0, float y0, float w, float h) : Sprite(x0, y0), w(w), h(h) {
    }

    void render (uint32_t colour) const {
        vita2d_draw_rectangle(x, y, w, h, colour);
    }

    float w, h;
};

struct Paddle : Rectangle {
    Paddle (float x0, float y0, float w, float h) : Rectangle(x0, y0, w, h) {
    }

    bool player;
};

struct Circle : Sprite {
    Circle (float x0, float y0, float r) : Sprite(x0, y0), r(r) {
    }

    void render (uint32_t colour) const {
        vita2d_draw_fill_circle(x, y, r, colour);
    }

    float r;
};

struct Ball : Circle {
    Ball (float x0, float y0, float r) : Circle(x0, y0, r) {
    }
};

struct Vec2 {
    Vec2 (float x, float y) : x(x), y(y) {
    }

    float x, y;
};

struct InputState {
    InputState () {
        // Enable analog stick
        sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

        // Enable front and back touchscreen
        sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
        sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, 1);

        memset(&pad, 0, sizeof(pad));
    }

    void update () {
        // Read controls, touchscreens and analog sticks
        sceCtrlPeekBufferPositive(0, &pad, 1);

        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touchpad_front, 1);
        sceTouchPeek(SCE_TOUCH_PORT_BACK, &touchpad_back, 1);

        lx = (signed char) pad.lx - 128;
        ly = (signed char) pad.ly - 128;
        rx = (signed char) pad.rx - 128;
        ry = (signed char) pad.ry - 128;
    }

    bool isButtonPressed (int button) {
        return pad.buttons & button;
    }

    bool isButtonReleased (int button) {
        return ! isButtonPressed(button);
    }

    bool isTouchpadActive (int touchpad) {
        return touchpad == 0 ? (touchpad_front.reportNum > 0) : (touchpad_back.reportNum > 0);
    }

    Vec2 getTouchpadFront () {
        // Front touchscreen: 1920x1088
        float x = lerp(touchpad_front.report[0].x, 1920, SCREEN_W),
              y = lerp(touchpad_front.report[0].y, 1088, SCREEN_H);
        return Vec2(x, y);
    }

    SceCtrlData pad;
    SceTouchData touchpad_front, touchpad_back;
    signed char lx, ly, rx, ry;
};

int main (void) {
    // Intialization
    vita2d_init();

    vita2d_set_clear_color(BLACK);

    // Load PGF font
    vita2d_pgf* pgf = vita2d_load_default_pgf();

    InputState input;

    // FPS counting
    SceUInt64 cur_micros = 0, delta_micros = 0, last_micros = 0;
    uint32_t frames = 0;
    float fps = 0.0f;

    // Ball
    Ball ball(SCREEN_W / 2, SCREEN_H / 2, 10);

    // Paddles
    float square_w = 20, square_h = 120;
    float square_speed = 2;

    Paddle player(10,
                  SCREEN_H / 2 - square_h / 2,
                  square_w, square_h);
    player.player = true;

    Paddle cpu(SCREEN_W - 10 - square_w,
               SCREEN_H / 2 - square_h / 2,
               square_w, square_h);
    cpu.player = false;

    while (1) {
        // Prevent the screen from turning off
        sceKernelPowerTick(0);

        // Input
        input.update();

        // Exit with Start
        if (input.isButtonPressed(SCE_CTRL_START))
            break;

        // Move the paddle with the arrows
        if (input.isButtonPressed(SCE_CTRL_UP)) {
            player.moveY(-square_speed);
        } else if (input.isButtonPressed(SCE_CTRL_DOWN)) {
            player.moveY(+square_speed);
        }

        // Move the paddle with the left analog stick
        if (abs(input.ly) > 50) {
            player.moveY(square_speed * input.ly / 50.0f);
        }

        // Check collisions
        // Player
        // TODO

        // CPU
        // TODO

        // Ball
        // TODO

        // Render
        vita2d_start_drawing();
            vita2d_clear_screen();

            // TODO
            ball.render(WHITE);
            player.render(WHITE);
            cpu.render(WHITE);

            vita2d_pgf_draw_textf(pgf, SCREEN_W - 160, 30, GREEN, 1.0f, "FPS: %.2f", fps);
        vita2d_end_drawing();
        vita2d_swap_buffers();

        // Calculate FPS
        cur_micros = sceKernelGetProcessTimeWide();

        if (cur_micros >= (last_micros + 1000000)) {
            delta_micros = cur_micros - last_micros;
            last_micros = cur_micros;
            fps = (frames/(double)delta_micros)*1000000.0f;
            frames = 0;
        }
        frames++;
    }
    vita2d_fini();

    // Cleanup
    vita2d_free_pgf(pgf);

    sceKernelExitProcess(0);

    return 0;
}
