#include "../cli/interface.h"
#include "ListSequence.h"
#include "Sequence.h"
#include "Vector3D.h"
#include "Physics/CelestialBody.h"

int main() {
    // Инициализируем полиморфные АТД на базе твоих изменяемых списков
    Sequence<CelestialBody*>* bodies = new MutableListSequence<CelestialBody*>();
    Sequence<Vector3D>* forces = new MutableListSequence<Vector3D>();

    // Запускаем псевдографическую мини-игру
    ConsoleGameInterface game(bodies, forces);
    game.Start();

    return 0;
}