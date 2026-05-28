#include <gtest/gtest.h>
#include "../include/Physics/Space/MapReduceEngine.h"
#include "../include/Physics/Space/Spacecraft.h"
#include "../include/Physics/Space/CelestialBody.h"
#include "../include/Physics/Space/SpaceSimulation.h"
#include "../include/Sequences/ListSequence.h"


TEST(MapReduceEngineTest, MapFunctionality) {
    MutableListSequence<int> source;
    source.Append(1); source.Append(2); source.Append(3);
    MutableListSequence<int> destination;

    auto square = [](int x) { return x * x; };
    MapReduceEngine::Map(&source, &destination, square);

    EXPECT_EQ(destination.GetLength(), 3);
    EXPECT_EQ(destination.Get(0), 1);
    EXPECT_EQ(destination.Get(1), 4);
    EXPECT_EQ(destination.Get(2), 9);
}

TEST(MapReduceEngineTest, ReduceFunctionality) {
    MutableListSequence<int> source;
    source.Append(10); source.Append(20); source.Append(30);

    auto sum = [](int accum, int val) { return accum + val; };
    int result = MapReduceEngine::Reduce(&source, sum, 0);

    EXPECT_EQ(result, 60);
}


TEST(SpacecraftTest, InitializationParameters) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 1.0);

    EXPECT_DOUBLE_EQ(craft.GetMass(), 150.0);
    EXPECT_DOUBLE_EQ(craft.GetFuelMass(), 50.0);
}

TEST(SpacecraftTest, FuelConsumptionAndThrust) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 1.0);
    craft.SetThrottle(1.0);

    craft.UpdateState(1.0, Vector3D(0,0,0));

    EXPECT_LT(craft.GetFuelMass(), 50.0);
    EXPECT_GT(craft.GetMass(), 148.0);
}

TEST(SpacecraftTest, InvalidMassThrows) {
    EXPECT_THROW(Spacecraft(Vector3D(0,0,0), Vector3D(0,0,0), -10.0, 50.0, 10.0, Vector3D(1,0,0), 1.0), std::invalid_argument);
}

TEST(CelestialBodyTest, GravityForceCalculation) {
    CelestialBody planet(Vector3D(10, 0, 0), Vector3D(0,0,0), 5.972e24, true);

    Vector3D force = planet.CalculateGravityForce(Vector3D(0,0,0), 1000.0);

    EXPECT_GT(force.x, 0.0);
    EXPECT_DOUBLE_EQ(force.y, 0.0);
}

TEST(CelestialBodyTest, StaticBodyDoesNotMove) {
    CelestialBody planet(Vector3D(0,0,0), Vector3D(10,0,0), 1000.0, true);
    planet.UpdateState(1.0, Vector3D(100, 0, 0));

    EXPECT_DOUBLE_EQ(planet.GetPosition().x, 0.0);
    EXPECT_DOUBLE_EQ(planet.GetVelocity().x, 10.0);
}

TEST(SpaceSimulationTest, ConstructorValidation) {
    MutableListSequence<CelestialBody*> bodies;

    EXPECT_THROW(SpaceSimulation(nullptr, &bodies), std::invalid_argument);

    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 1.0);
    EXPECT_THROW(SpaceSimulation(&craft, nullptr), std::invalid_argument);
}

TEST(SpaceSimulationTest, TickSimulationFlow) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 1.0);
    MutableListSequence<CelestialBody*> bodies;
    MutableListSequence<Vector3D> forceStorage;

    SpaceSimulation sim(&craft, &bodies);

    CelestialBody* moon = new CelestialBody(Vector3D(100, 0, 0), Vector3D(0,0,0), 1e10, false);
    sim.AddCelestialBody(moon);

    EXPECT_NO_THROW(sim.Tick(0.1, &forceStorage));

    EXPECT_NE(craft.GetPosition().x, 0.0);

    delete moon;
}

TEST(SpacecraftTest, FuelDepletionLogic) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 10.0);
    craft.SetThrottle(1.0);
    craft.UpdateState(1.0, Vector3D(0,0,0));

    EXPECT_NEAR(craft.GetFuelMass(), 40.0, 1e-5);
    EXPECT_NEAR(craft.GetMass(), 140.0, 1e-5);
}

TEST(SpacecraftTest, FuelRunsOutMidTick) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 10.0, 100.0, Vector3D(1,0,0), 100.0);
    craft.SetThrottle(1.0);
    craft.UpdateState(1.0, Vector3D(0,0,0));

    EXPECT_NEAR(craft.GetFuelMass(), 0.0, 1e-5);
    EXPECT_NEAR(craft.GetMass(), 100.0, 1e-5); // Только сухая масса
}

TEST(CelestialBodyTest, ZeroDistanceGravityHandling) {
    CelestialBody sun(Vector3D(0, 0, 0), Vector3D(0, 0, 0), 1e20, true);
    Vector3D force = sun.CalculateGravityForce(Vector3D(0, 0, 0), 100.0);

    EXPECT_DOUBLE_EQ(force.x, 0.0);
    EXPECT_DOUBLE_EQ(force.y, 0.0);
    EXPECT_DOUBLE_EQ(force.z, 0.0);
}

TEST(CelestialBodyTest, StaticBodyImmutability) {
    Vector3D initialPos(10, 10, 10);
    CelestialBody asteroid(initialPos, Vector3D(1, 1, 1), 1000.0, true);

    asteroid.UpdateState(1.0, Vector3D(1e9, 1e9, 1e9));

    EXPECT_DOUBLE_EQ(asteroid.GetPosition().x, initialPos.x);
    EXPECT_DOUBLE_EQ(asteroid.GetVelocity().x, 1.0);
}


TEST(MapReduceEngineTest, EmptySourceHandling) {
    MutableListSequence<int> emptySource;
    MutableListSequence<int> destination;

    EXPECT_NO_THROW(MapReduceEngine::Map(&emptySource, &destination, [](int x){ return x; }));
    EXPECT_EQ(destination.GetLength(), 0);

    int res = MapReduceEngine::Reduce(&emptySource, [](int a, int b){ return a+b; }, 42);
    EXPECT_EQ(res, 42);
}



TEST(SpaceSimulationTest, TickWithZeroOrNegativeDeltaTime) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 1.0);
    MutableListSequence<CelestialBody*> bodies;
    MutableListSequence<Vector3D> forceStorage;
    SpaceSimulation sim(&craft, &bodies);

    EXPECT_THROW(sim.Tick(0.0, &forceStorage), std::invalid_argument);
    EXPECT_THROW(sim.Tick(-1.0, &forceStorage), std::invalid_argument);
}

TEST(SpaceSimulationTest, TickEmptyBodiesList) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 1.0);
    MutableListSequence<CelestialBody*> bodies;
    MutableListSequence<Vector3D> forceStorage;
    SpaceSimulation sim(&craft, &bodies);

    EXPECT_NO_THROW(sim.Tick(1.0, &forceStorage));
}

TEST(SpaceSimulationTest, ForceStorageCleaning) {
    Spacecraft craft(Vector3D(0,0,0), Vector3D(0,0,0), 100.0, 50.0, 10.0, Vector3D(1,0,0), 1.0);
    MutableListSequence<CelestialBody*> bodies;
    MutableListSequence<Vector3D> forceStorage;
    SpaceSimulation sim(&craft, &bodies);

    CelestialBody body(Vector3D(100, 0, 0), Vector3D(0, 0, 0), 1e10, true);
    sim.AddCelestialBody(&body);

    sim.Tick(1.0, &forceStorage);

    EXPECT_EQ(forceStorage.GetLength(), 0);
}



