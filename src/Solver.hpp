#pragma once

#include <array>
#include <unordered_set>
#include <vector>
#include <fstream>
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
        "D1F2R3",
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

    constexpr std::array<std::array<int, 4>, 12> nCr{ {
            // r = 0   1    2     3
            { 1,   0,   0,    0 },    // n = 0
            { 1,   1,   0,    0 },    // n = 1
            { 1,   2,   1,    0 },    // n = 2
            { 1,   3,   3,    1 },    // n = 3
            { 1,   4,   6,    4 },    // n = 4
            { 1,   5,  10,   10 },    // n = 5
            { 1,   6,  15,   20 },    // n = 6
            { 1,   7,  21,   35 },    // n = 7
            { 1,   8,  28,   56 },    // n = 8
            { 1,   9,  36,   84 },    // n = 9
            { 1,  10,  45,  120 },    // n = 10
            { 1,  11,  55,  165 }     // n = 11
        } };

    std::string debugColorName(const sf::Color& color) {
        if (color == sf::Color::Red) return "Red";
        if (color == sf::Color::Blue) return "Blue";
        if (color == sf::Color{ 255, 165, 0 }) return "Orange";
        if (color == sf::Color::Green) return "Green";
        if (color == sf::Color::White) return "White";
        if (color == sf::Color::Yellow) return "Yellow";
        return "Unknown";
    }

    constexpr std::array<std::pair<int, int>, 4> cornerPositions{ {
        {0, 0},
        {0, 2},
        {2, 2},
        {2, 0}
    } };

    constexpr std::array<std::pair<int, int>, 4> edgePositions{ {
        {0, 1},
        {1, 2},
        {2, 1},
        {1, 0}
    } };


    int getCornerOrientation(const FaceColors& colors) {
        int ans{ 0 };

        for (int i = 0; i < 7; i++) { // DRB not considered
            for (int j = 0; j < 3; j++) {
                const int face{ faceIndex.at(corners[i][j * 2]) };
                const auto [a, b] = cornerPositions[corners[i][j * 2 + 1] - '0'];
                const sf::Color color{ colors[face][a][b] };

                if (color == defaultFaceColors[4] || color == defaultFaceColors[5]) { // up or down color
                    ans = ans * 3 + j; // i will be the orientation
                    break;
                }
            }
        }

        return ans;
    }

    int getEdgeOrientation(const FaceColors& colors) {
        int ans{ 0 };

        for (int i = 0; i < 11; i++) { // BR not considered
            const auto [a1, b1] { edgePositions[edges[i][1] - '0'] };
            const sf::Color color1{ colors[faceIndex.at(edges[i][0])][a1][b1] };

            const auto [a2, b2] { edgePositions[edges[i][3] - '0'] };
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

    int getUDSliceCoordinate(const FaceColors& colors) {
        int a = -1;
        int ans{ 0 };

        for (int i = 0; i < 12; i++) {
            const auto [a1, b1] { edgePositions[edges[i][1] - '0'] };
            const sf::Color color1{ colors[faceIndex.at(edges[i][0])][a1][b1] };

            const auto [a2, b2] { edgePositions[edges[i][3] - '0'] };
            const sf::Color color2{ colors[faceIndex.at(edges[i][2])][a2][b2] };

            bool isUDSlice{ color1 != defaultFaceColors[4] && color1 != defaultFaceColors[5] && color2 != defaultFaceColors[4] && color2 != defaultFaceColors[5] };

            if (isUDSlice) a++;
            else if (a != -1) ans += nCr[i][a];
        }

        return ans;
    }

    void moveCube(Cube& cube, char move) {
        if (move == 'U') cube.U();
        else if (move == 'D') cube.D();
        else if (move == 'L') cube.L();
        else if (move == 'R') cube.R();
        else if (move == 'F') cube.F();
        else if (move == 'B') cube.B();
    }

    void generateCornerOrientationMoveTable() {
        std::array<std::array<int, 18>, 2187> table;
        std::array<bool, 2187> visited{};
        int visitedCount{ 0 };


        Cube cube{ 0.f };

        const auto dfs{
            [&](auto&& self) -> void {
                const int coord {getCornerOrientation(cube.faceColors)};

                if (coord < 0 || coord >= 2187) {
                    std::cout << "Invalid coord: " << coord << '\n';
                    return;
                }

                std::cout << visitedCount << ' ' << coord << '\n';

                if (visited[coord]) return;
                visited[coord] = true;
                visitedCount++;

                for (char move : {'F', 'R', 'B', 'L', 'U', 'D'}) {
                    for (int i = 0; i < 4; i++) {
                        moveCube(cube, move);
                        table[coord][i] = move;
                        self(self);
                    }
                }
            }
        };

        dfs(dfs);


        // Save binary
        std::ofstream out("cornerOrientation1.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));
    }

    void generateEdgeOrientationMoveTable() {
        std::array<std::array<int, 18>, 2048> table;
        std::array<bool, 2048> visited{};
        int visitedCount{ 0 };


        Cube cube{ 0.f };

        const auto dfs{
            [&](auto&& self) -> void {
                const int coord {getEdgeOrientation(cube.faceColors)};

                if (coord < 0 || coord >= 2048) {
                    std::cout << "Invalid coord: " << coord << '\n';
                    return;
                }

                std::cout << visitedCount << ' ' << coord << '\n';

                if (visited[coord]) return;
                visited[coord] = true;
                visitedCount++;

                for (char move : {'F', 'R', 'B', 'L', 'U', 'D'}) {
                    for (int i = 0; i < 4; i++) {
                        moveCube(cube, move);
                        table[coord][i] = move;
                        self(self);
                    }
                }
            }
        };

        dfs(dfs);


        // Save binary
        std::ofstream out("edgeOrientation1.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));
    }

    void generateUDSliceCoordinateTable() {
        std::array<std::array<int, 18>, 495> table;
        std::array<bool, 495> visited{};
        int visitedCount{ 0 };


        Cube cube{ 0.f };

        const auto dfs{
            [&](auto&& self) -> void {
                const int coord {getUDSliceCoordinate(cube.faceColors)};

                if (coord < 0 || coord >= 495) {
                    std::cout << "Invalid coord: " << coord << '\n';
                    return;
                }

                std::cout << visitedCount << ' ' << coord << '\n';

                if (visited[coord]) return;
                visited[coord] = true;
                visitedCount++;

                for (char move : {'F', 'R', 'B', 'L', 'U', 'D'}) {
                    for (int i = 0; i < 4; i++) {
                        moveCube(cube, move);
                        table[coord][i] = move;
                        self(self);
                    }
                }
            }
        };

        dfs(dfs);


        // Save binary
        std::ofstream out("UDSliceCoordinate1.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));
    }

    void generateAllMoveTables() {
        generateCornerOrientationMoveTable();
        generateEdgeOrientationMoveTable();
        generateUDSliceCoordinateTable();
    }

    std::vector<char> solve(const Cube& cube) {
        return {};
    }
}
