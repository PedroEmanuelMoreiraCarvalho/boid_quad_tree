#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "boid.cpp"
using namespace std;

class QuadTree {
    private:
    int x, y, width, height;
    int boids_count = 0;
    const int CAPACITY = 4;
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

    void popBoid(int mouseX, int mouseY) {
        if (divided) {
            int indexX = (mouseX >= x + width / 2) ? 1 : 0;
            int indexY = (mouseY >= y + height / 2) ? 1 : 0;

            if (children[indexY][indexX]) {
                children[indexY][indexX]->popBoid(mouseX, mouseY);
            }
        } else {
            boids_references.pop_back();
        }
    }

    void addPoint(int boid_reference) {
        if(!divided){
            this->boids_references.push_back(boid_reference);
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
        boids_count = 0;

        for (int ref : tempPoints) {
            this->alocatePoint(ref);
        }
    }

    void analyseDivide(void) {
        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->analyseDivide();
                    }
                }
            }
        } else {
            if(boids_references.size() > CAPACITY){
                this->subdivide();
                this->realocatePoints();
            }
        }
    }

    void analyseUndivide() {
        if (divided) {
            int totalBoids = 0;
            bool canUndivide = true;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->analyseUndivide();
                        if (!children[i][j]->divided){
                            totalBoids += children[i][j]->boids_references.size();
                        }else{
                            canUndivide = false;
                        }
                    }
                }
            }

            if(totalBoids <= CAPACITY and canUndivide){
                for (int i = 0; i < 2; ++i) {
                    for (int j = 0; j < 2; ++j) {
                        if (children[i][j]) {
                            for (int ref : children[i][j]->boids_references) {
                                this->boids_references.push_back(ref);
                            }
                            delete children[i][j];
                            children[i][j] = nullptr;
                        }
                    }
                }
                divided = false;
            }
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

        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            cout << "Failed to load font!" << endl;
            return;
        }
        sf::Text text;
        text.setFont(font);
        if(!divided){
            text.setString(to_string(boids_references.size()));
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            text.setPosition(static_cast<float>(x + width / 2), static_cast<float>(y + height / 2));
            window.draw(text);
        }

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