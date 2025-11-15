#ifndef DOOM_VEC_H
#define DOOM_VEC_H

#include <cmath>
#include <algorithm>
#include <ostream>
#include <cassert>
#include <cstdint>

template<typename T>
class vec3 {
public:
    T x, y, z;

    // =============================================================
    // Constructors
    // =============================================================

    constexpr vec3() noexcept : x(0), y(0), z(0) {}
    constexpr vec3(T x, T y, T z) noexcept : x(x), y(y), z(z) {}
    explicit vec3(T s) noexcept : x(s), y(s), z(s) {}

    // Copy constructor & assignment (default is fine)
    constexpr vec3(const vec3&) noexcept = default;
    constexpr vec3& operator=(const vec3&) noexcept = default;

    // Move constructor & assignment (default is fine)
    constexpr vec3(vec3&&) noexcept = default;
    constexpr vec3& operator=(vec3&&) noexcept = default;

    // =============================================================
    // Conversion from different type
    // =============================================================
    template<typename U>
    explicit constexpr vec3(const vec3<U>& v) noexcept : x(static_cast<T>(v.x)),
                                                          y(static_cast<T>(v.y)),
                                                          z(static_cast<T>(v.z)) {}

    // =============================================================
    // Element access
    // =============================================================
    constexpr const T& operator[](size_t i) const {
        assert(i < 3);
        return (&x)[i];
    }

    // Swizzling (common useful ones)
    constexpr vec3 yzx() const noexcept { return vec3(y, z, x); }
    constexpr vec3 zxy() const noexcept { return vec3(z, x, y); }
    constexpr vec3 xyz() const noexcept { return *this; }

    // =============================================================
    // Comparison operators
    // =============================================================
    constexpr bool operator==(const vec3& other) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }
    constexpr bool operator!=(const vec3& other) const noexcept {
        return !(*this == other);
    }

    // =============================================================
    // Arithmetic operators
    // =============================================================
    constexpr vec3 operator+(const vec3& other) const noexcept {
        return vec3(x + other.x, y + other.y, z + other.z);
    }
    constexpr vec3 operator-(const vec3& other) const noexcept {
        return vec3(x - other.x, y - other.y, z - other.z);
    }
    constexpr vec3 operator*(const vec3& other) const noexcept {  // component-wise
        return vec3(x * other.x, y * other.y, z * other.z);
    }
    constexpr vec3 operator/(const vec3& other) const noexcept {
        return vec3(x / other.x, y / other.y, z / other.z);
    }

    constexpr vec3 operator*(T s) const noexcept {
        return vec3(x * s, y * s, z * s);
    }
    constexpr vec3 operator/(T s) const noexcept {
        return vec3(x / s, y / s, z / s);
    }
    constexpr vec3 operator-() const noexcept {
        return vec3(-x, -y, -z);
    }

    // Compound assignment
    vec3& operator+=(const vec3& other) noexcept { x += other.x; y += other.y; z += other.z; return *this; }
    vec3& operator-=(const vec3& other) noexcept { x -= other.x; y -= other.y; z -= other.z; return *this; }
    vec3& operator*=(const vec3& other) noexcept { x *= other.x; y *= other.y; z *= other.z; return *this; }
    vec3& operator/=(const vec3& other) noexcept { x /= other.x; y /= other.y; z /= other.z; return *this; }

    vec3& operator*=(T s) noexcept { x *= s; y *= s; z *= s; return *this; }
    vec3& operator/=(T s) noexcept { x /= s; y /= s; z /= s; return *this; }

    // =============================================================
    // Vector operations
    // =============================================================
    constexpr T length_sq() const noexcept { return x*x + y*y + z*z; }
    T length() const noexcept { return std::sqrt(length_sq()); }

    vec3 normalized() const noexcept {
        const T len = length();
        return (len > T(0)) ? (*this / len) : vec3(T(0));
    }

    void normalize() noexcept { *this = normalized(); }

    constexpr T dot(const vec3& other) const noexcept {
        return x*other.x + y*other.y + z*other.z;
    }

    constexpr vec3 cross(const vec3& other) const noexcept {
        return vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Static versions
    static constexpr T dot(const vec3& a, const vec3& b) noexcept { return a.dot(b); }
    static constexpr vec3 cross(const vec3& a, const vec3& b) noexcept { return a.cross(b); }

    // =============================================================
    // Utility functions
    // =============================================================
    constexpr bool is_zero(T epsilon = T(1e-6)) const noexcept {
        return std::abs(x) <= epsilon &&
               std::abs(y) <= epsilon &&
               std::abs(z) <= epsilon;
    }

    constexpr bool is_normalized(T epsilon = T(1e-4)) const noexcept {
        return std::abs(length_sq() - T(1)) <= epsilon;
    }

    vec3 lerp(const vec3& other, T t) const noexcept {
        return *this * (T(1) - t) + other * t;
    }

    vec3 slerp(const vec3& other, T t) const noexcept; // implemented below for float/double

    // Distance
    T distance_to(const vec3& other) const noexcept { return (*this - other).length(); }
    static T distance(const vec3& a, const vec3& b) noexcept { return a.distance_to(b); }

    // Angle between two vectors (in radians)
    T angle_to(const vec3& other) const noexcept {
        T d = dot(other);
        T len = length() * other.length();
        if (len == T(0)) return T(0);
        return std::acos(std::clamp(d / len, T(-1), T(1)));
    }

    // Project this vector onto another
    vec3 project_onto(const vec3& onto) const noexcept {
        return onto * (dot(onto) / onto.length_sq());
    }

    // Reflect this vector over a normal
    vec3 reflect(const vec3& normal) const noexcept {
        return *this - normal * (T(2) * dot(normal));
    }

    // Refract (for float/double only)
    vec3 refract(const vec3& normal, T eta) const noexcept; // implemented below

    // =============================================================
    // Constants
    // =============================================================
    static const vec3 zero;
    static const vec3 one;
    static const vec3 up;
    static const vec3 down;
    static const vec3 right;
    static const vec3 left;
    static const vec3 forward;
    static const vec3 back;
};

