#pragma once
#include "SpaceObject.h"

class CelestialBody : public SpaceObject {
private:
    double gravityConstant;

public:
    CelestialBody(const Vector3D& pos, const Vector3D& vel, double m, bool isStat, double gConst = 6.67430e-11)
        : SpaceObject(pos, vel, m, isStat), gravityConstant(gConst) {}

    void UpdateState(double deltaTime, const Vector3D& totalForce) {
        if (isStatic) return;

        Vector3D acceleration = totalForce / mass;
        velocity = velocity + acceleration * (0.5 * deltaTime);
        position = position + velocity * deltaTime;
        velocity = velocity + acceleration * (0.5 * deltaTime);
    }

    Vector3D CalculateGravityForce(const Vector3D& targetPosition, double targetMass) const override {
        Vector3D direction = position - targetPosition;
        double distance = direction.Length();

        if (distance < 1e-3) {
            return Vector3D(0, 0, 0);
        }

        double forceMagnitude = (gravityConstant * mass * targetMass) / (distance * distance);
        return direction.Normalize() * forceMagnitude;
    }
};