#include "../include/Physics/Space/Spacecraft.h"
#include "../include/Physics/Space/CelestialBody.h"
#include "../include/Physics/Space/SpaceSimulation.h"
#include "include/Sequences/ListSequence.h"
#include "ui/SimulationAstronomy.h"
#include "ui/SimulationMassSpring.h"
#include "ui/SymulationSpaceGame.h"

int main() {
    std::cout << "Choose mode:\n1 - Spacecraft\n2 - Astronomy\n3 - Mass Spring System\n";
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

        SymulationSpaceGame ui(simulation);
        ui.Run();
        delete player;
        delete bodiesContainer;
        return 0;
    }
    if (mode == 2) {
        SimulationAstronomy viewer;
        viewer.Run();
        return 0;
    }
    if (mode == 3) {
        MassSpringSystem system(0.016);

        system.AddParticle({-50,-50,0},1,true);
        system.AddParticle({50,-50,0},1,true);
        system.AddParticle({50,50,0},1);
        system.AddParticle({-50,50,0},1);

        system.AddSpring(0,1,5,0.1);
        system.AddSpring(1,2,5,0.1);
        system.AddSpring(2,3,5,0.1);
        system.AddSpring(3,0,5,0.1);

        system.AddSpring(0,2,5,0.1);
        system.AddSpring(1,3,5,0.1);

        SimulationMassSpring sim(system);
        sim.run();
    } else {
        std::cout << "No such mode\n";
    }

}