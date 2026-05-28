#pragma once
#include "SpaceObject.h"

class Spacecraft : public SpaceObject {
private:
    double dryMass;
    double fuelMass;
    double fuelConsumptionRate;
    double maxThrust;
    Vector3D thrustDirection;
    double throttle;

public:
    Spacecraft(const Vector3D& pos, const Vector3D& vel, double dryM, double initFuel,
               double maxThrustForce, const Vector3D& initThrustDir, double fuelRate)
        : SpaceObject(pos, vel, dryM + initFuel, false),
          dryMass(dryM), fuelMass(initFuel), maxThrust(maxThrustForce),
          thrustDirection(initThrustDir.Normalize()), fuelConsumptionRate(fuelRate), throttle(0.0) {

        if (dryM <= 0.0 || initFuel < 0.0 || fuelRate < 0.0 || maxThrustForce < 0.0) {
            throw std::invalid_argument("Spacecraft Error: Invalid physical constraints during initialization.");
        }
    }

    void SetThrustDirection(const Vector3D& dir) {
        thrustDirection = dir.Normalize();
    }

    void SetThrottle(double t) {
        if (t < 0.0) throttle = 0.0;
        else if (t > 1.0) throttle = 1.0;
        else throttle = t;
    }

    double GetFuelMass() const { return fuelMass; }

    void UpdateState(double deltaTime, const Vector3D& externalForces) override {
        if (deltaTime <= 0.0) return;
        Vector3D currentThrustForce(0, 0, 0);
        if (fuelMass > 0.0 && throttle > 0.0) {
            double consumedFuel = std::min(fuelConsumptionRate * throttle * deltaTime, fuelMass);
            fuelMass -= consumedFuel;
            mass = dryMass + fuelMass;

            double actualThrottle = (fuelConsumptionRate * deltaTime > 0) ? (consumedFuel / (fuelConsumptionRate * deltaTime)) : 0.0;
            currentThrustForce = thrustDirection * (maxThrust * actualThrottle);
        }
        Vector3D totalForce = externalForces + currentThrustForce;
        Vector3D acceleration = totalForce / mass;

        // ИНТЕГРАТОР ВЕРЛЕ
        velocity = velocity + acceleration * (0.5 * deltaTime);
        position = position + velocity * deltaTime;
        velocity = velocity + acceleration * (0.5 * deltaTime);
    }

    Vector3D CalculateGravityForce(const Vector3D& targetPosition, double targetMass) const override {
        return Vector3D(0, 0, 0);
    }
};