#include <iostream>
#include <SFML/Graphics.hpp>
#include "quadTree.cpp"
#include <bits/stdc++.h>

int main(){
    const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Boids");
    window.setFramerateLimit(60);
    sf::Clock clock;
    vector<Boid> boids;
    QuadTree quadTree(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, &boids); //root

    while (window.isOpen()){
        sf::Event event;
        sf::Time deltaTime = clock.restart();
        
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed){
                if (event.mouseButton.button == sf::Mouse::Left){
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    boids.emplace_back(static_cast<double>(mouseX), static_cast<double>(mouseY));
                    quadTree.addPoint(boids.size() - 1); //add last point reference to the quadtree
                }
                else if (event.mouseButton.button == sf::Mouse::Right){
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    for (Boid& boid : boids){
                        boid.setDestin(static_cast<double>(mouseX), static_cast<double>(mouseY));
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        quadTree.update(deltaTime.asSeconds()); //update with deltaTime
        quadTree.render(window);

        window.display();
    }

    return 0;
}