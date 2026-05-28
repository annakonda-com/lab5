#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <cmath>

#include "../include/Physics/Space/SpaceSimulation.h"
#include "../Sequences/ListSequence.h"

class SymulationSpaceGame {
private:
    SpaceSimulation& simulation;
    sf::RenderWindow window;
    sf::Font font;

    double scale;
    sf::Vector2f viewOffset;
    double currentThrottle;
    float shipAngle;

public:
    SymulationSpaceGame(SpaceSimulation& sim)
        : simulation(sim),
          window(sf::VideoMode({1200, 800}), "Space Sandbox Pro (SFML 3.0)"),
          scale(0.05),
          viewOffset(600.f, 400.f),
          currentThrottle(0.0),
          shipAngle(270.f)
    {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                std::cerr << "Warning: Failed to load font.\n";
            }
        }
        window.setFramerateLimit(60);
    }

    void Run() {
        sf::Clock clock;
        MutableListSequence<Vector3D> forceVectorStorage;

        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
                }

                if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    if (mouseWheel->delta > 0) scale *= 1.25;
                    else scale /= 1.25;
                }

                if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                    float mouseX = static_cast<float>(mousePressed->position.x);
                    float mouseY = static_cast<float>(mousePressed->position.y);

                    double worldX = (mouseX - viewOffset.x) / scale;
                    double worldY = (mouseY - viewOffset.y) / scale;
                    Vector3D spawnPos(worldX, worldY, 0);

                    if (mousePressed->button == sf::Mouse::Button::Left) {
                        CelestialBody* heaviest = FindHeaviestBody();
                        Vector3D initialVelocity(0, 0, 0);

                        if (heaviest) {
                            Vector3D rVec = spawnPos - heaviest->GetPosition();
                            double r = rVec.Length();

                            if (r > 1.0) {
                                double v = std::sqrt(6.67430e-11 * heaviest->GetMass() / r);
                                Vector3D tangent(-rVec.GetY(), rVec.GetX(), 0);
                                initialVelocity = tangent.Normalize() * v;
                            }
                        }

                        CelestialBody* planet = new CelestialBody(spawnPos, initialVelocity, 1e15, false);
                        simulation.AddCelestialBody(planet);
                    }
                    else if (mousePressed->button == sf::Mouse::Button::Right) {
                        CelestialBody* star = new CelestialBody(spawnPos, Vector3D(0, 0, 0), 3e17, true);
                        simulation.AddCelestialBody(star);
                    }
                }
            }

            HandleInput();

            float dt = clock.restart().asSeconds();
            if (dt > 0.1f) dt = 0.1f;

            try {
                double timeWarp = 40.0;
                simulation.Tick(dt * timeWarp, &forceVectorStorage);
            } catch (const std::exception& e) {
                std::cerr << "Physics Error: " << e.what() << "\n";
            }

            window.clear(sf::Color(8, 8, 15));

            DrawGrid();
            DrawCelestialBodies();
            DrawSpacecraft();
            DrawHUD();

            window.display();
        }

        CleanUpMemory();
    }

