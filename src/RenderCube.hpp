#pragma once

#include <array>
#include <cassert>
#include <iostream>

#include <sfml/Graphics.hpp>

constexpr int numSides{ 3 };

using FaceColors = std::array<std::array<std::array<sf::Color, numSides>, numSides>, 6>;
using Vertices = std::array<sf::Vector3f, 8>;

class RenderCube {
protected:
    float m_cubeletSize{ 10.f };

    sf::Vector3f m_i{ 1.f, 0.f, 0.f };
    sf::Vector3f m_j{ 0.f, 1.f, 0.f };
    sf::Vector3f m_k{ 0.f, 0.f, 1.f };

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

public:
    FaceColors faceColors;

    static constexpr std::array<std::array<int, 4>, 6> faceVertexData{ {
        { 0, 1, 2, 3 }, // front
        { 1, 5, 6, 2 }, // right
        { 5, 4, 7, 6 }, // back
        { 4, 0, 3, 7 }, // left
        { 4, 5, 1, 0 }, // top
        { 3, 2, 6, 7 } // bottom
    } };

    RenderCube(float cubeletSize = 5.f) : m_cubeletSize(cubeletSize) {
        setFaceColorDefault();
        updateVertexPositions();
    }

    RenderCube(float cubeletSize, sf::Vector3f i, sf::Vector3f j, sf::Vector3f k) :
        m_cubeletSize(cubeletSize), m_i(i), m_j(j), m_k(k) {
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
                    faceColors[i][j][k] = colors[i];
                }
            }
        }
    }

    void setFaceColorEmpty() {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < numSides; j++) {
                for (int k = 0; k < numSides; k++) {
                    faceColors[i][j][k] = sf::Color::Black;
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

    const sf::Vector3f& getBasis(int n) const {
        assert(n >= 0 && n < 3 && "Basis index out of range");

        if (n == 0) return m_i;
        else if (n == 1) return m_j;
        else if (n == 2) return m_k;
    }

    const Vertices& getVertices() const {
        return m_vertices;
    }
};
