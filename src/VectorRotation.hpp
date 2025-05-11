#pragma once

#include <cmath>

#include <sfml/Graphics.hpp>


inline std::pair<float, float> getSinCos(float angle) {
    double rad{ angle * (M_PI / 180.0) };
    return { std::sin(rad), std::cos(rad) };
}

inline sf::Transform Rx(double angle) {
    auto [sin, cos] = getSinCos(angle);

    return sf::Transform{
        1, 0, 0,
        0, cos, -sin,
        0, sin, cos
    };
}

inline sf::Transform Ry(double angle) {
    auto [sin, cos] = getSinCos(angle);

    return sf::Transform{
        cos, 0, sin,
        0, 1, 0,
        -sin, 0, cos
    };
}

inline sf::Transform Rz(double angle) {
    auto [sin, cos] = getSinCos(angle);

    return sf::Transform{
        cos, -sin, 0,
        sin, cos, 0,
        0, 0, 1
    };
}
