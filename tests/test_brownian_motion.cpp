#include <gtest/gtest.h>

#include "../include/Common/Particle.h"
#include "../include/Common/Vector3D.h"
#include "Physics/BrownianMotion.h"

static Particle MakeParticle(double x, double y, double z, bool fixed = false) {
    Particle p(Vector3D(x, y, z), 1.0, fixed);
    p.velocity = Vector3D(0, 0, 0);
    return p;
}

TEST(BrownianMotionTests, AddParticleIncreasesSize) {
    BrownianMotion sim(100, 100, 100, 0.02, 0.1);

    sim.AddParticle(MakeParticle(10, 10, 10));
    sim.AddParticle(MakeParticle(20, 20, 20));

    ASSERT_EQ(sim.GetParticles()->GetLength(), 2);
}

TEST(BrownianMotionTests, ParticleMovesAfterStep) {
    BrownianMotion sim(100, 100, 100, 0.02, 0.1);

    sim.AddParticle(MakeParticle(50, 50, 50));

    Vector3D before = (*sim.GetParticles())[0].position;

    sim.Step();

    Vector3D after = (*sim.GetParticles())[0].position;

    ASSERT_FALSE(before.x == after.x &&
                 before.y == after.y &&
                 before.z == after.z);
}

TEST(BrownianMotionTests, WallCollisionReflectsVelocityX) {
    BrownianMotion sim(100, 100, 100, 0.02, 0.1);

    Particle p = MakeParticle(99.0, 50.0, 50.0);
    p.velocity = Vector3D(10, 0, 0);

    sim.AddParticle(p);

    sim.Step();

    Particle result = (*sim.GetParticles())[0];

    ASSERT_LE(result.position.x, 100.0);
}

TEST(BrownianMotionTests, DeterministicBehaviorSameInitialState) {
    BrownianMotion sim1(100, 100, 100, 0.02, 0.1);
    BrownianMotion sim2(100, 100, 100, 0.02, 0.1);

    Particle p1 = MakeParticle(10, 10, 10);
    Particle p2 = MakeParticle(10, 10, 10);

    sim1.AddParticle(p1);
    sim2.AddParticle(p2);

    sim1.Step();
    sim2.Step();

    Particle r1 = (*sim1.GetParticles())[0];
    Particle r2 = (*sim2.GetParticles())[0];

    ASSERT_EQ(r1.position.x, r2.position.x);
    ASSERT_EQ(r1.position.y, r2.position.y);
    ASSERT_EQ(r1.position.z, r2.position.z);
}

TEST(BrownianMotionTests, CollisionDoesNotCrashAndChangesState) {
    BrownianMotion sim(100, 100, 100, 0.02, 0.1);

    Particle a = MakeParticle(50, 50, 50);
    Particle b = MakeParticle(50.1, 50, 50);

    sim.AddParticle(a);
    sim.AddParticle(b);

    sim.Step();

    ASSERT_EQ(sim.GetParticles()->GetLength(), 2);
}