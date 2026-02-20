#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "boid.cpp"
using namespace std;

class QuadTree {
    private:
    int x, y, width, height;
    int boids_count = 0;
    int CAPACITY = 15;
    int LEVEL_LIMIT = 0;
    int level;
    bool divided;
    bool selected;
    vector<Boid>* boids = nullptr;
    vector<int> boids_references;

    QuadTree* children[2][2];
    QuadTree* parent;
    set<QuadTree*> visibles;

    public:

    QuadTree(int _x, int _y, int _width, int _height, int _level, int _LEVEL_LIMIT, int _CAPACITY, vector<Boid>* _boids = nullptr,QuadTree* _parent = nullptr)
        : x(_x), y(_y), width(_width), height(_height), level(_level), LEVEL_LIMIT(_LEVEL_LIMIT), CAPACITY(_CAPACITY), divided(false), parent(_parent), boids(_boids) {
        children[0][0] = children[0][1] = children[1][0] = children[1][1] = nullptr;
        if (parent){
            this->level = parent->level + 1;
        }else{
            this->level = 0;
        }
        boids_references = vector<int>();
        visibles = set<QuadTree*>({this});
        selected = false;
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

    void selectQuadrant(int mouseX, int mouseY) {
        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->selectQuadrant(mouseX, mouseY);
                    }
                }
            }
        }else{
            if(x <= mouseX && mouseX < x + width && y <= mouseY && mouseY < y + height){
                selected = !selected;
            }
        }
    }

    void addBoid(int boid_reference) {
        if(!divided){
            this->boids_references.push_back(boid_reference);
        }else{
            this->alocateBoid(boid_reference);
        }
    }

    void subdivide() {
        if (divided) return;

        int halfWidth = width / 2;
        int halfHeight = height / 2;

        children[0][0] = new QuadTree(x, y, halfWidth, halfHeight, level + 1, LEVEL_LIMIT, CAPACITY, boids, this);
        children[0][1] = new QuadTree(x + halfWidth, y, halfWidth, halfHeight, level + 1, LEVEL_LIMIT, CAPACITY, boids, this);
        children[1][0] = new QuadTree(x, y + halfHeight, halfWidth, halfHeight, level + 1, LEVEL_LIMIT, CAPACITY, boids, this);
        children[1][1] = new QuadTree(x + halfWidth, y + halfHeight, halfWidth, halfHeight, level + 1, LEVEL_LIMIT, CAPACITY, boids, this);

        for(int i = 0; i < 2; ++i){
            for(int j = 0; j < 2; ++j){
                if(children[i][j]){
                    children[i][j]->addVisibility(children[0][0]);
                    children[i][j]->addVisibility(children[0][1]);
                    children[i][j]->addVisibility(children[1][0]);
                    children[i][j]->addVisibility(children[1][1]);

                    for(QuadTree* visible : visibles){
                        if(visible == this) continue;
                        children[i][j]->checkVisibility(visible);
                        visible->visibles.erase(this);
                    }
                }
            }
        }

        visibles.clear();

        divided = true;
    }

    void checkVisibility(QuadTree* qt) {
        if (qt == this) return;

        int center[2] = {x + width / 2, y + height / 2};
        int qt_center[2] = {qt->x + qt->width / 2, qt->y + qt->height / 2};

        double max_distance_possible = sqrt(pow(width / 2 + qt->width / 2, 2) + pow(height / 2 + qt->height / 2, 2)) + Boid::vision_range;
        double actual_distance = sqrt(pow(center[0] - qt_center[0], 2) + pow(center[1] - qt_center[1], 2));

        if(actual_distance < max_distance_possible){
            this->addVisibility(qt);
        }

    }

    void addVisibility(QuadTree* qt) {
        visibles.insert(qt);
        qt->visibles.insert(this);
    }

    void alocateBoid(int boid_reference) {
        Boid& b = this->boids->at(boid_reference);
        double px = b.getX();
        double py = b.getY();

        int indexX = (px >= x + width / 2) ? 1 : 0;
        int indexY = (py >= y + height / 2) ? 1 : 0;

        if (children[indexY][indexX]) {
            children[indexY][indexX]->addBoid(boid_reference);
        }
    }

    void realocateBoids() {
        vector<int> tempBoids = boids_references;
        boids_references.clear();
        boids_count = 0;

        for (int ref : tempBoids) {
            this->alocateBoid(ref);
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
            if(boids_references.size() > CAPACITY and level < LEVEL_LIMIT){
                this->subdivide();
                this->realocateBoids();
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
                            for (QuadTree* visible : children[i][j]->visibles){
                                if(visible->parent == this) continue;
                                visible->visibles.erase(children[i][j]);
                                this->addVisibility(visible);
                            }
                        }
                    }
                }
                for (int i = 0; i < 2; ++i) {
                    for (int j = 0; j < 2; ++j) {
                        if (children[i][j]) {
                            delete children[i][j];
                            children[i][j] = nullptr;
                        }
                    }
                }
                divided = false;
            }
        }
    }

    void checkBoidsPosition() {
        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->checkBoidsPosition();
                    }
                }
            }
        } else {
            for(int i = 0; i < boids_references.size(); ++i){
                auto ref = boids_references[i];
                Boid& b = this->boids->at(ref);
                double px = b.getX();
                double py = b.getY();

                if ((px < x || px >= x + width || py < y || py >= y + height) and (px >= 0 && px < Boid::SCREEN_WIDTH && py >= 0 && py < Boid::SCREEN_HEIGHT)) {
                    boids_references.erase(boids_references.begin() + i);
                    if (parent) {
                        parent->reAddBoid(ref);
                    }
                }
            }
        }
    }

    void reAddBoid(int boid_reference){
        if(parent){
            parent->reAddBoid(boid_reference);
            return;
        }else{
            this->alocateBoid(boid_reference);
        }
    }

    void allign(vector<int>& visible_boids_references){
        if(divided){
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->allign(visible_boids_references);
                    }
                }
            }
        }else{
            for(int ref : boids_references){
                Boid* boid = &this->boids->at(ref);
                double boidX = boid->getX();
                double boidY = boid->getY();
            
                double alignmentX = 0.0;
                double alignmentY = 0.0;
                int count = 0;

                for(int visible_ref : visible_boids_references){
                    if(visible_ref == ref) continue;
                    Boid& visible_boid = this->boids->at(visible_ref);
                    double distance = sqrt(pow(visible_boid.getX() - boidX, 2) + pow(visible_boid.getY() - boidY, 2));
                    if(distance < Boid::vision_range){
                        alignmentX += visible_boid.getVelocityX();
                        alignmentY += visible_boid.getVelocityY();
                        count++;
                    }
                }

                if(count > 0){
                    alignmentX /= count;
                    alignmentY /= count;
                }

                boid->setAlignment(alignmentX, alignmentY);
            }
        }
    }

    void cohesion(vector<int>& visible_boids_references){
        if(divided){
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->cohesion(visible_boids_references);
                    }
                }
            }
        }else{
            for(int ref : boids_references){
                Boid* boid = &this->boids->at(ref);
                double boidX = boid->getX();
                double boidY = boid->getY();
            
                double cohesionX = 0.0;
                double cohesionY = 0.0;
                int count = 0;

                for(int visible_ref : visible_boids_references){
                    if(visible_ref == ref) continue;
                    Boid& visible_boid = this->boids->at(visible_ref);
                    double distance = sqrt(pow(visible_boid.getX() - boidX, 2) + pow(visible_boid.getY() - boidY, 2));
                    if(distance < Boid::vision_range){
                        cohesionX += visible_boid.getX();
                        cohesionY += visible_boid.getY();
                        count++;
                    }
                }

                if(count > 0){
                    cohesionX /= count;
                    cohesionY /= count;
                    cohesionX -= boidX;
                    cohesionY -= boidY;
                }

                boid->setCohesion(cohesionX, cohesionY);
            }
        }
    }

    void avoid(vector<int>& visible_boids_references){
        if(divided){
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->avoid(visible_boids_references);
                    }
                }
            }
        }else{

            for(int ref : boids_references){
                Boid* boid = &this->boids->at(ref);
                double boidX = boid->getX();
                double boidY = boid->getY();
            
                double separationX = 0.0;
                double separationY = 0.0;

                for(int visible_ref : visible_boids_references){
                    if(visible_ref == ref) continue;
                    Boid& visible_boid = this->boids->at(visible_ref);
                    double distance = sqrt(pow(visible_boid.getX() - boidX, 2) + pow(visible_boid.getY() - boidY, 2));
                    if(distance < Boid::avoid_range and distance > 0){
                        separationX += (boidX - visible_boid.getX()) / distance;
                        separationY += (boidY - visible_boid.getY()) / distance;
                    }
                }

                boid->setSeparation(separationX, separationY);
            }
        }
    }

    void boid(){
        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->boid();
                    }
                }
            }
        } else {
            vector<int> visible_boids_references;
            
            for(QuadTree* visible : visibles){
                for(int ref : visible->boids_references){
                    visible_boids_references.push_back(ref);
                }
            }
            
            this->allign(visible_boids_references);
            this->cohesion(visible_boids_references);
            this->avoid(visible_boids_references);
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
                Boid* boid = &this->boids->at(ref);
                boid->update(deltaTime);
            }
        }
    }

    void render(sf::RenderWindow& window, int cameraX = 0, int cameraY = 0, double zoom = 1.0) {
        sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
        rectangle.setPosition(static_cast<float>(x - cameraX), static_cast<float>(y - cameraY));
        selected ? rectangle.setFillColor(sf::Color::Blue) : rectangle.setFillColor(sf::Color::Transparent) ;
        rectangle.setOutlineColor(sf::Color::White);
        rectangle.setOutlineThickness(1.0f);
        window.draw(rectangle);

        // sf::Font font;
        // if (!font.loadFromFile("arial.ttf")) {
        //     cout << "Failed to load font!" << endl;
        //     return;
        // }
        // sf::Text text;
        // text.setFont(font);
        // if(!divided){
        //     text.setString(to_string(boids_references.size()));
        //     text.setCharacterSize(20);
        //     text.setFillColor(sf::Color::White);
        //     text.setPosition(static_cast<float>(x + width / 2), static_cast<float>(y + height / 2));
        //     window.draw(text);
        // }

        if(selected and !divided){
            for(QuadTree* visible : visibles){
                if(visible == this) continue;
                sf::RectangleShape visible_rectangle(sf::Vector2f(static_cast<float>(visible->width), static_cast<float>(visible->height)));
                visible_rectangle.setPosition(static_cast<float>(visible->x - cameraX), static_cast<float>(visible->y - cameraY));
                visible_rectangle.setFillColor(sf::Color(0, 100, 150, 100));
                window.draw(visible_rectangle);
            }
        }

        if (divided) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (children[i][j]) {
                        children[i][j]->render(window, cameraX, cameraY, zoom);
                    }
                }
            }
        } else {
            for (int ref : boids_references) {
                this->boids->at(ref).render(window, cameraX, cameraY, zoom);
            }
        }
    }
};