#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Enemy {
private:
    sf::CircleShape shape;
    std::vector<sf::Vector2f> path;
    int currentTargetIndex;
    float speed;
    int hp;
    int maxHp;
    bool alive;

public:
    Enemy(const std::vector<sf::Vector2i>& pathTiles, sf::Color color, float speed_, int hp_);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
    void takeDamage(int damage);
    bool isAlive() const;
    void setAlive(bool status);
    bool reachedEnd() const;
    sf::Vector2f getPosition() const;
};