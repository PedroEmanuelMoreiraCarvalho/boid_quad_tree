#include <iostream>
#include <SFML/Graphics.hpp>
#include "quadTree.cpp"
#include <bits/stdc++.h>

int main(int argc, char* args[]){

    cout << "number of arguments: " << argc << endl;
    int BOIDS_COUNT = argc > 1 ? stoi(args[1]) : 50;
    int LEVEL_LIMIT = argc > 2 ? stoi(args[2]) : 4;
    int CAPACITY = argc > 3 ? stoi(args[3]) : 15;

    int cameraX = 0, cameraY = 0;
    double zoom = 1.0;

    sf::RenderWindow window(sf::VideoMode(Boid::SCREEN_WIDTH, Boid::SCREEN_HEIGHT), "Boids");
    window.setFramerateLimit(60);
    sf::Clock clock;
    vector<Boid> boids;
    QuadTree quadTree(0, 0, Boid::SCREEN_WIDTH, Boid::SCREEN_HEIGHT, 0, LEVEL_LIMIT, CAPACITY, &boids); //root
    bool shift = false;
    bool pause = false;

    for(int i = 0; i < BOIDS_COUNT; ++i){
        double x = rand() % Boid::SCREEN_WIDTH;
        double y = rand() % Boid::SCREEN_HEIGHT;
        boids.emplace_back(x, y);
        quadTree.addBoid(boids.size() - 1); //add last boid reference to the quadtree
        quadTree.analyseUndivide();
        quadTree.analyseDivide();
        quadTree.checkBoidsPosition();
    }

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

                    if(shift){
                        quadTree.selectQuadrant(mouseX, mouseY);
                    }else{
                        boids.emplace_back(static_cast<double>(mouseX), static_cast<double>(mouseY));
                        quadTree.addBoid(boids.size() - 1); //add last boid reference to the quadtree
                    }
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled){
                if (event.mouseWheelScroll.delta > 0){
                    zoom += 0.1;
                }else{
                    zoom -= 0.1;
                    if (zoom < 0.1) zoom = 0.1;
                }
            }

            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W){
                cameraY -= 20;
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S){
                cameraY += 20;
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A){
                cameraX -= 20;
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D){
                cameraX += 20;
            }

            if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::LShift){
                shift = true;
            }
            if( event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::LShift){
                shift = false;
            }
            if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
                pause = !pause;
            }
        }

        window.clear(sf::Color::Black);

        quadTree.analyseUndivide();
        quadTree.analyseDivide();
        quadTree.checkBoidsPosition();
        if (!pause) {
            quadTree.boid();
            quadTree.update(deltaTime.asSeconds()); //update with deltaTime
        }
        quadTree.render(window, cameraX, cameraY, zoom);

        window.display();
    }

    return 0;
}