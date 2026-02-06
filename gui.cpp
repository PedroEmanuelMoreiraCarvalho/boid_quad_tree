#include <iostream>
#include <SFML/Graphics.hpp>
#include "quadTree.cpp"
#include <bits/stdc++.h>

int main()
{
    
    sf::RenderWindow window(sf::VideoMode(800, 600), "Boids");
    window.setFramerateLimit(60);
    vector<Point> points;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    points.emplace_back(static_cast<double>(mouseX), static_cast<double>(mouseY));
                }
            }
        }

        window.clear(sf::Color::Black);

        for (auto& point : points) {
            point.render(window);
        }

        window.display();
    }

    return 0;
}