#pragma once
#include "Vector3D.h"

struct Observation {
    Vector3D observerPos;
    Vector3D direction;

    Observation() = default;
    Observation(const Vector3D& o, const Vector3D& d)
        : observerPos(o), direction(d) {}
};