// Static constants definition
template<typename T> const vec3<T> vec3<T>::zero(0,0,0);
template<typename T> const vec3<T> vec3<T>::one(1,1,1);
template<typename T> const vec3<T> vec3<T>::up(0,1,0);
template<typename T> const vec3<T> vec3<T>::down(0,-1,0);
template<typename T> const vec3<T> vec3<T>::right(1,0,0);
template<typename T> const vec3<T> vec3<T>::left(-1,0,0);
template<typename T> const vec3<T> vec3<T>::forward(0,0,-1);
template<typename T> const vec3<T> vec3<T>::back(0,0,1);

// Free functions for scalar on left
template<typename T>
constexpr vec3<T> operator*(T s, const vec3<T>& v) noexcept { return v * s; }

// =============================================================
// Stream output
// =============================================================
template<typename T>
std::ostream& operator<<(std::ostream& os, const vec3<T>& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// =============================================================
// Specializations for float/double: slerp and refract
// =============================================================
template<typename T>
vec3<T> vec3<T>::slerp(const vec3<T>& other, T t) const noexcept {
    static_assert(std::is_floating_point_v<T>, "slerp only meaningful for floating point types");
    T d = dot(other);
    d = std::clamp(d, T(-1), T(1));
    T theta = std::acos(d);
    if (theta < T(1e-6)) return lerp(other, t);
    T sin_theta = std::sin(theta);
    T a = std::sin((T(1) - t) * theta) / sin_theta;
    T b = std::sin(t * theta) / sin_theta;
    return *this * a + other * b;
}

template<typename T>
vec3<T> vec3<T>::refract(const vec3<T>& normal, T eta) const noexcept {
    static_assert(std::is_floating_point_v<T>, "refract only meaningful for floating point");
    T n_dot_i = dot(normal);
    T k = T(1) - eta * eta * (T(1) - n_dot_i * n_dot_i);
    if (k < T(0)) return vec3<T>(0);
    return *this * eta - normal * (eta * n_dot_i + std::sqrt(k));
}

// Common typedefs
using fvec3 = vec3<float>;
using dvec3 = vec3<double>;
using ivec3 = vec3<int32_t>;
using uvec3 = vec3<uint32_t>;
using svec3 = vec3<int16_t>;

#endif // DOOM_VEC_H