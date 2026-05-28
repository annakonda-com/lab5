#include <gtest/gtest.h>
#include "Physics/AstronomySimulation.h"

class AstronomySimulationTest : public ::testing::Test {
protected:
    AstronomySimulation sim;
};

TEST_F(AstronomySimulationTest, PerfectIntersection) {
    Vector3D o1(0, 0, 0);
    Vector3D d1(1, 0, 0);
    Vector3D o2(1, 1, 0);
    Vector3D d2(0, -1, 0);

    Vector3D result = sim.RecoverPosition(o1, d1, o2, d2);

    EXPECT_NEAR(result.x, 1.0, 1e-5);
    EXPECT_NEAR(result.y, 0.0, 1e-5);
    EXPECT_NEAR(result.z, 0.0, 1e-5);
}

TEST_F(AstronomySimulationTest, ParallelLinesHandling) {
    Vector3D o1(0, 0, 0);
    Vector3D d1(1, 0, 0);
    Vector3D o2(0, 1, 0);
    Vector3D d2(1, 0, 0);

    Vector3D result = sim.RecoverPosition(o1, d1, o2, d2);

    EXPECT_NEAR(result.x, 0.0, 1e-5);
    EXPECT_NEAR(result.y, 0.0, 1e-5);
    EXPECT_NEAR(result.z, 0.0, 1e-5);
}

TEST_F(AstronomySimulationTest, SkewLinesMidpoint) {
    Vector3D o1(0, 0, 0);
    Vector3D d1(1, 0, 0);

    Vector3D o2(0.5, 1, 0);
    Vector3D d2(0, -1, 0);

    Vector3D result = sim.RecoverPosition(o1, d1, o2, d2);
    EXPECT_TRUE(result.Length() > 0.0);
}

TEST_F(AstronomySimulationTest, SymmetryTest) {
    Vector3D o1(0, 0, 0);
    Vector3D d1(0.707, 0.707, 0);
    Vector3D o2(2, 0, 0);
    Vector3D d2(-0.707, 0.707, 0);

    Vector3D res1 = sim.RecoverPosition(o1, d1, o2, d2);
    Vector3D res2 = sim.RecoverPosition(o2, d2, o1, d1);

    EXPECT_NEAR(res1.x, res2.x, 1e-5);
    EXPECT_NEAR(res1.y, res2.y, 1e-5);
    EXPECT_NEAR(res1.z, res2.z, 1e-5);
}