#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet(sf::Vector2f pos, sf::Vector2f dir);
    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
    bool isActive() const;
    void setActive(bool status);

private:
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;
    bool active;
};
