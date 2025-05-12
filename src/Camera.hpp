#pragma once

#include <array>
#include <iostream>
#include <algorithm>

#include <sfml/Graphics.hpp>

#include "Constants.hpp"
#include "Cube.hpp"
#include "RubiksCube.hpp"

class Camera {
    int m_imageWidth{};
    int m_imageHeight{};

    float viewportWidth{};
    float viewportHeight{};

    sf::Vector3f m_center{ 0.f, 0.f, 20.f };
    sf::Vector3f m_viewPortCenter{ 0.f, 0.f, 10.f };

    sf::Vector2f pointOnViewport(const sf::Vector3f& v) const {
        sf::Vector3f scaled{ (v - m_center) * ((m_viewPortCenter.z - m_center.z) / (v.z - m_center.z)) };
        return sf::Vector2f{ scaled.x, scaled.y };
    }

    sf::Vector2f viewportToScreen(const sf::Vector2f& v) const {
        return sf::Vector2f{ ((v.x / viewportWidth) + 0.5f) * m_imageWidth,
                             ((-v.y / viewportHeight) + 0.5f) * m_imageHeight };
    }

public:
    Camera(int imageWidth, int imageHeight) : m_imageWidth(imageWidth), m_imageHeight(imageHeight) {
        viewportWidth = 6.f;
        viewportHeight = viewportWidth * static_cast<float>(m_imageHeight) / static_cast<float>(m_imageWidth);
    }

    void drawCube(sf::RenderWindow& win, const Cube& cube) const {
        // TODO: Rewrite to use normals

        std::array<int, 6> faces{ 0, 1, 2, 3, 4, 5 };

        const auto& vertices{ cube.vertices() };
        const auto& faceVertexData{ cube.faceVertexData };
        std::sort(faces.begin(), faces.end(), [&vertices, &faceVertexData](int a, int b) {
            const auto& aVertices{ faceVertexData[a] };
            const auto& bVertices{ faceVertexData[b] };

            float aCenterZ{ 0.f };
            float bCenterZ{ 0.f };

            for (const auto& vertex : aVertices) {
                aCenterZ += vertices[vertex].z;
            }
            for (const auto& vertex : bVertices) {
                bCenterZ += vertices[vertex].z;
            }

            return aCenterZ < bCenterZ;
            });

        for (const auto& face : faces) {
            const auto& vertices{ cube.faceVertexData[face] };
            const auto& color{ cube.faceColors()[face] };

            sf::ConvexShape quad, quad2;
            quad.setPointCount(4);
            quad.setFillColor(color);


            for (int i = 0; i < 4; ++i) {
                const auto& vertex{ vertices[i] };
                const auto& screenPoint{ viewportToScreen(pointOnViewport(cube.vertices()[vertex])) };

                quad.setPoint(i, screenPoint);
            }

            win.draw(quad);
        }
    }

    void drawRubiksCube(sf::RenderWindow& win, RubiksCube& rubiksCube) const {
        std::array<Cube, Constants::numSides* Constants::numSides* Constants::numSides> cubes{};
        int index{ 0 };
        for (int i = 0; i < Constants::numSides; i++) {
            for (int j = 0; j < Constants::numSides; j++) {
                for (int k = 0; k < Constants::numSides; k++) {
                    cubes[index++] = rubiksCube.cubes()[i][j][k];
                }
            }
        }
        std::sort(cubes.begin(), cubes.end(), [](const Cube& a, const Cube& b) {
            return a.vertices()[0].z < b.vertices()[0].z;
            });
        for (const auto& cube : cubes) {
            drawCube(win, cube);
        }
    }
};
