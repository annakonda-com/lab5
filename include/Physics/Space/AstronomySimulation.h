#pragma once
#include "../include/Common/Vector3D.h"

class AstronomySimulation {
public:

    Vector3D RecoverPosition(
        const Vector3D& o1, const Vector3D& d1,
        const Vector3D& o2, const Vector3D& d2
    ) const {
        Vector3D r = o2 - o1;

        double a = 1.0;
        double b = d1.x*d2.x + d1.y*d2.y + d1.z*d2.z;
        double c = 1.0;

        double d = d1.x*r.x + d1.y*r.y + d1.z*r.z;
        double e = d2.x*r.x + d2.y*r.y + d2.z*r.z;

        double det = a*c - b*b;

        if (fabs(det) < 1e-6)
            return Vector3D(0,0,0);

        double t1 = (d*c - e*b) / det;
        double t2 = (d*b - e*a) / det;

        Vector3D p1 = o1 + d1 * t1;
        Vector3D p2 = o2 + d2 * t2;

        return (p1 + p2) * 0.5;
    }
};