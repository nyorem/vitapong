#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <vita2d.h>

#include "geometry.h"

struct Sprite {
    Sprite (float x0, float y0) : p(x0, y0) {
    }

    Sprite (Vec2f const& p) : p(p) {
    }

    virtual void render (uint32_t colour) const = 0;

    void move (float dx, float dy) {
        p.x += dx;
        p.y += dy;
    }

    void move (Vec2f const& dp) {
        p += dp;
    }

    void moveX (float dx) {
        move(dx, 0.0f);
    }

    void moveY (float dy) {
        move(0.0f, dy);
    }

    float x () const {
        return p.x;
    }

    float y () const {
        return p.y;
    }

    float& x () {
        return p.x;
    }

    float& y () {
        return p.y;
    }

    // AABB
    virtual float left () const = 0;
    virtual float right () const = 0;
    virtual float top () const = 0;
    virtual float bottom () const = 0;

    bool intersects (Sprite const& other) const {
        return right()  >= other.left() && left() <= other.right() &&
               bottom() >= other.top()  && top()  <= other.bottom();
    }

    Vec2f p;
};

struct Rectangle : Sprite {
    Rectangle (float x0, float y0, float w, float h) : Sprite(x0, y0), dims(w, h) {
    }

    Rectangle (Vec2f const& p0, float w, float h) : Rectangle(p0.x, p0.y, w, h) {
    }

    Rectangle (Vec2f const& p0, Vec2f const& dims0) : Rectangle(p0, dims0.x, dims0.y) {
    }

    void render (uint32_t colour) const {
        vita2d_draw_rectangle(x(), y(), width(), height(), colour);
    }

    float width () const {
        return dims.x;
    }

    float height () const {
        return dims.y;
    }

    float left () const {
        return x();
    }

    float right () const {
        return x() + width();
    }

    float top () const {
        return y();
    }

    float bottom () const {
        return y() + height();
    }


    Vec2f dims;
};

struct Circle : Sprite {
    Circle (float x0, float y0, float r) : Sprite(x0, y0), r(r) {
    }

    Circle (Vec2f const& p0, float r) : Circle(p0.x, p0.y, r) {
    }

    void render (uint32_t colour) const {
        vita2d_draw_fill_circle(x(), y(), r, colour);
    }

    float radius () const {
        return r;
    }

    float left () const {
        return x();
    }

    float right () const {
        return x() + 2 * radius();
    }

    float top () const {
        return y();
    }

    float bottom () const {
        return y() + 2 * radius();
    }

    float r;
};

#endif

