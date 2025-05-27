#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Enemy {
private:
    sf::CircleShape shape;
    std::vector<sf::Vector2f> path; // đường đi (tọa độ pixel)
    size_t currentTargetIndex;
    float speed;
    int hp;
    bool alive;

public:
    Enemy(const std::vector<sf::Vector2i>& pathTiles, sf::Color color, float speed = 100.f, int hp = 100);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    bool isAlive() const { return alive; }
    void takeDamage(int dmg) { hp -= dmg; if (hp <= 0) alive = false; }
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
};
