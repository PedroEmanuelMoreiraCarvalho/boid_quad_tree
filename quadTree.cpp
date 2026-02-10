#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "boid.cpp"
using namespace std;

class QuadTree {
    private:
    int x, y, width, height;
    int points_count = 0;
    const int capacity = 4;
    const int LEVEL_LIMIT = 4;
    int level;
    bool divided;
    vector<Boid>* boids = nullptr;
    vector<int> boids_references;

    QuadTree* children[2][2];
    QuadTree* parent;

    public:

    QuadTree(int _x, int _y, int _width, int _height, int _level, vector<Boid>* _boids = nullptr,QuadTree* _parent = nullptr)
        : x(_x), y(_y), width(_width), height(_height), level(_level), divided(false), parent(_parent), boids(_boids) {
        children[0][0] = children[0][1] = children[1][0] = children[1][1] = nullptr;
        if (parent){
            this->level = parent->level + 1;
        }else{
            this->level = 0;
        }
        boids_references = vector<int>();
    }

    void addPoint(int boid_reference) {
        if(!divided){
            this->boids_references.push_back(boid_reference);
            points_count++;
            if(points_count > capacity and level < LEVEL_LIMIT){
                this->subdivide();
                this->realocatePoints();
            }
        }else{
            this->alocatePoint(boid_reference);
        }
    }

    void subdivide() {
        if (divided) return;

        int halfWidth = width / 2;
        int halfHeight = height / 2;

        children[0][0] = new QuadTree(x, y, halfWidth, halfHeight, level + 1, boids, this);
        children[0][1] = new QuadTree(x + halfWidth, y, halfWidth, halfHeight, level + 1, boids, this);
        children[1][0] = new QuadTree(x, y + halfHeight, halfWidth, halfHeight, level + 1, boids, this);
        children[1][1] = new QuadTree(x + halfWidth, y + halfHeight, halfWidth, halfHeight, level + 1, boids, this);

        divided = true;
    }

    void alocatePoint(int boid_reference) {
        Boid& b = this->boids->at(boid_reference);
        double px = b.getX();
        double py = b.getY();

        int indexX = (px >= x + width / 2) ? 1 : 0;
        int indexY = (py >= y + height / 2) ? 1 : 0;

        if (children[indexY][indexX]) {
            children[indexY][indexX]->addPoint(boid_reference);
        }
    }

    void realocatePoints() {
        vector<int> tempPoints = boids_references;
        boids_references.clear();
        points_count = 0;

        for (int ref : tempPoints) {
            this->alocatePoint(ref);
        }
    }

    void update(float deltaTime) {
        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->update(deltaTime);
                    }
                }
            }
        } else {
            for (int ref : boids_references) {
                this->boids->at(ref).update(deltaTime);
            }
        }
    }

    void render(sf::RenderWindow& window) {
        sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
        rectangle.setPosition(static_cast<float>(x), static_cast<float>(y));
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setOutlineColor(sf::Color::White);
        rectangle.setOutlineThickness(1.0f);
        window.draw(rectangle);

        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->render(window);
                    }
                }
            }
        } else {
            for (int ref : boids_references) {
                this->boids->at(ref).render(window);
            }
        }
    }
};