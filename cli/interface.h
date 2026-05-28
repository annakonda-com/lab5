#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <windows.h> // Для мгновенного опроса клавиш и управления курсором
#include "ListSequence.h"
#include "Physics/CelestialBody.h"
#include "Physics/Spacecraft.h"
#include "Physics/SpaceSimulation.h"
#include <conio.h>

class ConsoleGameInterface {
private:
    const int WIDTH = 40;
    const int HEIGHT = 20;

    SpaceSimulation* simulation;
    Sequence<Vector3D>* sharedForceStorage;
    Sequence<CelestialBody*>* bodiesStorage;
    Spacecraft* spacecraft;

    // Координаты прицела для установки планет
    int cursorX = 20;
    int cursorY = 10;

    // Инструмент для отрисовки экрана без мерцания (перенос курсора в 0,0)
    void SetCursorPosition(int x, int y) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SetConsoleCursorPosition(hOut, coord);
    }

    // Скрытие стандартного мигающего курсора Windows
    void HideCursor() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hOut, &cursorInfo);
    }

public:
    ConsoleGameInterface(Sequence<CelestialBody*>* bodiesSeq, Sequence<Vector3D>* forceSeq)
        : simulation(nullptr), sharedForceStorage(forceSeq), bodiesStorage(bodiesSeq), spacecraft(nullptr) {}

    ~ConsoleGameInterface() {
        delete simulation;
        delete spacecraft;
        // Очистка памяти АТД (Критерий 1.1)
        if (bodiesStorage) {
            for (int i = 0; i < bodiesStorage->GetLength(); ++i) delete bodiesStorage->Get(i);
            delete bodiesStorage;
        }
        delete sharedForceStorage;
    }

    void Start() {
        system("cls");
        HideCursor();

        // Спавним корабль в центре: позиция (10, 10), скорость (5, 0)
        // Масса настроена под масштаб консоли
        spacecraft = new Spacecraft(
            Vector3D(10.0, 10.0, 0.0),
            Vector3D(5.0, 0.0, 0.0),
            1000.0, 500.0, 8000.0,
            Vector3D(1.0, 0.0, 0.0),
            5.0
        );
        simulation = new SpaceSimulation(spacecraft, bodiesStorage);
        std::cout << "=== CONSOLE SPACE SIM ===\n"
                  << "ARROW KEYS (LEFT/RIGHT/UP/DOWN) - control spacecraft thrust\n"
                  << "W, A, S, D - move '+' crosshair for planets\n"
                  << "SPACEBAR - place a static planet\n"
                  << "ESC - exit game\n\n"
                  << "Press any key to start...";
        while (!_kbhit()); // Wait for start

        system("cls");

        double dt = 0.05; // Fixed time step for smoothness

        while (true) {
            // 1. INPUT PROCESSING (Asynchronous Windows keyboard polling)
            if (GetAsyncKeyState(VK_ESCAPE)) break;

            // Planet crosshair control (WASD)
            if (GetAsyncKeyState('W') & 0x8000) if (cursorY > 0) cursorY--;
            if (GetAsyncKeyState('S') & 0x8000) if (cursorY < HEIGHT - 1) cursorY++;
            if (GetAsyncKeyState('A') & 0x8000) if (cursorX > 0) cursorX--;
            if (GetAsyncKeyState('D') & 0x8000) if (cursorX < WIDTH - 1) cursorX++;

            // Planet placement (SPACEBAR) with spam protection
            static bool spacePressed = false;
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!spacePressed) {
                    // Mass 8e10 is chosen for noticeable trajectory curvature in the console
                    CelestialBody* planet = new CelestialBody(
                        Vector3D(cursorX, cursorY, 0.0),
                        Vector3D(0.0, 0.0, 0.0),
                        8e10,
                        true,
                        0.000001 // Modified G for screen scale
                    );
                    simulation->AddCelestialBody(planet);
                    spacePressed = true;
                }
            } else {
                spacePressed = false;
            }

            // Spacecraft control (Arrow Keys)
            Vector3D thrustDir(0, 0, 0);
            bool engineOn = false;
            if (GetAsyncKeyState(VK_UP) & 0x8000)    { thrustDir.y = -1; engineOn = true; }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000)  { thrustDir.y = 1;  engineOn = true; }
            if (GetAsyncKeyState(VK_LEFT) & 0x8000)  { thrustDir.x = -1; engineOn = true; }
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { thrustDir.x = 1;  engineOn = true; }

            if (engineOn) {
                spacecraft->SetThrustDirection(thrustDir);
                spacecraft->SetThrottle(1.0);
            } else {
                spacecraft->SetThrottle(0.0);
            }

            // 2. PHYSICS CALCULATION (Via your Map-Reduce engine)
            try {
                simulation->Tick(dt, sharedForceStorage);
            } catch (...) {
                // Prevent console crash on ADT errors
            }

            // Toroidal screen (edge wrapping)
            Vector3D pos = spacecraft->GetPosition();
            if (pos.x < 0) pos.x += WIDTH;
            if (pos.x >= WIDTH) pos.x -= WIDTH;
            if (pos.y < 0) pos.y += HEIGHT;
            if (pos.y >= HEIGHT) pos.y -= HEIGHT;
            spacecraft->SetPosition(pos);

            // 3. FRAME RENDERING INTO GRID BUFFER
            std::vector<std::string> grid(HEIGHT, std::string(WIDTH, ' '));

            // Draw placement crosshair
            grid[cursorY][cursorX] = '+';

            // Draw planets from your ListSequence
            const Sequence<CelestialBody*>* bodies = simulation->GetCelestialBodies();
            for (int i = 0; i < bodies->GetLength(); ++i) {
                Vector3D bPos = bodies->Get(i)->GetPosition();
                int px = static_cast<int>(bPos.x);
                int py = static_cast<int>(bPos.y);
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    grid[py][px] = 'O'; // Letter O represents a planet
                }
            }

            // Draw spacecraft
            int cx = static_cast<int>(spacecraft->GetPosition().x);
            int cy = static_cast<int>(spacecraft->GetPosition().y);
            if (cx >= 0 && cx < WIDTH && cy >= 0 && cy < HEIGHT) {
                grid[cy][cx] = engineOn ? 'A' : 'x'; // 'A' if active under thrust, 'x' by inertia
            }

            // Output frame to screen
            SetCursorPosition(0, 0);
            std::cout << "+" << std::string(WIDTH, '-') << "+\n";
            for (int y = 0; y < HEIGHT; ++y) {
                std::cout << "|" << grid[y] << "|\n";
            }
            std::cout << "+" << std::string(WIDTH, '-') << "+\n";

            // Metrics (Criterion 4.2)
            std::cout << "Fuel: " << static_cast<int>(spacecraft->GetFuelMass()) << " kg   \n"
                      << "Planet Crosshair: [" << cursorX << ", " << cursorY << "]   \n"
                      << "Planets in Orbit: " << bodies->GetLength() << "    \n";

            Sleep(30); // Delay for FPS stabilization (~30 frames per second)
        }
        system("cls");
        std::cout << "Simulation finished successfully.\n";
    }
};

