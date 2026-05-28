#include "include/Physics/Spacecraft.h"
#include "include/Physics/CelestialBody.h"
#include "include/Physics/SpaceSimulation.h"
#include "include/Sequences/ListSequence.h"
#include "ui/SimulationViewer.h"
#include "ui/SpaceGameUI.h"

int main() {
    std::cout << "Choose mode:\n1 - Spacecraft\n2 - Astronomy\n";
    int mode;
    std::cin >> mode;
    if (mode == 1) {
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
    } else if (mode == 2) {
        SimulationViewer viewer;
        viewer.Run();
        return 0;
    } else {
        std::cout << "No such mode\n";
    }

}