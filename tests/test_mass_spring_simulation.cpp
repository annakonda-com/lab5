#include <gtest/gtest.h>
#include "../include/Physics/MassSpringSystem.h"

class MassSpringSystemTest : public ::testing::Test {
protected:
    MassSpringSystem* system;
    const double dt = 0.01;

    void SetUp() override {
        system = new MassSpringSystem(dt);
    }

    void TearDown() override {
        delete system;
    }
};

TEST_F(MassSpringSystemTest, AddParticle) {
    system->AddParticle(Vector3D(0, 0, 0), 1.0, false);
    
    EXPECT_EQ(system->GetParticleCount(), 1);
    
    Particle* p = system->GetParticle(0);
    EXPECT_DOUBLE_EQ(p->mass, 1.0);
    EXPECT_FALSE(p->fixed);
}

TEST_F(MassSpringSystemTest, AddSpring) {
    system->AddParticle(Vector3D(0, 0, 0), 1.0, true);  // 0
    system->AddParticle(Vector3D(0, 10, 0), 1.0, false); // 1
    
    system->AddSpring(0, 1, 100.0, 100.0);
    
    EXPECT_EQ(system->GetSpringCount(), 1);
    Spring* s = system->GetSpring(0);
    
    EXPECT_EQ(s->p1, 0);
    EXPECT_EQ(s->p2, 1);
    EXPECT_DOUBLE_EQ(s->restLength, 10.0);
}

TEST_F(MassSpringSystemTest, SimulationUpdate) {
    system->AddParticle(Vector3D(0, 0, 0), 1.0, false);
    
    Vector3D initialPos = system->GetParticle(0)->position;
    system->Update();
    
    Vector3D newPos = system->GetParticle(0)->position;
    EXPECT_NE(newPos.y, initialPos.y); 
}