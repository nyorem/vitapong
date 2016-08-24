#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <cmath>

// TODO: small 2D geometry library

template < typename T >
struct Vec2 {
    Vec2 (T x, T y) : x(x), y(y) {
    }

    Vec2 () : Vec2(T(0), T(0)) {
    }

    Vec2<T>& operator+= (Vec2<T> const& u) {
        x += u.x;
        y += u.y;

        return *this;
    }

    Vec2<T>& operator-= (Vec2<T> const& u) {
        x -= u.x;
        y -= u.y;

        return *this;
    }

    // Length
    T squared_length () const {
        return x * x + y * y;
    }

    T length () const {
        return std::sqrt(squared_length());
    }

    T x, y;
};

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;
typedef Vec2<unsigned int> Vec2u;

// Multiplication by a scalar
template < typename T >
Vec2<T> operator* (T a, Vec2<T> const& v) {
    return Vec2<T>(a * v.x, a * v.y);
}

template < typename T >
Vec2<T> operator* (Vec2<T> const& v, T a) {
    return a * v;
}

// Addition
template < typename T >
Vec2<T> operator+ (Vec2<T> const& u, Vec2<T> const& v) {
    Vec2<T> tmp = u;
    tmp += v;

    return tmp;
}

// Subtraction
template < typename T >
Vec2<T> operator- (Vec2<T> const& u, Vec2<T> const& v) {
    Vec2<T> tmp = u;
    tmp -= v;

    return tmp;
}

// Dot product
template < typename T >
T operator* (Vec2<T> const& u, Vec2<T> const& v) {
    return u.x * v.x + u.y * v.y;
}

#endif

