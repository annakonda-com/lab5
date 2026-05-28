#pragma once
#include "../Common/Vector3D.h"

class SpaceObject {
protected:
    Vector3D position;
    Vector3D velocity;
    double mass;
    bool isStatic;

public:
    SpaceObject(const Vector3D& pos, const Vector3D& vel, double m, bool isStat)
        : position(pos), velocity(vel), mass(m), isStatic(isStat) {
        if (m <= 0.0) {
            throw std::invalid_argument("SpaceObject Error: Mass must be strictly positive.");
        }
    }

    virtual ~SpaceObject() = default;

    Vector3D GetPosition() const { return position; }
    Vector3D GetVelocity() const { return velocity; }
    double GetMass() const { return mass; }
    bool IsStatic() const { return isStatic; }

    void SetPosition(const Vector3D& pos) { position = pos; }
    void SetVelocity(const Vector3D& vel) { velocity = vel; }

    virtual void UpdateState(double deltaTime, const Vector3D& totalForce) = 0;

    virtual Vector3D CalculateGravityForce(const Vector3D& targetPosition, double targetMass) const = 0;
};