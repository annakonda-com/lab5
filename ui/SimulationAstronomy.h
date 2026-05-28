#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <sstream>

#include "../include/Common/Vector3D.h"
#include "../Sequences/ListSequence.h"
#include "Observation.h"
#include "../include/Physics/Space/AstronomySimulation.h"

class SimulationAstronomy {
private:
    sf::RenderWindow window;
    AstronomySimulation sim;

    MutableListSequence<Observation>* observations;
    MutableListSequence<Vector3D>* points;
    MutableListSequence<sf::Vector2f>* screenPoints;

    enum Mode {
        INPUT_2D,
        VIEW_SPHERE
    } mode = INPUT_2D;

    Vector3D cameraPos1 = {-100, 0, 0};
    Vector3D cameraPos2 = {100, 0, 0};

    float yaw = 0;
    float pitch = 0;
    float zoom = 400.f;

    sf::Font font;
    sf::Text text;

    Vector3D ScreenToRay(float x, float y) {
        float nx = (x - 600) / 400.f;
        float ny = (y - 400) / 400.f;

        Vector3D d(nx, ny, 1.0);
        return d.Normalize();
    }

public:
    SimulationAstronomy()
        : window(sf::VideoMode({1200,800}), "3D Reconstruction"),
    text(font)
    {
        observations = new MutableListSequence<Observation>();
        points = new MutableListSequence<Vector3D>();
        screenPoints = new MutableListSequence<sf::Vector2f>();

        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                std::cerr << "Warning: Failed to load font.\n";
            }
        }

        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
    }

    ~SimulationAstronomy() {
        delete observations;
        delete points;
        delete screenPoints;
    }

    Vector3D RotateY(const Vector3D& v, float a) {
        return {v.x*cos(a) + v.z*sin(a), v.y, -v.x*sin(a) + v.z*cos(a)};
    }

    Vector3D RotateX(const Vector3D& v, float a) {
        return {v.x, v.y*cos(a) - v.z*sin(a), v.y*sin(a) + v.z*cos(a)};
    }

    sf::Vector2f Project(const Vector3D& v) {
        float k = zoom / (zoom + v.z);
        return {static_cast<float>(600 + v.x * k), static_cast<float>(400 + v.y * k)};
    }

    void HandleInput() {
        while (const auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
                if (k->scancode == sf::Keyboard::Scancode::Enter)
                    mode = VIEW_SPHERE;
            }

            if (mode == INPUT_2D) {
                if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (m->button == sf::Mouse::Button::Left) {
                        if (screenPoints->GetLength() < 2) {
                            screenPoints->Append(
                                {(float)m->position.x, (float)m->position.y}
                            );
                        }
                    }
                }
            }

            else if (mode == VIEW_SPHERE) {
                if (const auto* w = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    zoom -= w->delta * 20.f;
                    if (zoom < 100) zoom = 100;
                    if (zoom > 2000) zoom = 2000;
                }
            }
        }

        if (mode == VIEW_SPHERE) {
            float rotSpeed = 0.02f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
                yaw -= rotSpeed;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
                yaw += rotSpeed;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
                pitch -= rotSpeed;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
                pitch += rotSpeed;

            if (pitch > 1.5f) pitch = 1.5f;
            if (pitch < -1.5f) pitch = -1.5f;
        }
    }

    void BuildObservations() {

        if (screenPoints->GetLength() < 2) return;
        if (points->GetLength() > 0) return;

        auto p1 = screenPoints->Get(0);
        auto p2 = screenPoints->Get(1);

        Vector3D d1 = ScreenToRay(p1.x, p1.y);
        Vector3D d2 = ScreenToRay(p2.x, p2.y);

        d1 = RotateY(d1, yaw);
        d1 = RotateX(d1, pitch);

        d2 = RotateY(d2, yaw);
        d2 = RotateX(d2, pitch);

        observations->Append(Observation(cameraPos1, d1));
        observations->Append(Observation(cameraPos2, d2));

        Vector3D res = sim.RecoverPosition(
            cameraPos1, d1,
            cameraPos2, d2
        );

        points->Append(res);
    }

    void DrawGrid() {
        int N = 10;
        float step = 40;

        for (int i = -N; i <= N; i++) {

            Vector3D a(i*step, -N*step, 0);
            Vector3D b(i*step,  N*step, 0);

            Vector3D c(-N*step, i*step, 0);
            Vector3D d( N*step, i*step, 0);

            a = RotateY(a, yaw); a = RotateX(a, pitch);
            b = RotateY(b, yaw); b = RotateX(b, pitch);
            c = RotateY(c, yaw); c = RotateX(c, pitch);
            d = RotateY(d, yaw); d = RotateX(d, pitch);

            sf::Vertex line1[] = {
                sf::Vertex(Project(a), sf::Color(80,80,80)),
                sf::Vertex(Project(b), sf::Color(80,80,80))
            };

            sf::Vertex line2[] = {
                sf::Vertex(Project(c), sf::Color(80,80,80)),
                sf::Vertex(Project(d), sf::Color(80,80,80))
            };

            window.draw(line1, 2, sf::PrimitiveType::Lines);
            window.draw(line2, 2, sf::PrimitiveType::Lines);
        }
    }

    void DrawSphere() {
        for (int i = 0; i < 40; i++) {
            for (int j = 0; j < 40; j++) {

                float t = i * 3.1415f / 40;
                float p = j * 2 * 3.1415f / 40;

                Vector3D v(
                    150*cos(p)*sin(t),
                    150*sin(p)*sin(t),
                    150*cos(t)
                );

                v = RotateY(v, yaw);
                v = RotateX(v, pitch);

                auto s = Project(v);

                sf::CircleShape dot(1);
                dot.setPosition(s);
                dot.setFillColor(sf::Color(100,100,255));
                window.draw(dot);
            }
        }
    }

    void DrawObservations() {
        for (int i = 0; i < observations->GetLength(); i++) {

            auto obs = observations->Get(i);

            Vector3D start = obs.observerPos;
            Vector3D end = obs.observerPos + obs.direction * 300;

            start = RotateY(start, yaw);
            start = RotateX(start, pitch);

            end = RotateY(end, yaw);
            end = RotateX(end, pitch);

            auto s1 = Project(start);
            auto s2 = Project(end);

            sf::Vertex line[] = {
                sf::Vertex(s1, sf::Color::Red),
                sf::Vertex(s2, sf::Color::Red)
            };

            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }

    void DrawRecoveredPoint() {
        if (points->GetLength() == 0) return;

        Vector3D p = points->Get(0);

        p = RotateY(p, yaw);
        p = RotateX(p, pitch);

        auto s = Project(p);

        sf::CircleShape dot(6);
        dot.setPosition(s);
        dot.setFillColor(sf::Color::Green);

        window.draw(dot);
    }

    void DrawScreenPoints() {
        for (int i = 0; i < screenPoints->GetLength(); i++) {
            auto p = screenPoints->Get(i);

            sf::CircleShape dot(5);
            dot.setPosition({p.x, p.y});
            dot.setFillColor(sf::Color::Yellow);
            window.draw(dot);

            sf::Text text(font);
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::White);

            text.setString(
                "(" + std::to_string((int)p.x) + ", " +
                std::to_string((int)p.y) + ")"
            );

            text.setPosition({p.x + 10, p.y});
            window.draw(text);
        }
    }

    void DrawUI() {

        std::stringstream ss;

        if (mode == INPUT_2D) {
            ss << "STEP 1: Click 2 points\n";
            ss << "Press ENTER\n";
            ss << "Points: " << screenPoints->GetLength();
        }
        else {
            ss << "STEP 2: Sphere view\n";
            ss << "W/S - up/down\nA/D - left/right\nWheel - zoom\n";

            if (points->GetLength() > 0) {
                auto p = points->Get(0);
                ss << "\n3D Point:\n";
                ss << "X=" << p.x << " Y=" << p.y << " Z=" << p.z;
            }
        }

        text.setString(ss.str());
        text.setPosition({10,10});
        window.draw(text);
    }

    void DrawAxes() {
        float L = 200;

        struct Axis {
            Vector3D dir;
            sf::Color color;
            std::string name;
        };

        Axis axes[3] = {
            {{L,0,0}, sf::Color::Red, "X"},
            {{0,L,0}, sf::Color::Green, "Y"},
            {{0,0,L}, sf::Color::Blue, "Z"}
        };

        for (auto& a : axes) {

            Vector3D start(0,0,0);
            Vector3D end = a.dir;

            start = RotateY(start, yaw);
            start = RotateX(start, pitch);

            end = RotateY(end, yaw);
            end = RotateX(end, pitch);

            auto s1 = Project(start);
            auto s2 = Project(end);

            sf::Vertex line[] = {
                sf::Vertex(s1, a.color),
                sf::Vertex(s2, a.color)
            };

            window.draw(line, 2, sf::PrimitiveType::Lines);

            sf::Text label(font);
            label.setCharacterSize(16);
            label.setFillColor(a.color);
            label.setString(a.name);
            label.setPosition(s2);
            window.draw(label);
        }
    }

    void DrawCameras() {

        struct Cam {
            Vector3D pos;
            std::string name;
            sf::Color color;
        };

        Cam cams[2] = {
            {cameraPos1, "Cam1", sf::Color::Cyan},
            {cameraPos2, "Cam2", sf::Color::Magenta}
        };

        for (auto& c : cams) {

            Vector3D p = c.pos;

            p = RotateY(p, yaw);
            p = RotateX(p, pitch);

            auto s = Project(p);

            sf::CircleShape dot(6);
            dot.setPosition(s);
            dot.setFillColor(c.color);
            window.draw(dot);

            sf::Text label(font);
            label.setCharacterSize(14);
            label.setFillColor(c.color);
            label.setString(c.name);

            label.setPosition({s.x + 8, s.y});
            window.draw(label);
        }
    }

    void Run() {
        while (window.isOpen()) {

            HandleInput();

            if (mode == VIEW_SPHERE)
                BuildObservations();

            window.clear(sf::Color(10,10,20));

            if (mode == INPUT_2D) {
                DrawScreenPoints();
            } else {
                DrawGrid();
                DrawAxes();
                DrawSphere();

                DrawCameras();
                DrawObservations();
                DrawRecoveredPoint();
            }
            DrawUI();
            window.display();
        }
    }
};