#pragma once
#include "../Sequences/Sequence.h"
#include "CelestialBody.h"
#include "Spacecraft.h"
#include "../Physics/MapReduceEngine.h"

class SpaceSimulation {
private:
    Spacecraft* playerCraft;
    Sequence<CelestialBody*>* celestialBodies;

public:
    SpaceSimulation(Spacecraft* craft, Sequence<CelestialBody*>* bodiesContainer)
        : playerCraft(craft), celestialBodies(bodiesContainer) {
        if (!playerCraft) {
            throw std::invalid_argument("Simulation Error: Spacecraft object cannot be null.");
        }
        if (!celestialBodies) {
            throw std::invalid_argument("Simulation Error: Celestial bodies container cannot be null.");
        }
    }

    void AddCelestialBody(CelestialBody* body) {
        if (!body) {
            throw std::invalid_argument("Simulation Error: Cannot add a null celestial body.");
        }
        celestialBodies->Append(body);
    }

    Spacecraft* GetSpacecraft() const { return playerCraft; }
    const Sequence<CelestialBody*>* GetCelestialBodies() const { return celestialBodies; }

    void Tick(double deltaTime, Sequence<Vector3D>* forceVectorStorage) {
        if (deltaTime <= 0.0) {
            throw std::invalid_argument("Simulation Error: Timestep (dt) must be strictly positive.");
        }

        auto reduceSum = [](const Vector3D& accum, const Vector3D& force) -> Vector3D {
            return accum + force;
        };

        while (forceVectorStorage->GetLength() > 0) forceVectorStorage->RemoveAt(0);

        Vector3D craftPos = playerCraft->GetPosition();
        double craftMass = playerCraft->GetMass();

        auto mapCraftGravity = [craftPos, craftMass](CelestialBody* body) -> Vector3D {
            return body->CalculateGravityForce(craftPos, craftMass);
        };

        MapReduceEngine::Map(celestialBodies, forceVectorStorage, mapCraftGravity);
        Vector3D totalCraftGravity = MapReduceEngine::Reduce(forceVectorStorage, reduceSum, Vector3D(0, 0, 0));
        playerCraft->UpdateState(deltaTime, totalCraftGravity);

        for (int i = 0; i < celestialBodies->GetLength(); ++i) {
            CelestialBody* currentBody = celestialBodies->Get(i);
            if (currentBody->IsStatic()) continue;
            while (forceVectorStorage->GetLength() > 0) forceVectorStorage->RemoveAt(0);

            Vector3D bodyPos = currentBody->GetPosition();
            double bodyMass = currentBody->GetMass();

            auto mapBodyGravity = [currentBody, bodyPos, bodyMass](CelestialBody* otherBody) -> Vector3D {
                if (currentBody == otherBody) return Vector3D(0, 0, 0);
                return otherBody->CalculateGravityForce(bodyPos, bodyMass);
            };

            MapReduceEngine::Map(celestialBodies, forceVectorStorage, mapBodyGravity);
            Vector3D totalBodyForce = MapReduceEngine::Reduce(forceVectorStorage, reduceSum, Vector3D(0, 0, 0));

            currentBody->UpdateState(deltaTime, totalBodyForce);
        }

        while (forceVectorStorage->GetLength() > 0) {
            forceVectorStorage->RemoveAt(0);
        }
    }
};