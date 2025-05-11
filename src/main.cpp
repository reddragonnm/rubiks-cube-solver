#include <SFML/Graphics.hpp>

#include "Constants.hpp"
#include "Camera.hpp"
#include "Cube.hpp"

int main()
{
    Camera cam{ Constants::windowWidth, Constants::windowHeight };
    // Cube cube{};
    RubiksCube cube{ Constants::cubeSize };

    auto window{ sf::RenderWindow(sf::VideoMode({ Constants::windowWidth, Constants::windowHeight}), "Rubik's Cube") };

    while (window.isOpen())
    {
        while (const std::optional event{ window.pollEvent() })
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                    cube.rotate(-Constants::rotationSpeed, { 1.f, 0.f, 0.f });
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
                    cube.rotate(Constants::rotationSpeed, { 1.f, 0.f, 0.f });
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                    cube.rotate(-Constants::rotationSpeed, { 0.f, 1.f, 0.f });
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                    cube.rotate(Constants::rotationSpeed, { 0.f, 1.f, 0.f });

            }
        }


        window.clear();

        // cam.drawCube(window, cube);
        cam.drawRubiksCube(window, cube);

        window.display();
    }
}
