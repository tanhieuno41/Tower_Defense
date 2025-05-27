#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"
#include "Enemy.h"

class Tower {
public:
    Tower(sf::Vector2f pos);
    void update(float dt, std::vector<Bullet>& bullets, const std::vector<Enemy>& enemies);
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape shape;
    float shootTimer;
    float shootInterval;
};
