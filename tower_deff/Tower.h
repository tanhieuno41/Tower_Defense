#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"
#include "Enemy.h"

class Tower {
private:
    sf::RectangleShape shape;
    float shootTimer;
    float shootInterval;
    int level;
    float range;
    int damage;

public:
    Tower(sf::Vector2f pos);
    void update(float dt, std::vector<Bullet>& bullets, const std::vector<Enemy>& enemies);
    void draw(sf::RenderWindow& window) const;
    void upgrade();
    sf::Vector2f getPosition() const;
    int getLevel() const;
    float getRange() const;
};