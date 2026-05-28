#pragma once
#include "Vector3D.h"

struct Particle {
    Vector3D position;
    Vector3D velocity;
    Vector3D force;
    double mass;
    bool fixed;

    Particle(Vector3D pos, double m, bool isFixed = false)
        : position(pos),
          velocity(0,0,0),
          force(0,0,0),
          mass(m),
          fixed(isFixed) {}
};
