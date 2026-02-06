#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "point.cpp"
using namespace std;

class QuadTree {
    private:
    int x, y, width, height;
    const int capacity = 10;
    int level;
    bool divided;
    vector<int> points_references;

    QuadTree* children[2][2];
    QuadTree* parent;

    public:

    QuadTree(int _x, int _y, int _width, int _height, int _level, QuadTree* _parent = nullptr)
        : x(_x), y(_y), width(_width), height(_height), level(_level), divided(false), parent(_parent) {
        children[0][0] = children[0][1] = children[1][0] = children[1][1] = nullptr;
        if (parent){
            this->level = parent->level + 1;
        }else{
            this->level = 0;
        }
    }

    void addPoint(int point_reference) {
        this->points_references.push_back(point_reference);
    }

    void render(sf::RenderWindow& window, vector<Point>& points) {
        sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
        rectangle.setPosition(static_cast<float>(x), static_cast<float>(y));
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setOutlineColor(sf::Color::White);
        rectangle.setOutlineThickness(1.0f);
        window.draw(rectangle);

        for (int ref : points_references) {
            points[ref].render(window);
        }

        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->render(window, points);
                    }
                }
            }
        }
    }
};