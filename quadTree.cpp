#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "point.cpp"
using namespace std;

class QuadTree {
    private:
    int x, y, width, height;
    int points_count = 0;
    const int capacity = 4;
    const int LEVEL_LIMIT = 4;
    int level;
    bool divided;
    vector<Point>* points = nullptr;
    vector<int> points_references;

    QuadTree* children[2][2];
    QuadTree* parent;

    public:

    QuadTree(int _x, int _y, int _width, int _height, int _level, vector<Point>* _points = nullptr,QuadTree* _parent = nullptr)
        : x(_x), y(_y), width(_width), height(_height), level(_level), divided(false), parent(_parent), points(_points) {
        children[0][0] = children[0][1] = children[1][0] = children[1][1] = nullptr;
        if (parent){
            this->level = parent->level + 1;
        }else{
            this->level = 0;
        }
        points_references = vector<int>();
    }

    void addPoint(int point_reference) {
        if(!divided){
            this->points_references.push_back(point_reference);
            points_count++;
            if(points_count > capacity and level < LEVEL_LIMIT){
                this->subdivide();
                this->realocatePoints();
            }
        }else{
            this->alocatePoint(point_reference);
        }
    }

    void subdivide() {
        if (divided) return;

        int halfWidth = width / 2;
        int halfHeight = height / 2;

        children[0][0] = new QuadTree(x, y, halfWidth, halfHeight, level + 1, points, this);
        children[0][1] = new QuadTree(x + halfWidth, y, halfWidth, halfHeight, level + 1, points, this);
        children[1][0] = new QuadTree(x, y + halfHeight, halfWidth, halfHeight, level + 1, points, this);
        children[1][1] = new QuadTree(x + halfWidth, y + halfHeight, halfWidth, halfHeight, level + 1, points, this);

        divided = true;
    }

    void alocatePoint(int point_reference) {
        Point& p = this->points->at(point_reference);
        double px = p.getX();
        double py = p.getY();

        int indexX = (px >= x + width / 2) ? 1 : 0;
        int indexY = (py >= y + height / 2) ? 1 : 0;

        if (children[indexY][indexX]) {
            children[indexY][indexX]->addPoint(point_reference);
            cout << "Level<<" << level<< " Children:" << indexX << " " << indexY << endl;
        }
    }

    void realocatePoints() {
        vector<int> tempPoints = points_references;
        points_references.clear();
        points_count = 0;

        for (int ref : tempPoints) {
            this->alocatePoint(ref);
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
            for (int ref : points_references) {
                this->points->at(ref).render(window);
            }
        }
    }
};