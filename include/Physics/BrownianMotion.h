#pragma once

#include "Common/Particle.h"
#include "Sequences/ListSequence.h"
#include "Common/RandomGenerator.h"

class BrownianMotion {
private:
    Sequence<Particle> *particles;
    RandomGen rng;

    double width, height, depth;
    double dt;
    double diffusion_coeff;
    double radius;

public:
    BrownianMotion(double w, double h, double d, double timeStep, double diffusion)
        : width(w), height(h), depth(d),
          dt(0.02),
          diffusion_coeff(diffusion),
          particles(new MutableListSequence<Particle>()),
          rng(42),
          radius(6.0) {
    }

    ~BrownianMotion() {
        delete particles;
    }

    Sequence<Particle> *GetParticles() const {
        return particles;
    }

    void AddParticle(const Particle &p) {
        particles->Append(p);
    }

    void Step() {
        int count = particles->GetLength();

        for (int i = 0; i < count; ++i) {
            Particle &p = (*particles)[i];
            if (p.fixed) continue;
            Vector3D noise(rng.Next(), rng.Next(), rng.Next());
            p.velocity += noise * diffusion_coeff;
            double speed = p.velocity.Length();
            if (speed > 50.0) {
                p.velocity = p.velocity.Normalize() * 50.0;
            }

            p.position += p.velocity * dt;
            CheckWallCollision(p);
        }
        for (int i = 0; i < count; ++i) {
            for (int j = i + 1; j < count; ++j) {
                ResolveCollision((*particles)[i], (*particles)[j]);
            }
        }
    }

private:
    void CheckWallCollision(Particle &p) {
        if (p.position.x < 0) {
            p.position.x = 0;
            p.velocity.x *= -1;
        }
        if (p.position.x > width) {
            p.position.x = width;
            p.velocity.x *= -1;
        }

        if (p.position.y < 0) {
            p.position.y = 0;
            p.velocity.y *= -1;
        }
        if (p.position.y > height) {
            p.position.y = height;
            p.velocity.y *= -1;
        }

        if (p.position.z < 0) {
            p.position.z = 0;
            p.velocity.z *= -1;
        }
        if (p.position.z > depth) {
            p.position.z = depth;
            p.velocity.z *= -1;
        }
    }

    void ResolveCollision(Particle &a, Particle &b) {
        Vector3D delta = a.position - b.position;
        double dist = delta.Length();
        if (dist >= radius * 2 || dist < 1e-9) return;

        Vector3D normal = delta / dist;
        double overlap = (radius * 2) - dist;
        double percent = 0.8;
        Vector3D separation = normal * (overlap * percent);

        if (!a.fixed) a.position += separation * 0.5;
        if (!b.fixed) a.position -= separation * 0.5;

        Vector3D relVel = a.velocity - b.velocity;
        double velAlongNormal = relVel.Dot(normal);

        if (velAlongNormal > 0) return;

        double restitution = 1.0;
        double impulseMagnitude = -(1.0 + restitution) * velAlongNormal / 2.0;

        Vector3D impulseVec = normal * impulseMagnitude;

        if (!a.fixed) a.velocity += impulseVec;
        if (!b.fixed) a.velocity -= impulseVec;
    }
};
