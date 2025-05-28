#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
private:
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;
    bool active;
    int damage;

public:
    Bullet(sf::Vector2f pos, sf::Vector2f dir, int dmg = 20);
    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
    bool isActive() const;
    void setActive(bool status);
    int getDamage() const;
    sf::Vector2f getPosition() const;
};