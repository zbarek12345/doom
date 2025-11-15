// vec2.h
#ifndef DOOM_VEC2_H
#define DOOM_VEC2_H

#include <cmath>
#include <algorithm>
#include <ostream>
#include <cassert>
#include <cstdint>

template<typename T>
class vec2 {
public:
    T x, y;

    // =============================================================
    // Constructors
    // =============================================================
    constexpr vec2() noexcept : x(0), y(0) {}
    constexpr vec2(T x, T y) noexcept : x(x), y(y) {}
    explicit constexpr vec2(T s) noexcept : x(s), y(s) {}

    // Copy / Move
    constexpr vec2(const vec2&) noexcept = default;
    constexpr vec2& operator=(const vec2&) noexcept = default;
    constexpr vec2(vec2&&) noexcept = default;
    constexpr vec2& operator=(vec2&&) noexcept = default;

    // Convert from different type
    template<typename U>
    explicit constexpr vec2(const vec2<U>& v) noexcept : x(static_cast<T>(v.x)),
                                                          y(static_cast<T>(v.y)) {}

    // =============================================================
    // Element access
    // =============================================================
    constexpr const T& operator[](size_t i) const {
        assert(i < 2);
        return (&x)[i];
    }

    // Swizzling
    constexpr vec2 yx() const noexcept { return vec2(y, x); }

    // =============================================================
    // Comparison
    // =============================================================
    constexpr bool operator==(const vec2& other) const noexcept {
        return x == other.x && y == other.y;
    }
    constexpr bool operator!=(const vec2& other) const noexcept {
        return !(*this == other);
    }

    // =============================================================
    // Arithmetic operators
    // =============================================================
    constexpr vec2 operator+(const vec2& other) const noexcept {
        return {x + other.x, y + other.y};
    }
    constexpr vec2 operator-(const vec2& other) const noexcept {
        return {x - other.x, y - other.y};
    }
    constexpr vec2 operator*(const vec2& other) const noexcept {  // component-wise
        return {x * other.x, y * other.y};
    }
    constexpr vec2 operator/(const vec2& other) const noexcept {
        return {x / other.x, y / other.y};
    }

    constexpr vec2 operator*(T s) const noexcept { return {x * s, y * s}; }
    constexpr vec2 operator/(T s) const noexcept { return {x / s, y / s}; }
    constexpr vec2 operator-() const noexcept { return {-x, -y}; }

    // Compound assignment
    vec2& operator+=(const vec2& v) noexcept { x += v.x; y += v.y; return *this; }
    vec2& operator-=(const vec2& v) noexcept { x -= v.x; y -= v.y; return *this; }
    vec2& operator*=(const vec2& v) noexcept { x *= v.x; y *= v.y; return *this; }
    vec2& operator/=(const vec2& v) noexcept { x /= v.x; y /= v.y; return *this; }
    vec2& operator*=(T s) noexcept { x *= s; y *= s; return *this; }
    vec2& operator/=(T s) noexcept { x /= s; y /= s; return *this; }

    // =============================================================
    // Vector operations
    // =============================================================
    constexpr T length_sq() const noexcept { return x*x + y*y; }
    T length() const noexcept { return std::sqrt(length_sq()); }

    vec2 normalized() const noexcept {
        T len = length();
        return len > T(0) ? (*this / len) : vec2(T(0));
    }
    void normalize() noexcept { *this = normalized(); }

    constexpr T dot(const vec2& other) const noexcept {
        return x*other.x + y*other.y;
    }

    // Perpendicular (clockwise 90°)
    constexpr vec2 perpendicular() const noexcept { return {-y, x}; }
    // Counter-clockwise 90°
    constexpr vec2 perpendicular_ccw() const noexcept { return {y, -x}; }

    // Signed angle from this to other (radians)
    T angle_to(const vec2& other) const noexcept {
        return std::atan2(other.y, other.x) - std::atan2(y, x);
    }

    // Angle of this vector from positive X axis
    T angle() const noexcept {
        return std::atan2(y, x);
    }

    // Rotate by angle (radians)
    vec2 rotated(T angle) const noexcept {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return {x*c - y*s, x*s + y*c};
    }

    void rotate(T angle) noexcept { *this = rotated(angle); }

    // Linear interpolation
    vec2 lerp(const vec2& other, T t) const noexcept {
        return *this * (T(1) - t) + other * t;
    }

    // Distance
    T distance_to(const vec2& other) const noexcept { return (*this - other).length(); }

    // Project onto another vector
    vec2 project_onto(const vec2& onto) const noexcept {
        return onto * (dot(onto) / onto.length_sq());
    }

    // Reflect over a normal
    vec2 reflect(const vec2& normal) const noexcept {
        return *this - normal * (T(2) * dot(normal));
    }

    // =============================================================
    // Utility
    // =============================================================
    constexpr bool is_zero(T eps = T(1e-6)) const noexcept {
        return std::abs(x) <= eps && std::abs(y) <= eps;
    }

    // =============================================================
    // Static constants
    // =============================================================
    static const vec2 zero;
    static const vec2 one;
    static const vec2 up;
    static const vec2 down;
    static const vec2 right;
    static const vec2 left;
};

// Static constants
template<typename T> const vec2<T> vec2<T>::zero(0, 0);
template<typename T> const vec2<T> vec2<T>::one(1, 1);
template<typename T> const vec2<T> vec2<T>::up(0, 1);
template<typename T> const vec2<T> vec2<T>::down(0, -1);
template<typename T> const vec2<T> vec2<T>::right(1, 0);
template<typename T> const vec2<T> vec2<T>::left(-1, 0);

// Scalar on left
template<typename T>
constexpr vec2<T> operator*(T s, const vec2<T>& v) noexcept { return v * s; }

// Stream output
template<typename T>
std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

// Common typedefs
using vec2f = vec2<float>;
using vec2d = vec2<double>;
using vec2i = vec2<int32_t>;
using vec2u = vec2<uint32_t>;

#endif // DOOM_VEC2_H