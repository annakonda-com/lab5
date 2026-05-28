#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include "Physics/MassSpringSystem.h"

class SimulationMassSpring {
private:
    MassSpringSystem& system;
    sf::RenderWindow window;

    int dragged = -1;
    float grabRadius = 15.f;

    float scale = 1.0f;
    sf::Vector2f offset = {400, 300};

    sf::Font font;
    sf::Text uiText;

    sf::Vector2f toScreen(const Vector3D& p) {
        return {
            offset.x + float(p.x * scale),
            offset.y + float(p.y * scale)
        };
    }

    Vector3D fromScreen(sf::Vector2i m) {
        return Vector3D(
            (m.x - offset.x) / scale,
            (m.y - offset.y) / scale,
            0
        );
    }

    int findClosest(sf::Vector2i mouse) {
        int idx = -1;
        float best = grabRadius;

        for (int i = 0; i < system.GetParticleCount(); i++) {
            auto p = toScreen(system.GetParticle(i)->position);
            float dx = p.x - mouse.x;
            float dy = p.y - mouse.y;
            float d = std::sqrt(dx * dx + dy * dy);

            if (d < best) {
                best = d;
                idx = i;
            }
        }
        return idx;
    }

    sf::Color springColor(double ratio) {
        if (ratio > 1.0)
            return sf::Color(255, 80, 80);
        if (ratio < 1.0)
            return sf::Color(80, 120, 255);
        return sf::Color(200, 200, 200);
    }

    void draw() {
        window.clear(sf::Color(30, 30, 30));

        for (int i = 0; i < system.GetSpringCount(); i++) {
            auto s = system.GetSpring(i);

            auto* a = system.GetParticle(s->p1);
            auto* b = system.GetParticle(s->p2);

            auto p1 = toScreen(a->position);
            auto p2 = toScreen(b->position);

            double dist = (a->position - b->position).Length();
            double ratio = dist / s->restLength;

            sf::Vertex line[] = {
                sf::Vertex(p1, springColor(ratio)),
                sf::Vertex(p2, springColor(ratio))
            };

            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (int i = 0; i < system.GetParticleCount(); i++) {
            sf::CircleShape c(5);

            auto* p = system.GetParticle(i);

            if (i == dragged)
                c.setFillColor(sf::Color::Yellow);
            else if (p->fixed)
                c.setFillColor(sf::Color::Red);
            else
                c.setFillColor(sf::Color::Cyan);

            auto pos = toScreen(p->position);
            c.setPosition({pos.x - 5, pos.y - 5});

            window.draw(c);
        }

        window.draw(uiText);
        window.display();
    }

public:
    SimulationMassSpring(MassSpringSystem& sys)
        : system(sys), uiText(font) {

        window.create(sf::VideoMode({800, 600}), "Mass-Spring Simulation");
        window.setFramerateLimit(60);

        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                std::cerr << "Warning: Failed to load font.\n";
            }
        }

        uiText.setFont(font);
        uiText.setCharacterSize(16);
        uiText.setFillColor(sf::Color::White);
        uiText.setPosition({10, 10});
    }

    void run() {
        while (window.isOpen()) {

            while (const std::optional ev = window.pollEvent()) {

                if (ev->is<sf::Event::Closed>())
                    window.close();

                if (auto* e = ev->getIf<sf::Event::MouseButtonPressed>()) {
                    if (e->button == sf::Mouse::Button::Left)
                        dragged = findClosest(sf::Mouse::getPosition(window));
                }

                if (auto* e = ev->getIf<sf::Event::MouseButtonReleased>()) {
                    if (e->button == sf::Mouse::Button::Left)
                        dragged = -1;
                }
            }

            if (dragged != -1) {
                auto m = sf::Mouse::getPosition(window);
                auto* p = system.GetParticle(dragged);
                p->position = fromScreen(m);
                p->velocity = Vector3D(0, 0, 0);
            }

            uiText.setString(
                "Mass-Spring System\n"
                "-------------------\n"
                "LMB: drag particle\n"
                "Red: fixed points\n"
                "Blue/Red springs: compression/stretch\n"
                "Yellow: selected particle\n"
            );

            system.Update();
            draw();
        }
    }
};