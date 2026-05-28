#pragma once
#include "../include/Common/Particle.h"
#include "../Common/Vector3D.h"
#include "../Sequences/ListSequence.h"

struct Spring {
    int p1, p2;
    double restLength;
    double k1, k2;

    Spring(int i, int j, double len, double s1, double s2)
        : p1(i), p2(j), restLength(len), k1(s1), k2(s2) {}
};

class MassSpringSystem {
private:
    MutableListSequence<Particle*> particles;
    MutableListSequence<Spring*> springs;

    Vector3D gravity;
    double dt;
    double damping = 0.995;

public:
    MassSpringSystem(double timeStep)
        : gravity(0, 200.0, 0), dt(timeStep) {}

    int GetParticleCount() const { return particles.GetLength(); }
    Particle* GetParticle(int i) { return particles[i]; }

    int GetSpringCount() const { return springs.GetLength(); }
    Spring* GetSpring(int i) { return springs[i]; }

    void AddParticle(Vector3D pos, double mass, bool fixed = false) {
        particles.Append(new Particle(pos, mass, fixed));
    }

    void AddSpring(int i, int j, double k1, double k2) {
        double len = (particles[i]->position - particles[j]->position).Length();
        springs.Append(new Spring(i, j, len, k1, k2));
    }

    void Update() {
        for (int i = 0; i < particles.GetLength(); i++) {
            particles[i]->force = gravity * particles[i]->mass;
        }

        for (int i = 0; i < particles.GetLength(); i++) {
            Particle* p = particles[i];

            if (p->fixed) continue;

            Vector3D acc = p->force / p->mass;
            p->velocity = (p->velocity + acc * dt) * damping;
            p->position = p->position + p->velocity * dt;
        }

        for (int iter = 0; iter < 5; iter++) {
            for (int i = 0; i < springs.GetLength(); i++) {
                Spring* s = springs[i];
                Particle* a = particles[s->p1];
                Particle* b = particles[s->p2];

                Vector3D delta = a->position - b->position;
                double len = delta.Length();
                if (len < 1e-6) continue;

                double diff = (len - s->restLength) / len * 0.5;

                if (!a->fixed)
                    a->position -= delta * diff;

                if (!b->fixed)
                    b->position += delta * diff;
            }
        }
    }

    ~MassSpringSystem() {
        for (int i = 0; i < particles.GetLength(); i++) delete particles[i];
        for (int i = 0; i < springs.GetLength(); i++) delete springs[i];
    }
};