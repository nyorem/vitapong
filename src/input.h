#ifndef _INPUT_H_
#define _INPUT_H_

#include <psp2/ctrl.h>
#include <psp2/touch.h>

#include "utils.h"

struct InputState {
    InputState () {
        firstUpdate = true;

        // Enable analog sticks
        sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

        // Enable front and back touchscreen
        sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
        sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, 1);

        memset(&pad, 0, sizeof(pad));
        memset(&oldpad, 0, sizeof(oldpad));
        memset(&touchpad_front, 0, sizeof(touchpad_front));
        memset(&touchpad_back, 0, sizeof(touchpad_back));
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

        if (firstUpdate) {
            memcpy(&oldpad, &pad, sizeof(pad));
            firstUpdate = false;
        }
    }

    void endUpdate () {
        memcpy(&oldpad, &pad, sizeof(pad));
    }

    bool isButtonPressed (int button) const {
        return pad.buttons & button;
    }

    bool isButtonPressedOnce (int button) const {
        return isButtonPressed(button) && !(oldpad.buttons & button);
    }

    bool isButtonReleased (int button) const {
        return ! isButtonPressed(button);
    }

    int numberTouches (int touchpad) const {
        return touchpad == 0 ? touchpad_front.reportNum : touchpad_back.reportNum;
    }

    bool isTouchpadActive (int touchpad) const {
        return touchpad == 0 ? (touchpad_front.reportNum > 0) : (touchpad_back.reportNum > 0);
    }

    Vec2f getTouchpadFront (int i = 0) const {
        float x = lerp(touchpad_front.report[0].x, TOUCHPAD_FRONT_W, SCREEN_W),
              y = lerp(touchpad_front.report[0].y, TOUCHPAD_FRONT_H, SCREEN_H);
        return Vec2f(x, y);
    }

    Vec2f getTouchpadBack (int i = 0) const {
        float x = lerp(touchpad_back.report[0].x, TOUCHPAD_BACK_W, SCREEN_W),
              y = lerp(touchpad_back.report[0].y, TOUCHPAD_BACK_H, SCREEN_H);
        return Vec2f(x, y);
    }

    bool firstUpdate = true;
    SceCtrlData oldpad, pad;
    SceTouchData touchpad_front, touchpad_back;
    signed char lx, ly, rx, ry;
};

#endif

