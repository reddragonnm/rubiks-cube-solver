#pragma once

#include <array>
#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>

#include "RenderCube.hpp"

#include <sfml/Graphics.hpp>


// forward declarations
class Cube;
namespace Solver {
    void moveCube(Cube& cube, char move);
};


class Cube : public RenderCube {
public:
    enum State {
        IDLE,
        ROTATING,
    };

private:
    State m_state{ IDLE };
    float m_rotationAngle{ 0.f };
    std::vector<char> m_rotationQueue{};
    std::array<RenderCube, 2> m_renderCubes{};
    sf::Vector3f m_rotationAxis{};

    void rotateFace(int face) {
        for (int i = 0; i < numSides; i++) {
            for (int j = 0; j < i; j++) {
                std::swap(faceColors[face][i][j], faceColors[face][j][i]);
            }
        }

        for (int i = 0; i < numSides; i++) {
            std::reverse(faceColors[face][i].begin(), faceColors[face][i].end());
        }
    }

    void U(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(4);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ faceColors[1][0][i] };

                for (int j : { 0, 3, 2, 1 }) {
                    std::swap(last, faceColors[j][0][i]);
                }

            }
        }
    }

    void D(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(5);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ faceColors[3][numSides - 1][i] };

                for (int j : { 0, 1, 2, 3 }) {
                    std::swap(last, faceColors[j][numSides - 1][i]);
                }
            }
        }
    }

    void L(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(3);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ faceColors[2][numSides - i - 1][numSides - 1] };

                for (int j : { 4, 0, 5 }) {
                    std::swap(last, faceColors[j][i][0]);
                }
                std::swap(last, faceColors[2][numSides - i - 1][numSides - 1]);
            }
        }
    }

    void R(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(1);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ faceColors[2][numSides - i - 1][0] };

                for (int j : { 5, 0, 4 }) {
                    std::swap(last, faceColors[j][i][numSides - 1]);
                }
                std::swap(last, faceColors[2][numSides - i - 1][0]);
            }
        }
    }

    void F(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(0);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ faceColors[4][numSides - 1][i] };

                std::swap(last, faceColors[1][i][0]);
                std::swap(last, faceColors[5][0][numSides - 1 - i]);
                std::swap(last, faceColors[3][numSides - 1 - i][numSides - 1]);
                std::swap(last, faceColors[4][numSides - 1][i]);
            }
        }
    }

    void B(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(2);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ faceColors[4][0][i] };

                std::swap(last, faceColors[3][numSides - 1 - i][0]);
                std::swap(last, faceColors[5][numSides - 1][numSides - 1 - i]);
                std::swap(last, faceColors[1][i][numSides - 1]);
                std::swap(last, faceColors[4][0][i]);
            }
        }
    }

    void newRotation(char rotationType) {
        m_state = ROTATING;

        m_renderCubes[0] = RenderCube{ m_cubeletSize, m_i, m_j, m_k };
        m_renderCubes[1] = RenderCube{ m_cubeletSize, m_i, m_j, m_k };

        m_renderCubes[0].faceColors = faceColors;
        m_renderCubes[1].setFaceColorEmpty();

        auto& a{ m_renderCubes[0].faceColors };
        auto& b{ m_renderCubes[1].faceColors };

        if (rotationType == 'U') {
            m_rotationAxis = -getFaceNormal(4);
            std::swap(a[4], b[4]);

            for (int i = 0; i < numSides; i++) {
                for (int j = 0; j <= 3; j++) {
                    std::swap(a[j][0][i], b[j][0][i]);
                }
            }
        }
        else if (rotationType == 'D') {
            m_rotationAxis = -getFaceNormal(5);
            std::swap(a[5], b[5]);

            for (int i = 0; i < numSides; i++) {
                for (int j = 0; j <= 3; j++) {
                    std::swap(a[j][numSides - 1][i], b[j][numSides - 1][i]);
                }
            }
        }
        else if (rotationType == 'L') {
            m_rotationAxis = -getFaceNormal(3);
            std::swap(a[3], b[3]);

            for (int i = 0; i < numSides; i++) {
                for (int j : {0, 4, 5}) {
                    std::swap(a[j][i][0], b[j][i][0]);
                }
                std::swap(a[2][i][numSides - 1], b[2][i][numSides - 1]);
            }
        }
        else if (rotationType == 'R') {
            m_rotationAxis = -getFaceNormal(1);
            std::swap(a[1], b[1]);

            for (int i = 0; i < numSides; i++) {
                for (int j : {0, 4, 5}) {
                    std::swap(a[j][i][numSides - 1], b[j][i][numSides - 1]);
                }
                std::swap(a[2][i][0], b[2][i][0]);
            }
        }
        else if (rotationType == 'F') {
            m_rotationAxis = -getFaceNormal(0);
            std::swap(a[0], b[0]);

            for (int i = 0; i < numSides; i++) {
                std::swap(a[4][numSides - 1][i], b[4][numSides - 1][i]);
                std::swap(a[1][i][0], b[1][i][0]);
                std::swap(a[5][0][i], b[5][0][i]);
                std::swap(a[3][i][numSides - 1], b[3][i][numSides - 1]);
            }
        }
        else if (rotationType == 'B') {
            m_rotationAxis = -getFaceNormal(2);
            std::swap(a[2], b[2]);

            for (int i = 0; i < numSides; i++) {
                std::swap(a[4][0][i], b[4][0][i]);
                std::swap(a[1][i][numSides - 1], b[1][i][numSides - 1]);
                std::swap(a[5][numSides - 1][i], b[5][numSides - 1][i]);
                std::swap(a[3][i][0], b[3][i][0]);
            }

        }
    }


public:
    Cube(float cubeletSize) : RenderCube{ cubeletSize } {}

    friend void Solver::moveCube(Cube& cube, char move);

    void shuffle(int numberOfRotations) {
        for (int i = 0; i < numberOfRotations; i++) {
            startRotation("UDFBLR"[rand() % 6]);
        }
    }

    const std::array<RenderCube, 2>& getRenderCubes() const {
        return m_renderCubes;
    }

    const State& getState() const {
        return m_state;
    }

    void startRotation(char type) {
        m_rotationQueue.push_back(type);

        if (m_state == IDLE) {
            newRotation(type);
        }
    }

    void manageRotation(float rotationSpeed) {
        if (m_state == ROTATING) {
            m_rotationAngle += rotationSpeed;
            m_renderCubes[1].rotate(rotationSpeed * (M_PI / 180.f), m_rotationAxis);

            if (m_rotationAngle >= 90.f) {
                m_rotationAngle = 0.f;

                switch (m_rotationQueue.front()) {
                case 'U': U(); break;
                case 'D': D(); break;
                case 'L': L(); break;
                case 'R': R(); break;
                case 'F': F(); break;
                case 'B': B(); break;
                default:
                    std::cerr << "Invalid rotation type: " << m_rotationQueue.front() << std::endl;
                    break;
                }

                m_rotationQueue.erase(m_rotationQueue.begin());

                if (m_rotationQueue.empty()) {
                    m_state = IDLE;
                }
                else {
                    newRotation(m_rotationQueue.front());
                }
            }
        }
    }

    const sf::Vector3f& getRotationAxis() const {
        return m_rotationAxis;
    }
};
