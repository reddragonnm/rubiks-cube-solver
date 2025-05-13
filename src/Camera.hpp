#pragma once

#include <array>
#include <iostream>
#include <algorithm>

#include <sfml/Graphics.hpp>

#include "RenderCube.hpp"
#include "Cube.hpp"

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

    void drawCube(sf::RenderWindow& win, const RenderCube& cube) const {
        std::array<int, 6> faceOrder{ 0, 1, 2, 3, 4, 5 };
        std::sort(faceOrder.begin(), faceOrder.end(), [&cube](int a, int b) {
            return cube.getFaceNormal(a).dot({ 0.f, 0.f, -1.f }) > cube.getFaceNormal(b).dot({ 0.f, 0.f, -1.f });
            });

        for (const int& i : faceOrder) {
            if (cube.getFaceNormal(i).dot({ 0.f, 0.f, -1.f }) > 0.f) continue;

            const auto& vertexData{ Cube::faceVertexData[i] };
            const auto& vertices{ cube.getVertices() };

            sf::Vector3f v0{ vertices[vertexData[0]] };

            sf::Vector3f x{ (vertices[vertexData[1]] - v0) / static_cast<float>(numSides) };
            sf::Vector3f y{ (vertices[vertexData[3]] - v0) / static_cast<float>(numSides) };

            for (int j = 0; j < numSides; j++) {
                for (int k = 0; k < numSides; k++) {
                    sf::Color faceColor{ cube.faceColors[i][j][k] };
                    if (faceColor == sf::Color::Black) continue;

                    sf::ConvexShape face;
                    face.setPointCount(4);
                    face.setFillColor(faceColor);

                    sf::Vector3 base{ v0 + (y * static_cast<float>(j)) + (x * static_cast<float>(k)) };

                    face.setPoint(0, viewportToScreen(pointOnViewport(base)));
                    face.setPoint(1, viewportToScreen(pointOnViewport(base + x)));
                    face.setPoint(2, viewportToScreen(pointOnViewport(base + x + y)));
                    face.setPoint(3, viewportToScreen(pointOnViewport(base + y)));

                    face.setOutlineColor(sf::Color::Black);
                    face.setOutlineThickness(-2.f);

                    win.draw(face);
                }
            }
        }

    }

public:
    Camera(int imageWidth, int imageHeight, float centerZ = 20.f, float viewportZ = 10.f) : m_imageWidth(imageWidth), m_imageHeight(imageHeight) {
        viewportWidth = 8.f;
        viewportHeight = viewportWidth * static_cast<float>(m_imageHeight) / static_cast<float>(m_imageWidth);

        m_center.z = centerZ;
        m_viewPortCenter.z = viewportZ;
    }

    void draw(sf::RenderWindow& win, const Cube& cube) const {
        if (cube.getState() == Cube::IDLE) {
            drawCube(win, cube);
        }
        else if (cube.getState() == Cube::ROTATING) {
            if (cube.getRotationAxis().dot({ 0.f, 0.f, -1.f }) > 0.f) {
                drawCube(win, cube.getRenderCubes()[0]);
                drawCube(win, cube.getRenderCubes()[1]);
            }
            else {
                drawCube(win, cube.getRenderCubes()[1]);
                drawCube(win, cube.getRenderCubes()[0]);
            }
        }
    }
};
