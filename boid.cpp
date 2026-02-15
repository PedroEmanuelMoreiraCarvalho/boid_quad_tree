#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace std;

class Boid {
    private:
    double x;
    double y;
    double velocityX;
    double velocityY;
    double speed = 50.0; // Speed of 50 units per second

    public:
    const static constexpr double vision_range = 100.0;
    Boid(double _x = 0.0, double _y = 0.0) : x(_x), y(_y), velocityX(rand() % 100 - 50.0), velocityY(rand() % 100 - 50.0) {}

    double getX() const {return x;}
    double getY() const {return y;}
    double getVelocityX() const {return velocityX;}
    double getVelocityY() const {return velocityY;}
    void setVelocityX(double velocity){velocityX = velocity;}
    void setVelocityY(double velocity){velocityY = velocity;}

    void setDestin(double destX, double destY) {
        double dirX = destX - x;
        double dirY = destY - y;
        double length = sqrt(dirX * dirX + dirY * dirY);
        if (length > 0) {
            velocityX = (dirX / length) * speed; // Use the speed variable
            velocityY = (dirY / length) * speed;
        }
    }

    void update(float deltaTime) {
        x += velocityX * deltaTime;
        y += velocityY * deltaTime;
        return;
    }

    void render(sf::RenderWindow& window) {
        sf::ConvexShape shape(4);
        shape.setPoint(0, sf::Vector2f(10.0f, 0.0f));
        shape.setPoint(1, sf::Vector2f(-5.0f, 5.0f));
        shape.setPoint(2, sf::Vector2f(-5.0f, -5.0f));
        shape.setPoint(3, sf::Vector2f(10.0f, 0.0f));
        shape.setPosition((float)(x), (float)(y));
        shape.rotate(atan2(velocityY, velocityX) * 180.0f / 3.14159265f);
        window.draw(shape);

        sf::CircleShape visionCircle(static_cast<float>(vision_range));
        visionCircle.setPosition(static_cast<float>(x - vision_range), static_cast<float>(y - vision_range));
        visionCircle.setFillColor(sf::Color(255, 255, 255, 20));
        visionCircle.setOutlineColor(sf::Color(255, 255, 255, 100));
        visionCircle.setOutlineThickness(1.0f);
        window.draw(visionCircle);
    }
};