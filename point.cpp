#include <SFML/Graphics.hpp>

class Point{
    private:
    double x;
    double y;
    const float radius = 1.5f;

    public:
    Point(double _x = 0.0, double _y = 0.0) : x(_x), y(_y) {}

    double getX() const {return x;}
    double getY() const {return y;}

    void render(sf::RenderWindow& window) {
        sf::CircleShape shape(radius * 2);
        shape.setFillColor(sf::Color::White);
        shape.setPosition(static_cast<float>(x - radius), static_cast<float>(y - radius));
        window.draw(shape);
    }
};