#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 700), "Mastermind");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        window.clear();
        window.display();
    }

    return EXIT_SUCCESS;
}