#pragma once

#include <SFML/Graphics.hpp>
#include "Physics/BrownianMotion.h"

class SimulationBrownianMotion {
private:
    BrownianMotion sim;
    sf::RenderWindow window;

    float scale;
    float pointRadius;

public:
    SimulationBrownianMotion(double w, double h, double d,
                             double dt, double diffusion,
                             unsigned int screenW = 900,
                             unsigned int screenH = 700)
        : sim(w, h, d, dt, diffusion),
          window(sf::VideoMode({screenW, screenH}), "Brownian Motion"),
          scale(std::min(screenW / w, screenH / h)),
          pointRadius(3.f)
    {
        window.setFramerateLimit(60);
    }

    void Run() {
        while (window.isOpen()) {
            HandleEvents();
            sim.Step();
            Render();
        }
    }

private:
    void HandleEvents() {
        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mouse =
                event->getIf<sf::Event::MouseButtonPressed>()) {

                if (mouse->button == sf::Mouse::Button::Left) {

                    Vector3D pos(
                        mouse->position.x / scale,
                        mouse->position.y / scale,
                        200.0
                    );

                    sim.AddParticle(Particle(pos, 1.0));
                }
            }
        }
    }

    void Render() {
        window.clear(sf::Color::Black);

        auto particles = sim.GetParticles();
        int n = particles->GetLength();

        for (int i = 0; i < n; ++i) {
            const Particle& p = (*particles)[i];

            sf::CircleShape shape(pointRadius);

            shape.setFillColor(sf::Color::Cyan);

            shape.setPosition({
                (float)p.position.x * scale,
                (float)p.position.y * scale
            });

            window.draw(shape);
        }

        window.display();
    }
};