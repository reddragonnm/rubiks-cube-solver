#pragma once

#include <array>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <stack>

#include <algorithm>
#include <filesystem>
#include <cstdint>

#include <SFML/Graphics.hpp>

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

    constexpr std::array<int, 10> factorial{ 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 };

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

    auto generateCornerOrientationMoveTable() {
        std::array<std::array<int, 18>, 2187> table;
        for (auto& row : table) row.fill(-1);

        std::array<bool, 2187> visited{};
        int visitedCount{ 0 };


        Cube cube{ 0.f };

        const auto dfs{
            [&](auto&& self, int coord) -> void {
                if (coord < 0 || coord >= 2187) {
                    std::cout << "Invalid coord: " << coord << '\n';
                    return;
                }

                std::cout << visitedCount << ' ' << coord << '\n';

                if (visited[coord]) return;
                visited[coord] = true;
                visitedCount++;

                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < 3; j++) {
                        moveCube(cube, "FRBLUD"[i]);

                        const int newCoord {getCornerOrientation(cube.faceColors)};
                        table[coord][i * 3 + j] = newCoord;

                        self(self, newCoord);
                    }
                    moveCube(cube, "FRBLUD"[i]); // to reset
                }
            }
        };

        dfs(dfs, 0);

        // Save binary
        std::ofstream out("cornerOrientation1.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));

        return table;
    }

    auto generateEdgeOrientationMoveTable() {
        std::array<std::array<int, 18>, 2048> table;
        std::array<bool, 2048> visited{};
        int visitedCount{ 0 };


        Cube cube{ 0.f };

        const auto dfs{
            [&](auto&& self, int coord) -> void {
                if (coord < 0 || coord >= 2048) {
                    std::cout << "Invalid coord: " << coord << '\n';
                    return;
                }

                std::cout << visitedCount << ' ' << coord << '\n';

                if (visited[coord]) return;
                visited[coord] = true;
                visitedCount++;

                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < 3; j++) {
                        moveCube(cube, "FRBLUD"[i]);

                        const int newCoord{ getEdgeOrientation(cube.faceColors) };
                        table[coord][i * 3 + j] = newCoord;

                        self(self, newCoord);
                    }
                    moveCube(cube, "FRBLUD"[i]); // to reset
                }
            }
        };

        dfs(dfs, 0);

        // Save binary
        std::ofstream out("edgeOrientation1.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));

        return table;
    }

    auto generateUDSliceCoordinateTable() {
        std::array<std::array<int, 18>, 495> table;
        std::array<bool, 495> visited{};
        int visitedCount{ 0 };


        Cube cube{ 0.f };

        const auto dfs{
            [&](auto&& self, int coord) -> void {
                if (coord < 0 || coord >= 495) {
                    std::cout << "Invalid coord: " << coord << '\n';
                    return;
                }

                std::cout << visitedCount << ' ' << coord << '\n';

                if (visited[coord]) return;
                visited[coord] = true;
                visitedCount++;

                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < 3; j++) {
                        moveCube(cube, "FRBLUD"[i]);

                        const int newCoord{ getUDSliceCoordinate(cube.faceColors) };
                        table[coord][i * 3 + j] = newCoord;

                        self(self, newCoord);
                    }
                    moveCube(cube, "FRBLUD"[i]); // to reset
                }
            }
        };

        dfs(dfs, 0);

        // Save binary
        std::ofstream out("UDSliceCoordinate1.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));

        return table;
    }

    void generateAllMoveTables() {
        generateCornerOrientationMoveTable();
        generateEdgeOrientationMoveTable();
        generateUDSliceCoordinateTable();
    }

    auto generatePhase1PruneTable() {
        std::vector<int> table(2187 * 2048, -1); std::vector<int> q;

        std::array<std::array<int, 18>, 2187> cornerTable;
        std::ifstream in1("cornerOrientation1.bin", std::ios::binary);
        in1.read(reinterpret_cast<char*>(cornerTable.data()), cornerTable.size() * sizeof(cornerTable[0]));
        in1.close();

        std::array<std::array<int, 18>, 2048> edgeTable;
        std::ifstream in2("edgeOrientation1.bin", std::ios::binary);
        in2.read(reinterpret_cast<char*>(edgeTable.data()), edgeTable.size() * sizeof(edgeTable[0]));
        in2.close();

        auto idx = [](int corner, int edge) -> int { return corner * 2048 + edge; };

        table[idx(0, 0)] = 0;
        q.push_back(idx(0, 0));

        int head{ 0 };

        while (head < q.size()) {
            int cur{ q[head++] };
            int curCorner{ cur / 2048 };
            int curEdge{ cur % 2048 };
            int depth{ table[cur] };

            for (int i = 0; i < 18; i++) {
                const int nxtCorner{ cornerTable[curCorner][i] };
                const int nxtEdge{ edgeTable[curEdge][i] };
                const int nxt{ idx(nxtCorner, nxtEdge) };

                if (table[nxt] == -1) {
                    table[nxt] = depth + 1;
                    q.push_back(nxt);
                }
            }

            if (head % 10000 == 0) {
                std::cout << head << ' ' << q.size() << '\n';
            }
        }

        const int maxDepth{ *std::max_element(table.begin(), table.end()) };
        std::cout << "Max depth: " << maxDepth << '\n';
        std::ofstream out("pruningTable1.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));

        return table;
    }

    int dfsPhase1(int corner, int edge, int udSlice, int g, int threshold, int lastMove, std::vector<int>& path, std::array<std::array<int, 18>, 2187>& cornerTable, std::array<std::array<int, 18>, 2048>& edgeTable, std::array<std::array<int, 18>, 495>& udSliceTable, std::vector<int>& pruneTable) {
        static const std::map<int, int> oppFaces{
            {0, 2},
            {1, 3},
            {2, 0},
            {3, 1},
            {4, 5},
            {5, 4}
        };


        int h{ pruneTable[corner * 2048 + edge] };
        int f{ g + h };

        if (f > threshold) return f;
        if (corner == 0 && edge == 0 && udSlice == 0) {
            // solution found
            for (int move : path) {
                std::cout << "FRBLUD"[move / 3];
                if (move % 3 == 1) std::cout << "2";
                else if (move % 3 == 2) std::cout << "'";
                std::cout << ' ';
            }
            std::cout << '\n';
            return -1;
        }

        int minOver{ 1 << 30 };

        for (int move = 0; move < 18; move++) {
            if (move / 3 == lastMove / 3) continue; // same face
            if (oppFaces.at(move / 3) == lastMove / 3) continue; // opposite face

            int newCorner{ cornerTable[corner][move] };
            int newEdge{ edgeTable[edge][move] };
            int newUDSlice{ udSliceTable[udSlice][move] };

            int childH{ pruneTable[newCorner * 2048 + newEdge] };
            if (g + 1 + childH > threshold) {
                minOver = std::min(minOver, g + 1 + childH);
                continue;
            }

            path.push_back(move);
            int t{ dfsPhase1(newCorner, newEdge, newUDSlice, g + 1, threshold, move, path, cornerTable, edgeTable, udSliceTable, pruneTable) };
            if (t == -1) return -1;
            if (t < minOver) minOver = t;
            path.pop_back();
        }

        return minOver;
    }

    void idaPhase1Search(const Cube& cube) {
        std::array<std::array<int, 18>, 2187> cornerTable;
        std::ifstream in1("cornerOrientation1.bin", std::ios::binary);
        in1.read(reinterpret_cast<char*>(cornerTable.data()), cornerTable.size() * sizeof(cornerTable[0]));
        in1.close();

        std::array<std::array<int, 18>, 2048> edgeTable;
        std::ifstream in2("edgeOrientation1.bin", std::ios::binary);
        in2.read(reinterpret_cast<char*>(edgeTable.data()), edgeTable.size() * sizeof(edgeTable[0]));
        in2.close();

        std::array<std::array<int, 18>, 495> udSliceTable;
        std::ifstream in3("UDSliceCoordinate1.bin", std::ios::binary);
        in3.read(reinterpret_cast<char*>(udSliceTable.data()), udSliceTable.size() * sizeof(udSliceTable[0]));
        in3.close();

        std::vector<int> pruneTable(2187 * 2048);
        std::ifstream in4("pruningTable1.bin", std::ios::binary);
        in4.read(reinterpret_cast<char*>(pruneTable.data()), pruneTable.size() * sizeof(pruneTable[0]));
        in4.close();

        int startCorner{ getCornerOrientation(cube.faceColors) };
        int startEdge{ getEdgeOrientation(cube.faceColors) };
        int startUDSlice{ getUDSliceCoordinate(cube.faceColors) };

        int threshold{ std::max({ pruneTable[startCorner * 2048 + startEdge], 1 }) };
        std::vector<int> path;

        while (true) {
            int nextThreshold{ dfsPhase1(startCorner, startEdge, startUDSlice, 0, threshold, -1, path, cornerTable, edgeTable, udSliceTable, pruneTable) };

            std::cout << "Next threshold: " << nextThreshold << '\n';

            if (nextThreshold == -1) {
                std::cout << "Solution found with " << path.size() << " moves\n";
                break;
            }
            if (nextThreshold == (1 << 30)) {
                std::cout << "No solution found\n";
                break;
            }

            threshold = nextThreshold;
        }
    }

    bool isSameCorner(std::array<sf::Color, 3> a, std::array<sf::Color, 3> b) {
        static auto sorter{
            [](const sf::Color& c1, const sf::Color& c2) {
                if (c1.r != c2.r) return c1.r < c2.r;
                if (c1.g != c2.g) return c1.g < c2.g;
                return c1.b < c2.b;
            }
        };

        std::sort(a.begin(), a.end(), sorter);
        std::sort(b.begin(), b.end(), sorter);

        return a == b;
    }

    int getCornerPermutation(const FaceColors& colors) {
        int ans{ 0 };

        std::array<std::array<sf::Color, 3>, 8> defaultCornerColors{};

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 3; j++) {
                const int face{ faceIndex.at(corners[i][j * 2]) };
                defaultCornerColors[i][j] = defaultFaceColors[face];
            }
        }

        std::array<int, 8> cornerOrder{};

        for (int i = 0; i < 8; i++) {
            std::array<sf::Color, 3> currentCornerColors{};

            for (int j = 0; j < 3; j++) {
                const int face{ faceIndex.at(corners[i][j * 2]) };
                const auto [a, b] = cornerPositions[corners[i][j * 2 + 1] - '0'];

                currentCornerColors[j] = colors[face][a][b];
            }

            for (int j = 0; j < 8; j++) {
                if (isSameCorner(currentCornerColors, defaultCornerColors[j])) {
                    cornerOrder[i] = j;
                    break;
                }
            }
        }

        for (int i = 1; i < 8; i++) {
            int num{ 0 };
            for (int j = 0; j < i; j++) {
                if (cornerOrder[j] > cornerOrder[i]) num++;
            }

            ans += num * factorial[i];
        }

        return ans;
    }

    bool isSameEdge(std::array<sf::Color, 2> a, std::array<sf::Color, 2> b) {
        return (a[0] == b[0] && a[1] == b[1]) || (a[0] == b[1] && a[1] == b[0]);
    }

    int getEdgePermutation(const FaceColors& colors) { // only valid for phase 2
        int ans{ 0 };

        int cornerOrientation{ getCornerOrientation(colors) };
        int edgeOrientation{ getEdgeOrientation(colors) };
        int udSlice{ getUDSliceCoordinate(colors) };

        if (cornerOrientation != 0 || edgeOrientation != 0 || udSlice != 0) {
            // std::cout << "Invalid state for edge permutation\n";
            return -1;
        }

        std::array<std::array<sf::Color, 2>, 8> defaultEdgeColors{};

        for (int i = 0; i < 8; i++) { // only UD edges
            for (int j = 0; j < 2; j++) {
                const int face{ faceIndex.at(edges[i][j * 2]) };
                defaultEdgeColors[i][j] = defaultFaceColors[face];
            }
        }

        std::array<int, 8> edgeOrder{};

        for (int i = 0; i < 8; i++) {
            std::array<sf::Color, 2> currentEdgeColors{};

            for (int j = 0; j < 2; j++) {
                const int face{ faceIndex.at(edges[i][j * 2]) };
                const auto [a, b] = edgePositions[edges[i][j * 2 + 1] - '0'];

                currentEdgeColors[j] = colors[face][a][b];
            }

            for (int j = 0; j < 8; j++) {
                if (isSameEdge(currentEdgeColors, defaultEdgeColors[j])) {
                    edgeOrder[i] = j;
                    break;
                }
            }
        }

        for (int i = 1; i < 8; i++) {
            int num{ 0 };
            for (int j = 0; j < i; j++) {
                if (edgeOrder[j] > edgeOrder[i]) num++;
            }

            ans += num * factorial[i];
        }

        return ans;
    }

    int getUDSlicePermutation(const FaceColors& colors) {
        int ans{ 0 };

        int cornerOrientation{ getCornerOrientation(colors) };
        int edgeOrientation{ getEdgeOrientation(colors) };
        int udSlice{ getUDSliceCoordinate(colors) };

        if (cornerOrientation != 0 || edgeOrientation != 0 || udSlice != 0) {
            // std::cout << "Invalid state for edge permutation\n";
            return -1;
        }

        std::array<std::array<sf::Color, 2>, 4> defaultEdgeColors{};

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2; j++) {
                const int face{ faceIndex.at(edges[i + 8][j * 2]) };
                defaultEdgeColors[i][j] = defaultFaceColors[face];
            }
        }

        std::array<int, 4> edgeOrder{};

        for (int i = 0; i < 4; i++) {
            std::array<sf::Color, 2> currentEdgeColors{};

            for (int j = 0; j < 2; j++) {
                const int face{ faceIndex.at(edges[i + 8][j * 2]) };
                const auto [a, b] = edgePositions[edges[i + 8][j * 2 + 1] - '0'];

                currentEdgeColors[j] = colors[face][a][b];
            }

            for (int j = 0; j < 4; j++) {
                if (isSameEdge(currentEdgeColors, defaultEdgeColors[j])) {
                    edgeOrder[i] = j;
                    break;
                }
            }
        }

        for (int i = 1; i < 4; i++) {
            int num{ 0 };
            for (int j = 0; j < i; j++) {
                if (edgeOrder[j] > edgeOrder[i]) num++;
            }

            ans += num * factorial[i];
        }

        return ans;
    }

    auto generateCornerPermutationMoveTable() {
        std::array<std::vector<int>, factorial[8]> table{};
        std::array<bool, factorial[8]> visited{};
        int visitedCount{ 0 };

        std::stack<std::pair<int, FaceColors>> stk;
        Cube cube{ 0.f };
        stk.push({ 0, cube.faceColors });

        while (!stk.empty()) {
            auto [coord, faceColors] = stk.top();
            stk.pop();

            if (coord < 0 || coord >= factorial[8]) {
                std::cout << "Invalid coord: " << coord << '\n';
                continue;
            }

            if (visited[coord]) continue;

            visited[coord] = true;
            visitedCount++;
            std::cout << visitedCount << ' ' << coord << std::endl;

            for (int i = 0; i < 4; i++) {
                cube.faceColors = faceColors;
                moveCube(cube, "FRLB"[i]);
                moveCube(cube, "FRLB"[i]);
                const int newCoord{ getCornerPermutation(cube.faceColors) };
                table[coord].push_back(newCoord);
                if (!visited[newCoord]) {
                    stk.push({ newCoord, cube.faceColors });
                }
            }

            for (int i = 0; i < 2; i++) {
                cube.faceColors = faceColors;
                for (int j = 0; j < 3; j++) {
                    moveCube(cube, "UD"[i]);
                    const int newCoord{ getCornerPermutation(cube.faceColors) };
                    table[coord].push_back(newCoord);
                    if (!visited[newCoord]) {
                        stk.push({ newCoord, cube.faceColors });
                    }
                }
            }
        }

        // Save binary
        std::ofstream out("cornerPermutation2.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()),
            table.size() * sizeof(table[0]));
        out.close();

        return table;
    }

    auto generateEdgePermutationMoveTable() {
        std::array<std::vector<int>, factorial[8]> table{};
        std::array<bool, factorial[8]> visited{};
        int visitedCount{ 0 };

        std::stack<std::pair<int, FaceColors>> stk;
        Cube cube{ 0.f };
        stk.push({ 0, cube.faceColors });

        while (!stk.empty()) {
            auto [coord, faceColors] = stk.top();
            stk.pop();

            if (coord < 0 || coord >= factorial[8]) {
                std::cout << "Invalid coord: " << coord << '\n';
                continue;
            }

            if (visited[coord]) continue;

            visited[coord] = true;
            visitedCount++;
            std::cout << visitedCount << ' ' << coord << std::endl;

            for (int i = 0; i < 4; i++) {
                cube.faceColors = faceColors;
                moveCube(cube, "FRLB"[i]);
                moveCube(cube, "FRLB"[i]);
                const int newCoord{ getEdgePermutation(cube.faceColors) };
                table[coord].push_back(newCoord);
                if (!visited[newCoord]) {
                    stk.push({ newCoord, cube.faceColors });
                }
            }

            for (int i = 0; i < 2; i++) {
                cube.faceColors = faceColors;
                for (int j = 0; j < 3; j++) {
                    moveCube(cube, "UD"[i]);
                    const int newCoord{ getEdgePermutation(cube.faceColors) };
                    table[coord].push_back(newCoord);
                    if (!visited[newCoord]) {
                        stk.push({ newCoord, cube.faceColors });
                    }
                }
            }
        }

        // Save binary
        std::ofstream out("edgePermutation2.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()),
            table.size() * sizeof(table[0]));
        out.close();

        return table;
    }

    auto generateUDPermutationMoveTable() {
        std::array<std::vector<int>, 24> table{};
        std::array<bool, 24> visited{};
        int visitedCount{ 0 };

        std::stack<std::pair<int, FaceColors>> stk;
        Cube cube{ 0.f };
        stk.push({ 0, cube.faceColors });

        while (!stk.empty()) {
            auto [coord, faceColors] = stk.top();
            stk.pop();

            if (coord < 0 || coord >= 24) {
                std::cout << "Invalid coord: " << coord << '\n';
                continue;
            }

            if (visited[coord]) continue;

            visited[coord] = true;
            visitedCount++;
            std::cout << visitedCount << ' ' << coord << std::endl;

            for (int i = 0; i < 4; i++) {
                cube.faceColors = faceColors;
                moveCube(cube, "FRLB"[i]);
                moveCube(cube, "FRLB"[i]);
                const int newCoord{ getUDSlicePermutation(cube.faceColors) };
                table[coord].push_back(newCoord);
                if (!visited[newCoord]) {
                    stk.push({ newCoord, cube.faceColors });
                }
            }

            for (int i = 0; i < 2; i++) {
                cube.faceColors = faceColors;
                for (int j = 0; j < 3; j++) {
                    moveCube(cube, "UD"[i]);
                    const int newCoord{ getUDSlicePermutation(cube.faceColors) };
                    table[coord].push_back(newCoord);
                    if (!visited[newCoord]) {
                        stk.push({ newCoord, cube.faceColors });
                    }
                }
            }
        }

        // Save binary
        std::ofstream out("UDPermutation2.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(table.data()),
            table.size() * sizeof(table[0]));
        out.close();

        return table;
    }

    std::vector<char> solve(const Cube& cube) {
        return {};
    }
}
