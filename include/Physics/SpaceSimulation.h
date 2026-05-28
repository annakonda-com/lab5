#pragma once
#include "../Sequences/Sequence.h"
#include "CelestialBody.h"
#include "Spacecraft.h"
#include "../Physics/MapReduceEngine.h"

class SpaceSimulation {
private:
    Spacecraft* playerCraft;
    Sequence<CelestialBody*>* celestialBodies; // Полиморфный АТД-контейнер для источников гравитации

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

    // Физический шаг симуляции (вычисление гравитации через Map-Reduce)
    void Tick(double deltaTime, Sequence<Vector3D>* forceVectorStorage) {
        if (deltaTime <= 0.0) {
            throw std::invalid_argument("Simulation Error: Timestep (dt) must be strictly positive.");
        }

        Vector3D craftPos = playerCraft->GetPosition();
        double craftMass = playerCraft->GetMass();

        // 1. ЭТАП MAP: Рассчитываем силу гравитационного влияния от каждого тела независимо
        auto mapGravity = [craftPos, craftMass](CelestialBody* body) -> Vector3D {
            return body->CalculateGravityForce(craftPos, craftMass);
        };
        MapReduceEngine::Map(celestialBodies, forceVectorStorage, mapGravity);

        // 2. ЭТАП REDUCE: Суммируем векторы всех сил в один результирующий вектор
        auto reduceSum = [](const Vector3D& accum, const Vector3D& force) -> Vector3D {
            return accum + force;
        };
        Vector3D totalGravityForce = MapReduceEngine::Reduce(forceVectorStorage, reduceSum, Vector3D(0, 0, 0));

        // 3. Обновляем состояние космического корабля (сила тяги посчитается внутри UpdateState)
        playerCraft->UpdateState(deltaTime, totalGravityForce);

        // 4. Обновляем состояние нестатичных тел окружения
        for (int i = 0; i < celestialBodies->GetLength(); ++i) {
            CelestialBody* body = celestialBodies->Get(i);
            // Нестатичные тела движутся по своим траекториям/инерции
            body->UpdateState(deltaTime, Vector3D(0, 0, 0));
        }

        // Очищаем временное хранилище векторов сил через интерфейс Sequence, чтобы не раздувать память
        while (forceVectorStorage->GetLength() > 0) {
            forceVectorStorage->RemoveAt(0);
        }
    }
};