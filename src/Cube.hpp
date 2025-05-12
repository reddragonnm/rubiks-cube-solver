#pragma once

#include <array>
#include <cassert>
#include <iostream>

#include <sfml/Graphics.hpp>

constexpr int numSides{ 3 };

using FaceColors = std::array<std::array<std::array<sf::Color, numSides>, numSides>, 6>;
using Vertices = std::array<sf::Vector3f, 8>;

class Cube {
    float m_cubeletSize{ 10.f };

    sf::Vector3f m_i{ 1.f, 0.f, 0.f };
    sf::Vector3f m_j{ 0.f, 1.f, 0.f };
    sf::Vector3f m_k{ 0.f, 0.f, 1.f };

    FaceColors m_faceColors;

    std::array<sf::Vector3f, 8> m_vertices{};

    static constexpr std::array<std::tuple<int, int, int>, 8> vertexOffsets{ {
        { -1, 1, 1 },
        { 1, 1, 1 },
        { 1, -1, 1 },
        { -1, -1, 1 },
        { -1, 1, -1 },
        { 1, 1, -1 },
        { 1, -1, -1 },
        { -1, -1, -1 },
    } };

    void updateVertexPositions() {
        for (int t = 0; t < 8; t++) {
            auto [a, b, c] = vertexOffsets[t];

            m_vertices[t] = ((m_cubeletSize * numSides) / 2.f) * (static_cast<float>(a) * m_i +
                static_cast<float>(b) * m_j +
                static_cast<float>(c) * m_k);
        }
    }

    void rotateFace(int face) {
        for (int i = 0; i < numSides; i++) {
            for (int j = 0; j < i; j++) {
                std::swap(m_faceColors[face][i][j], m_faceColors[face][j][i]);
            }
        }

        for (int i = 0; i < numSides; i++) {
            std::reverse(m_faceColors[face][i].begin(), m_faceColors[face][i].end());
        }
    }

public:
    static constexpr std::array<std::array<int, 4>, 6> faceVertexData{ {
        { 0, 1, 2, 3 }, // front
        { 1, 5, 6, 2 }, // right
        { 5, 4, 7, 6 }, // back
        { 4, 0, 3, 7 }, // left
        { 4, 5, 1, 0 }, // top
        { 3, 2, 6, 7 } // bottom
    } };

    Cube(float cubeletSize = 5.f) : m_cubeletSize(cubeletSize) {
        setFaceColorDefault();
        updateVertexPositions();
    }

    void rotate(float angle, sf::Vector3f axis) {
        m_i += (axis.normalized() * angle).cross(m_i);
        m_j += (axis.normalized() * angle).cross(m_j);

        m_i = m_i.normalized();
        m_k = m_i.cross(m_j).normalized();
        m_j = m_k.cross(m_i).normalized();

        updateVertexPositions();
    }

    void setFaceColorDefault() {
        static constexpr std::array<sf::Color, 6> colors{
            sf::Color::Red,
            sf::Color::Blue,
            sf::Color { 255, 165, 0 },
            sf::Color::Green,
            sf::Color::White,
            sf::Color::Yellow
        };

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < numSides; j++) {
                for (int k = 0; k < numSides; k++) {
                    m_faceColors[i][j][k] = colors[i];
                }
            }
        }
    }

    sf::Vector3f getFaceNormal(int face) const {

        switch (face) {
        case 0: return m_k;
        case 1: return m_i;
        case 2: return -m_k;
        case 3: return -m_i;
        case 4: return m_j;
        case 5: return -m_j;
        default:
            assert(false && "Invalid face index");
            return sf::Vector3f{};
        }
    }

    const FaceColors& getFaceColors() const {
        return m_faceColors;
    }

    const sf::Vector3f& getBasis(int n) const {
        assert(n >= 0 && n < 3 && "Basis index out of range");

        if (n == 0) return m_i;
        else if (n == 1) return m_j;
        else if (n == 2) return m_k;
    }

    const Vertices& getVertices() const {
        return m_vertices;
    }

    void U(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(4);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ m_faceColors[1][0][i] };

                for (int j : { 0, 3, 2, 1 }) {
                    std::swap(last, m_faceColors[j][0][i]);
                }

            }
        }
    }

    void D(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(5);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ m_faceColors[3][numSides - 1][i] };

                for (int j : { 0, 1, 2, 3 }) {
                    std::swap(last, m_faceColors[j][numSides - 1][i]);
                }
            }
        }
    }

    void L(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(3);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ m_faceColors[2][i][numSides - 1] };

                for (int j : { 4, 0, 5 }) {
                    std::swap(last, m_faceColors[j][i][0]);
                }
                std::swap(last, m_faceColors[2][i][numSides - 1]);
            }
        }
    }

    void R(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(1);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ m_faceColors[2][i][0] };

                for (int j : { 4, 0, 5 }) {
                    std::swap(last, m_faceColors[j][i][numSides - 1]);
                }
                std::swap(last, m_faceColors[2][i][0]);
            }
        }
    }

    void F(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(0);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ m_faceColors[4][numSides - 1][i] };

                std::swap(last, m_faceColors[1][i][0]);
                std::swap(last, m_faceColors[5][0][numSides - 1 - i]);
                std::swap(last, m_faceColors[3][numSides - 1 - i][numSides - 1]);
                std::swap(last, m_faceColors[4][numSides - 1][i]);
            }
        }
    }

    void B(bool clockwise = true) {
        for (int t = 0; t < (clockwise ? 1 : 3); t++) {
            rotateFace(0);

            for (int i = 0; i < numSides; i++) {
                sf::Color last{ m_faceColors[4][0][i] };

                std::swap(last, m_faceColors[1][i][numSides - 1]);
                std::swap(last, m_faceColors[5][numSides - 1][numSides - 1 - i]);
                std::swap(last, m_faceColors[3][numSides - 1 - i][0]);
                std::swap(last, m_faceColors[4][0][i]);
            }
        }
    }
};
