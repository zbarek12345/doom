// vec2.h
#ifndef DOOM_VEC2_H
#define DOOM_VEC2_H

#include <cmath>
#include <algorithm>
#include <ostream>
#include <cassert>
#include <cstdint>

template<typename T>
struct vec2 {
    T x, y;

    // constructors
    constexpr vec2() noexcept = default;
    constexpr vec2(T x, T y) noexcept : x(x), y(y) {}
    explicit constexpr vec2(T s) noexcept : x(s), y(s) {}

    // copy / move - note: copy assignment is NOT constexpr
    constexpr vec2(const vec2&) noexcept = default;
    vec2& operator=(const vec2&) noexcept = default;           // <-- fixed

    constexpr vec2(vec2&&) noexcept = default;
    vec2& operator=(vec2&&) noexcept = default;

    template<typename U>
    explicit constexpr vec2(const vec2<U>& v) noexcept
        : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

    // access
    constexpr const T& operator[](size_t i) const noexcept { assert(i<2); return (&x)[i]; }

    constexpr vec2 yx() const noexcept { return {y, x}; }

    // comparison
    constexpr bool operator==(const vec2& o) const noexcept;
    constexpr bool operator!=(const vec2& o) const noexcept;

    // arithmetic
    constexpr vec2 operator+(const vec2& o) const noexcept { return {x+o.x, y+o.y}; }
    constexpr vec2 operator-(const vec2& o) const noexcept { return {x-o.x, y-o.y}; }
    constexpr vec2 operator*(const vec2& o) const noexcept { return {x*o.x, y*o.y}; }
    constexpr vec2 operator/(const vec2& o) const noexcept { return {x/o.x, y/o.y}; }

    constexpr vec2 operator*(T s) const noexcept { return {x*s, y*s}; }
    constexpr vec2 operator/(T s) const noexcept { return {x/s, y/s}; }
    constexpr vec2 operator-() const noexcept { return {-x, -y}; }

    vec2& operator+=(const vec2& o) noexcept { x+=o.x; y+=o.y; return *this; }
    vec2& operator-=(const vec2& o) noexcept { x-=o.x; y-=o.y; return *this; }
    vec2& operator*=(const vec2& o) noexcept { x*=o.x; y*=o.y; return *this; }
    vec2& operator/=(const vec2& o) noexcept { x/=o.x; y/=o.y; return *this; }
    vec2& operator*=(T s) noexcept { x*=s; y*=s; return *this; }
    vec2& operator/=(T s) noexcept { x/=s; y/=s; return *this; }

    // vector ops
    constexpr T length_sq() const noexcept { return x*x + y*y; }
    T length() const noexcept { return std::sqrt(length_sq()); }

    vec2 normalized() const noexcept {
        T len = length();
        return len > T(0) ? *this / len : vec2{};
    }
    void normalize() noexcept { *this = normalized(); }

    constexpr T dot(const vec2& o) const noexcept { return x*o.x + y*o.y; }
    constexpr vec2 perpendicular() const noexcept { return {-y, x}; }      // 90° clockwise
    constexpr vec2 perpendicular_ccw() const noexcept { return {y, -x}; } // 90° counter-clockwise

    T angle() const noexcept { return std::atan2(y, x); }
    vec2 rotated(T radians) const noexcept {
        T c = std::cos(radians), s = std::sin(radians);
        return {x*c - y*s, x*s + y*c};
    }

    vec2 lerp(const vec2& o, T t) const noexcept { return *this*(T(1)-t) + o*t; }
    T distance_to(const vec2& o) const noexcept { return (*this - o).length(); }

    // constants
    static const vec2 zero;
    static const vec2 one;
    static const vec2 up;
    static const vec2 down;
    static const vec2 right;
    static const vec2 left;
};

template<typename T>
constexpr bool vec2<T>::operator==(const vec2 &o) const noexcept {
    return x==o.x && y==o.y;
}

template<typename T>
constexpr bool vec2<T>::operator!=(const vec2 &o) const noexcept {
    return !(*this == o);
}

template<typename T> const vec2<T> vec2<T>::zero{0,0};
template<typename T> const vec2<T> vec2<T>::one{1,1};
template<typename T> const vec2<T> vec2<T>::up{0,1};
template<typename T> const vec2<T> vec2<T>::down{0,-1};
template<typename T> const vec2<T> vec2<T>::right{1,0};
template<typename T> const vec2<T> vec2<T>::left{-1,0};

template<typename T>
constexpr vec2<T> operator*(T s, const vec2<T>& v) noexcept { return v*s; }

template<typename T>
std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
    return os << '(' << v.x << ", " << v.y << ')';
}

// Common typedefs
using fvec2 = vec2<float>;
using dvec2 = vec2<double>;
using ivec2 = vec2<int32_t>;
using uvec2 = vec2<uint32_t>;
using svec2 = vec2<int16_t>;
using usvec2 = vec2<uint16_t>;

#endif // DOOM_VEC2_H