#include "include/Physics/Spacecraft.h"
#include "include/Physics/CelestialBody.h"
#include "include/Physics/SpaceSimulation.h"
#include "include/Sequences/ListSequence.h"
#include "ui/SpaceGameUI.h"

int main() {
    Spacecraft* player = new Spacecraft(
        Vector3D(0, 350, 0),
        Vector3D(240, 0, 0),
        5000.0, 2000.0, 500000.0, Vector3D(1, 0, 0), 15.0
    );

    MutableListSequence<CelestialBody*>* bodiesContainer = new MutableListSequence<CelestialBody*>();

    bodiesContainer->Append(new CelestialBody(Vector3D(0, 0, 0), Vector3D(0, 0, 0), 3e17, true));

    SpaceSimulation simulation(player, bodiesContainer);

    SpaceGameUI ui(simulation);
    ui.Run();
    delete player;
    delete bodiesContainer;
    return 0;
}