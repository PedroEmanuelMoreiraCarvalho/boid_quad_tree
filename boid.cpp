#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace std;

class Boid {
    private:
    double x;
    double y;
    double velocityX;
    double velocityY;
    double cohesionX;
    double cohesionY;
    double alignmentX;
    double alignmentY;
    double separationX;
    double separationY;
    double next_velocityX;
    double next_velocityY;
    
    public:
    const static constexpr int FIELD_WIDTH = 1800, FIELD_HEIGHT = 1600;
    const static constexpr int LEFT_MARGIN = FIELD_WIDTH / 10, RIGHT_MARGIN = FIELD_WIDTH - FIELD_WIDTH / 10, TOP_MARGIN = FIELD_HEIGHT / 10, BOTTOM_MARGIN = FIELD_HEIGHT - FIELD_HEIGHT / 10;
    const static constexpr double vision_range = 30.0;
    const static constexpr double avoid_range = 30.0;
    const static constexpr double avg_speed = 100.0;
    const static constexpr double separation_weight = 2.0;
    const static constexpr double alignment_weight = 0.7;
    const static constexpr double cohesion_weight = 0.56;
    const static constexpr double turn_factor = 5.0;
    Boid(double _x = 0.0, double _y = 0.0) : x(_x), y(_y), velocityX(rand() % (int)avg_speed - avg_speed/2.0), velocityY(rand() % (int)avg_speed - avg_speed/2.0), next_velocityX(0.0), next_velocityY(0.0) {}

    double getX() const {return x;}
    double getY() const {return y;}
    void setX(double x){this->x = x;}
    void setY(double y){this->y = y;}
    double getVelocityX() const {return velocityX;}
    double getVelocityY() const {return velocityY;}
    void setVelocityX(double velocity){velocityX = velocity;}
    void setVelocityY(double velocity){velocityY = velocity;}
    void setNextVelocityX(double velocity){next_velocityX = velocity;}
    void setNextVelocityY(double velocity){next_velocityY = velocity;}
    void setAlignment(double alignX, double alignY){alignmentX = alignX; alignmentY = alignY;}
    void setCohesion(double cohX, double cohY){cohesionX = cohX; cohesionY = cohY;}
    void setSeparation(double sepX, double sepY){separationX = sepX; separationY = sepY;}

    void setDestin(double destX, double destY) {
        double dirX = destX - x;
        double dirY = destY - y;
        double length = sqrt(dirX * dirX + dirY * dirY);
        if (length > 0) {
            velocityX = (dirX / length) * avg_speed; // Use the avg_speed variable
            velocityY = (dirY / length) * avg_speed;
        }
    }

    void update(float deltaTime) {
        velocityX += (cohesionX * cohesion_weight + alignmentX * alignment_weight + separationX * separation_weight);
        velocityY += (cohesionY * cohesion_weight + alignmentY * alignment_weight + separationY * separation_weight);

        double speed = sqrt(velocityX * velocityX + velocityY * velocityY);

        if (speed > avg_speed) {
            velocityX = (velocityX / speed) * avg_speed;
            velocityY = (velocityY / speed) * avg_speed;
        }
        if(speed == 0){
            velocityX = (rand() % (int)avg_speed - avg_speed/2.0);
            velocityY = (rand() % (int)avg_speed - avg_speed/2.0);
        }
        
        if(x < LEFT_MARGIN) velocityX += turn_factor;
        else if(x > RIGHT_MARGIN) velocityX -= turn_factor;
        if(y < TOP_MARGIN) velocityY += turn_factor;
        else if(y > BOTTOM_MARGIN) velocityY -= turn_factor;

        x += velocityX * deltaTime;
        y += velocityY * deltaTime;

        return;
    }

    void render(sf::RenderWindow& window, int cameraX = 0, int cameraY = 0, double zoom = 1.0) {
        sf::ConvexShape shape(4);
        shape.setPoint(0, sf::Vector2f(10.0f, 0.0f));
        shape.setPoint(1, sf::Vector2f(-5.0f, 5.0f));
        shape.setPoint(2, sf::Vector2f(-5.0f, -5.0f));
        shape.setPoint(3, sf::Vector2f(10.0f, 0.0f));
        shape.setPosition((float)(x - cameraX) * zoom, (float)(y - cameraY) * zoom);
        shape.rotate(atan2(velocityY, velocityX) * 180.0f / 3.14159265f);
        shape.scale((float)zoom, (float)zoom);
        window.draw(shape);

        // sf::CircleShape visionCircle(static_cast<float>(vision_range));
        // visionCircle.setPosition(static_cast<float>(x - vision_range), static_cast<float>(y - vision_range));
        // visionCircle.setFillColor(sf::Color(255, 255, 255, 20));
        // visionCircle.setOutlineColor(sf::Color(255, 255, 255, 100));
        // visionCircle.setOutlineThickness(1.0f);
        // window.draw(visionCircle);
    }
};