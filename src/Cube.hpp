#pragma once

#include <array>
#include <optional>
#include <iostream>

#include <sfml/Graphics.hpp>

class Cube {
    sf::Vector3f m_position{};
    float m_size{ 10.f };

    sf::Vector3f m_i{ 1.f, 0.f, 0.f };
    sf::Vector3f m_j{ 0.f, 1.f, 0.f };
    sf::Vector3f m_k{ 0.f, 0.f, 1.f };

    std::array<sf::Color, 6> m_faceColors{
        sf::Color::Red,
        sf::Color::Blue,
        sf::Color { 255, 165, 0 },
        sf::Color::Green,
        sf::Color::White,
        sf::Color::Yellow
    };

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
            m_vertices[t] = m_position +
                (m_size / 2.f) * (static_cast<float>(std::get<0>(vertexOffsets[t])) * m_i +
                    static_cast<float>(std::get<1>(vertexOffsets[t])) * m_j +
                    static_cast<float>(std::get<2>(vertexOffsets[t])) * m_k);
        }
    }

public:
    static constexpr std::array<std::array<int, 4>, 6> faceVertexData{ {
        { 0, 1, 2, 3 }, // front
        { 1, 5, 6, 2 }, // right
        { 5, 4, 7, 6 }, // back
        { 4, 0, 3, 7 }, // left
        { 0, 1, 5, 4 }, // top
        { 3, 2, 6, 7 } // bottom
    } };

    const std::array<sf::Color, 6>& faceColors() const {
        return m_faceColors;
    }

    const std::array<sf::Vector3f, 8>& vertices() const {
        return m_vertices;
    }

    const sf::Vector3f& position() const {
        return m_position;
    }

    void setPosition(sf::Vector3f pos) {
        m_position = pos;
    }

    Cube(sf::Vector3f position = { 0.0, 0.0, 0.0 }, float size = 5.f)
        : m_position(position), m_size(size) {
        updateVertexPositions();
    }

    void setFaceColor(int face, const sf::Color& color) {
        if (face >= 0 && face < 6) {
            m_faceColors[face] = color;
        }
    }

    void rotate(float angle, sf::Vector3f axis, std::optional<sf::Vector3f> center = std::nullopt) {
        m_i += (axis.normalized() * angle).cross(m_i);
        m_j += (axis.normalized() * angle).cross(m_j);

        m_i = m_i.normalized();
        m_k = m_i.cross(m_j).normalized();
        m_j = m_k.cross(m_i).normalized();


        if (center) {
            float length{ (m_position - center.value()).length() };
            sf::Vector3f vec{ (axis.normalized() * angle).cross(m_position - center.value()) };

            m_position += center.value() + vec;
            if (length > 0.f)
                m_position = center.value() + (m_position - center.value()).normalized() * length;
        }

        updateVertexPositions();
    }
};
