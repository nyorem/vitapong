#ifndef _INPUT_H_
#define _INPUT_H_

#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <glm/glm.hpp>
#include "utils.h" // for lerp

struct InputState {
    InputState () {
        firstUpdate = true;

        // Enable analog sticks
        sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

        // Enable front and back touchscreen
        sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
        sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

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

    glm::vec2 getTouchpadFront (int i = 0) const {
        float x = lerp(touchpad_front.report[i].x, TOUCHPAD_FRONT_W, SCREEN_W),
              y = lerp(touchpad_front.report[i].y, TOUCHPAD_FRONT_H, SCREEN_H);
        return glm::vec2(x, y);
    }

    glm::vec2 getTouchpadBack (int i = 0) const {
        float x = lerp(touchpad_back.report[i].x, TOUCHPAD_BACK_W, SCREEN_W),
              y = lerp(touchpad_back.report[i].y, TOUCHPAD_BACK_H, SCREEN_H);
        return glm::vec2(x, y);
    }

    ~InputState () {
        // Enable front and back touchscreen
        sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_STOP);
        sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_STOP);
    }

    bool firstUpdate = true;
    SceCtrlData oldpad, pad;
    SceTouchData touchpad_front, touchpad_back;
    signed char lx, ly, rx, ry;
};

#endif

