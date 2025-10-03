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
        int powOf3{ 3 * 3 * 3 * 3 * 3 * 3 };

        for (int i = 0; i < 7; i++) { // DRB not considered
            for (int j = 0; j < 3; j++) {
                const int face{ faceIndex.at(corners[i][j * 2]) };
                const auto [a, b] = positions[corners[i][j * 2 + 1] - '0'];
                const sf::Color color{ colors[face][a][b] };

                if (color == colors[4][1][1] || color == colors[5][1][1]) { // up or down color
                    ans += j * powOf3; // i will be the orientation
                    powOf3 /= 3;

                    break;
                }
            }
        }

        return ans;
    }

    std::vector<char> solve(const Cube& cube) {
        return {};
    }
}
