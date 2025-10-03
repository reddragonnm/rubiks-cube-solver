#pragma once

#include <array>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <sfml/Graphics.hpp>

#include "RenderCube.hpp"
#include "Cube.hpp"

namespace Solver {
    constexpr std::array<std::string_view, 8> corners{
        "U2R0F1",
        "U3F0L1",
        "U0L0B1",
        "U1B0R1",
        "D1F2R2",
        "D0L2F3",
        "D3B2L3",
        "D2R2B3"
    };

    constexpr std::array<std::string_view, 12> edges{
        "U1R0",
        "U2F0",
        "U3L0",
        "U0B0",
        "D1R2",
        "D0F2",
        "D3L2",
        "D2B2",
        "F1R3",
        "F3L1",
        "B1L3",
        "B3R1"
    };

    const std::map<char, int> faceIndex{
        {'F', 0},
        {'R', 1},
        {'B', 2},
        {'L', 3},
        {'U', 4},
        {'D', 5}
    };

    int getCornerOrientation(const FaceColors& colors) {
        static constexpr std::array<std::pair<int, int>, 4> positions{ {
            {0, 0},
            {0, 2},
            {2, 2},
            {2, 0}
        } };

        int ans{ 0 };

        for (int i = 0; i < 7; i++) { // DRB not considered
            for (int j = 0; j < 3; j++) {
                const int face{ faceIndex.at(corners[i][j * 2]) };
                const auto [a, b] = positions[corners[i][j * 2 + 1] - '0'];
                const sf::Color color{ colors[face][a][b] };

                if (color == defaultFaceColors[4] || color == defaultFaceColors[5]) { // up or down color
                    ans = ans * 3 + j; // i will be the orientation
                    break;
                }
            }
        }

        return ans;
    }

    std::string debugColorName(const sf::Color& color) {
        if (color == sf::Color::Red) return "Red";
        if (color == sf::Color::Blue) return "Blue";
        if (color == sf::Color{ 255, 165, 0 }) return "Orange";
        if (color == sf::Color::Green) return "Green";
        if (color == sf::Color::White) return "White";
        if (color == sf::Color::Yellow) return "Yellow";
        return "Unknown";
    }

    int getEdgeOrientation(const FaceColors& colors) {
        static constexpr std::array<std::pair<int, int>, 4> positions{ {
            {0, 1},
            {1, 2},
            {2, 1},
            {1, 0}
        } };

        int ans{ 0 };

        for (int i = 0; i < 11; i++) { // BR not considered
            const auto [a1, b1] { positions[edges[i][1] - '0'] };
            const sf::Color color1{ colors[faceIndex.at(edges[i][0])][a1][b1] };

            const auto [a2, b2] { positions[edges[i][3] - '0'] };
            const sf::Color color2{ colors[faceIndex.at(edges[i][2])][a2][b2] };

            int x{ 1 };

            if (color1 == defaultFaceColors[4] || color1 == defaultFaceColors[5]) x = 0;
            else if (color2 == defaultFaceColors[4] || color2 == defaultFaceColors[5]) x = 1;
            else if (color1 == defaultFaceColors[0] || color1 == defaultFaceColors[2]) x = 0;
            else x = 1;


            ans = ans * 2 + x;
        }

        return ans;
    }

    std::vector<char> solve(const Cube& cube) {
        return {};
    }
}