private:
    CelestialBody* FindHeaviestBody() {
        const auto* bodies = simulation.GetCelestialBodies();
        if (!bodies || bodies->GetLength() == 0) return nullptr;
        IEnumerator<CelestialBody*>* it = const_cast<Sequence<CelestialBody*>*>(bodies)->GetEnumerator();
        CelestialBody* heaviest = nullptr;
        double maxMass = -1.0;

        it->Reset();
        while (it->MoveNext()) {
            CelestialBody* current = it->GetCurrent();
            if (current->GetMass() > maxMass) {
                maxMass = current->GetMass();
                heaviest = current;
            }
        }
        delete it;
        return heaviest;
    }

    void HandleInput() {
        Spacecraft* craft = simulation.GetSpacecraft();
        if (!craft) return;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            shipAngle -= 4.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            shipAngle += 4.0f;
        }

        if (shipAngle < 0.f) shipAngle += 360.f;
        if (shipAngle >= 360.f) shipAngle -= 360.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            currentThrottle = 1.0;
        } else {
            currentThrottle = 0.0;
        }
        craft->SetThrottle(currentThrottle);

        float rad = shipAngle * 3.14159265f / 180.f;
        Vector3D thrustDir(std::cos(rad), std::sin(rad), 0.0);
        craft->SetThrustDirection(thrustDir);

        Vector3D craftPos = craft->GetPosition();
        viewOffset.x = 600.f - static_cast<float>(craftPos.GetX() * scale);
        viewOffset.y = 400.f - static_cast<float>(craftPos.GetY() * scale);
    }

    void DrawGrid() {
        sf::Color gridColor(30, 30, 45);
        float step = 100.f;
        float startX = std::fmod(viewOffset.x, step);
        float startY = std::fmod(viewOffset.y, step);

        for (float x = startX; x < 1200; x += step) {
            sf::Vertex line[] = { { {x, 0}, gridColor }, { {x, 800}, gridColor } };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
        for (float y = startY; y < 800; y += step) {
            sf::Vertex line[] = { { {0, y}, gridColor }, { {1200, y}, gridColor } };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }

    void DrawCelestialBodies() {
        const auto* bodies = simulation.GetCelestialBodies();
        if (!bodies) return;

        for (int i = 0; i < bodies->GetLength(); ++i) {
            CelestialBody* body = bodies->Get(i);
            Vector3D pos = body->GetPosition();

            float screenX = static_cast<float>(pos.GetX() * scale) + viewOffset.x;
            float screenY = static_cast<float>(pos.GetY() * scale) + viewOffset.y;

            float radius = static_cast<float>(std::log10(body->GetMass() + 1.0) * 2.0f) - 10.f;
            if (radius < 8.f) radius = 8.f;
            if (radius > 60.f) radius = 60.f;

            sf::CircleShape circle(radius);
            circle.setFillColor(body->IsStatic() ? sf::Color(255, 140, 50) : sf::Color(65, 150, 250));
            circle.setOrigin({radius, radius});
            circle.setPosition({screenX, screenY});

            window.draw(circle);
        }
    }

    void DrawSpacecraft() {
        Spacecraft* craft = simulation.GetSpacecraft();
        if (!craft) return;

        Vector3D pos = craft->GetPosition();
        float screenX = static_cast<float>(pos.GetX() * scale) + viewOffset.x;
        float screenY = static_cast<float>(pos.GetY() * scale) + viewOffset.y;

        sf::ConvexShape ship(4);
        ship.setPoint(0, sf::Vector2f(0.f, -18.f));
        ship.setPoint(1, sf::Vector2f(12.f, 12.f));
        ship.setPoint(2, sf::Vector2f(0.f, 6.f));
        ship.setPoint(3, sf::Vector2f(-12.f, 12.f));

        ship.setFillColor(sf::Color(220, 220, 230));
        ship.setOutlineThickness(2.f);
        ship.setOutlineColor(sf::Color(45, 240, 120));

        ship.setPosition({screenX, screenY});
        ship.setRotation(sf::degrees(shipAngle + 90.f));
        window.draw(ship);

        if (currentThrottle > 0.0 && craft->GetFuelMass() > 0.0) {
            sf::ConvexShape flame(3);
            flame.setPoint(0, sf::Vector2f(0.f, 6.f));
            flame.setPoint(1, sf::Vector2f(6.f, 16.f));
            flame.setPoint(2, sf::Vector2f(-6.f, 16.f));

            float flameLength = static_cast<float>(currentThrottle * 25.f);
            flame.setPoint(1, sf::Vector2f(5.f, 6.f + flameLength));
            flame.setPoint(2, sf::Vector2f(-5.f, 6.f + flameLength));

            flame.setFillColor(sf::Color(255, 100, 0, 200));
            flame.setPosition({screenX, screenY});
            flame.setRotation(sf::degrees(shipAngle + 90.f));
            window.draw(flame);
        }
    }

    void DrawHUD() {
        Spacecraft* craft = simulation.GetSpacecraft();
        if (!craft) return;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "=== SANDBOX ORBITAL HUD ===\n";
        ss << "Speed       : " << craft->GetVelocity().Length() << " m/s\n";
        ss << "Heading     : " << shipAngle << " deg\n";
        ss << "Fuel Mass   : " << craft->GetFuelMass() << " kg\n";
        ss << "Engine      : " << (currentThrottle > 0.0 ? "ON [SPACEBAR]" : "OFF") << "\n";
        ss << "Gravity Sources: " << simulation.GetCelestialBodies()->GetLength() << "\n\n";
        ss << "=== CONTROLS ===\n";
        ss << "[A / D]       — Rotate Ship\n";
        ss << "[SPACEBAR]    — Thrust Forward\n";
        ss << "[Mouse Wheel] — Scale/Zoom\n";
        ss << "[LMB]         — Spawn Orbiting Planet\n";
        ss << "[RMB]         — Spawn Super Star";

        sf::Text hudText(font, ss.str(), 15);
        hudText.setFillColor(sf::Color::White);
        hudText.setPosition({15.f, 15.f});

        sf::RectangleShape background({280.f, 240.f});
        background.setFillColor(sf::Color(0, 0, 0, 170));
        background.setPosition({10.f, 10.f});

        window.draw(background);
        window.draw(hudText);
    }

    void CleanUpMemory() {
        const auto* bodies = simulation.GetCelestialBodies();
        if (bodies) {
            for (int i = 0; i < bodies->GetLength(); ++i) {
                delete bodies->Get(i);
            }
        }
    }
};