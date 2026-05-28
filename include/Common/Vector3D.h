#pragma once
#include <cmath>
#include <stdexcept>

class Vector3D {
public:
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vector3D() = default;
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3D& operator-=(const Vector3D& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector3D& operator+=(const Vector3D& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    Vector3D operator*(double scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(double scalar) const {
        if (std::abs(scalar) < 1e-9) {
            throw std::runtime_error("Vector3D Error: Division by zero or near-zero scalar.");
        }
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    double Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    double GetX() {
        return this->x;
    }

    double GetY() {
        return this->y;
    }

    Vector3D Normalize() const {
        double len = Length();
        if (len < 1e-9) {
            return Vector3D(0, 0, 0);
        }
        return *this / len;
    }
};