#pragma once

#include <cmath>

#include <sfml/Graphics.hpp>

#include "Constants.hpp"
#include "Cube.hpp"

using CubeData = std::array<std::array<std::array<Cube, Constants::numSides>, Constants::numSides>, Constants::numSides>;


class RubiksCube {
    sf::Vector3f m_i{ 1.f, 0.f, 0.f };
    sf::Vector3f m_j{ 0.f, 1.f, 0.f };
    sf::Vector3f m_k{ 0.f, 0.f, 1.f };

    float m_cubeSize{ 2.f };

    static constexpr std::array<sf::Color, 6> m_faceColors{
        sf::Color::Red,
        sf::Color::Blue,
        sf::Color { 255, 165, 0 },
        sf::Color::Green,
        sf::Color::White,
        sf::Color::Yellow
    };

    CubeData m_cubes;

public:
    RubiksCube(float cubeSize = 2.f) : m_cubeSize(cubeSize) {
        float offset{ (Constants::numSides - 1) / 2.f };
        for (int i = 0; i < Constants::numSides; i++) {
            for (int j = 0; j < Constants::numSides; j++) {
                for (int k = 0; k < Constants::numSides; k++) {
                    m_cubes[i][j][k] = Cube{
                        cubeSize * sf::Vector3f{i - offset, j - offset, k - offset},
                        cubeSize,
                    };
                }
            }
        }
    }

    const CubeData& cubes() const {
        return m_cubes;
    }

    void rotate(float angle, const sf::Vector3f& axis) {
        m_i += (axis.normalized() * angle).cross(m_i);
        m_j += (axis.normalized() * angle).cross(m_j);

        m_i = m_i.normalized();
        m_k = m_i.cross(m_j).normalized();
        m_j = m_k.cross(m_i).normalized();

        for (auto& cubeLayer : m_cubes) {
            for (auto& cubeRow : cubeLayer) {
                for (auto& cube : cubeRow) {
                    cube.rotate(angle, axis);
                }
            }
        }
        // setCubePositions();

        float offset{ (Constants::numSides - 1) / 2.f };
        for (int i = 0; i < Constants::numSides; i++) {
            for (int j = 0; j < Constants::numSides; j++) {
                for (int k = 0; k < Constants::numSides; k++) {
                    m_cubes[i][j][k].setPosition(
                        m_cubeSize * ((i - offset) * m_i +
                            (j - offset) * m_j +
                            (k - offset) * m_k)
                    );
                }
            }
        }
    }
};
