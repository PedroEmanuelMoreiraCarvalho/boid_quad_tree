#include <iostream>
#include <SFML/Graphics.hpp>
#include "quadTree.cpp"
#include <bits/stdc++.h>

int main(){
    const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Boids");
    window.setFramerateLimit(60);
    vector<Point> points;
    QuadTree quadTree(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, &points); //root

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed){
                if (event.mouseButton.button == sf::Mouse::Left){
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    points.emplace_back(static_cast<double>(mouseX), static_cast<double>(mouseY));
                    quadTree.addPoint(points.size() - 1); //add last point reference to the quadtree
                }
            }
        }

        window.clear(sf::Color::Black);

        quadTree.render(window);

        window.display();
    }

    return 0;
}