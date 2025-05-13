#include <SFML/Graphics.hpp>

#include "Camera.hpp"
#include "Cube.hpp"

constexpr int windowWidth{ 800 };
constexpr int windowHeight{ 600 };

constexpr float rotationSpeed{ .07f };

constexpr float cubeletSize{ 2.f };

int main()
{
    Cube cube{ cubeletSize };
    Camera cam{ windowWidth, windowHeight };


    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    auto window{ sf::RenderWindow(sf::VideoMode({ windowWidth, windowHeight}), "Rubik's Cube", sf::Style::Default, sf::State::Windowed, settings) };

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

            }
        }


        window.clear();

        cube.manageRotation(1.f);
        cam.draw(window, cube);

        window.display();
    }
}
