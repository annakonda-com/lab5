#pragma once
#include "SpaceObject.h"

class Spacecraft : public SpaceObject {
private:
    double dryMass;              // Масса корабля без топлива
    double fuelMass;             // Текущая масса топлива
    double fuelConsumptionRate;  // Расход топлива (кг/с) при 100% тяге
    double maxThrust;            // Максимальная сила тяги двигателя (Ньютоны)
    Vector3D thrustDirection;    // Направление вектора тяги (нормализованный)
    double throttle;             // Уровень тяги (от 0.0 до 1.0)

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

    // Управление вектором тяги
    void SetThrustDirection(const Vector3D& dir) {
        thrustDirection = dir.Normalize();
    }

    // Управление мощностью двигателя (дроссель)
    void SetThrottle(double t) {
        if (t < 0.0) throttle = 0.0;
        else if (t > 1.0) throttle = 1.0;
        else throttle = t;
    }

    double GetFuelMass() const { return fuelMass; }

    void UpdateState(double deltaTime, const Vector3D& externalForces) override {
        if (deltaTime <= 0.0) {
            throw std::invalid_argument("Spacecraft Error: Delta time must be positive.");
        }

        Vector3D currentThrustForce(0, 0, 0);

        // Моделирование расхода топлива и реактивной силы
        if (fuelMass > 0.0 && throttle > 0.0) {
            double consumedFuel = fuelConsumptionRate * throttle * deltaTime;
            if (consumedFuel > fuelMass) {
                consumedFuel = fuelMass; // Топливо закончилось в процессе шага
            }

            fuelMass -= consumedFuel;
            mass = dryMass + fuelMass; // Динамическое изменение массы аппарата

            // Сила тяги пропорциональна эффективной работе двигателя на этом шаге
            double actualThrottle = (fuelConsumptionRate > 0) ? (consumedFuel / (fuelConsumptionRate * deltaTime)) : 0.0;
            currentThrustForce = thrustDirection * (maxThrust * actualThrottle);
        }

        // Полная результирующая сила = Сумма внешних сил (гравитация) + Тяга двигателя
        Vector3D totalForce = externalForces + currentThrustForce;

        // Расчет движения по Ньютону (масса гарантированно > 0, так как dryMass > 0)
        Vector3D acceleration = totalForce / mass;
        velocity = velocity + acceleration * deltaTime;
        position = position + velocity * deltaTime;
    }

    Vector3D CalculateGravityForce(const Vector3D& targetPosition, double targetMass) const override {
        // Корабль обладает пренебрежимо малой массой, его гравитацией на окружение пренебрегаем
        return Vector3D(0, 0, 0);
    }
};