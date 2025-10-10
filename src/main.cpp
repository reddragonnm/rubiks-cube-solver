#include <iostream>

#include <SFML/Graphics.hpp>

#include "Camera.hpp"
#include "Cube.hpp"
#include "Solver.hpp"

constexpr int windowWidth{ 800 };
constexpr int windowHeight{ 600 };

constexpr float rotationSpeed{ .1f };
constexpr float faceRotationSpeed{ 500.f };
constexpr float cubeletSize{ 2.f };

int main()
{
    Cube cube{ cubeletSize };
    Camera cam{ windowWidth, windowHeight };

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    auto window{ sf::RenderWindow(sf::VideoMode({ windowWidth, windowHeight}), "Rubik's Cube", sf::Style::Default, sf::State::Windowed, settings) };

    static sf::Clock clock;
    static float frameTime{ 0.f };

    while (window.isOpen())
    {
        while (const std::optional event{ window.pollEvent() })
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                    cube.rotate(-rotationSpeed, { 1.f, 0.f, 0.f });
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                    cube.rotate(rotationSpeed, { 1.f, 0.f, 0.f });
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                    cube.rotate(-rotationSpeed, { 0.f, 1.f, 0.f });
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                    cube.rotate(rotationSpeed, { 0.f, 1.f, 0.f });

                else if (keyPressed->scancode == sf::Keyboard::Scancode::U)
                    cube.startRotation('U');
                else if (keyPressed->scancode == sf::Keyboard::Scancode::D)
                    cube.startRotation('D');
                else if (keyPressed->scancode == sf::Keyboard::Scancode::L)
                    cube.startRotation('L');
                else if (keyPressed->scancode == sf::Keyboard::Scancode::R)
                    cube.startRotation('R');
                else if (keyPressed->scancode == sf::Keyboard::Scancode::F)
                    cube.startRotation('F');
                else if (keyPressed->scancode == sf::Keyboard::Scancode::B)
                    cube.startRotation('B');

                else if (keyPressed->scancode == sf::Keyboard::Scancode::Space)
                    cube.shuffle(20);

                else if (keyPressed->scancode == sf::Keyboard::Scancode::Enter && cube.getState() == Cube::IDLE) {
                    // auto solution{ Solver::solve(cube) };
                    // for (const auto& move : solution) {
                    //     cube.startRotation(move);
                    // }

                    Solver::idaPhase1Search(cube);
                }

                else if (keyPressed->scancode == sf::Keyboard::Scancode::O && cube.getState() == Cube::IDLE) {
                    // auto solution{ Solver::solve(cube) };
                    // for (const auto& move : solution) {
                    //     cube.startRotation(move);
                    // }

                    Solver::idaPhase2Search(cube);
                }
            }
        }

        // if (cube.getState() == Cube::IDLE) {
        //     std::cout << Solver::getCornerPermutation(cube.faceColors) << ' ' << Solver::getEdgePermutation(cube.faceColors) << ' ' << Solver::getUDSlicePermutation(cube.faceColors) << '\n';
        // }

        window.clear();

        frameTime = clock.restart().asSeconds();

        cube.manageRotation(faceRotationSpeed * frameTime);
        cam.draw(window, cube);

        window.display();
    }
}

int main2() {
    // Solver::generateCornerPermutationMoveTable();
    // Solver::generateEdgePermutationMoveTable();
    // Solver::generateUDPermutationMoveTable();
    // Solver::generatePhase2PruneTable();
    // Solver::generatePhase2PruningTable2();
}
