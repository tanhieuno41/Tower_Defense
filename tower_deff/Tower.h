#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include "Enemy.h"  // Cần để biết enemy position

class Tower {
public:
    sf::CircleShape shape;
    float range = 150.f;      // Tầm bắn
    float fireCooldown = 1.f; // Thời gian giữa 2 lần bắn (giây)
    float fireTimer = 0.f;    // Bộ đếm thời gian bắn

    Tower(sf::Vector2f position) {
        shape.setRadius(25.f);
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(position);
    }

    // Kiểm tra khoảng cách đến enemy
    bool inRange(const Enemy& enemy) {
        sf::Vector2f towerCenter = shape.getPosition() + sf::Vector2f(shape.getRadius(), shape.getRadius());
        sf::Vector2f enemyCenter = enemy.shape.getPosition() + sf::Vector2f(enemy.shape.getRadius(), enemy.shape.getRadius());
        float dx = towerCenter.x - enemyCenter.x;
        float dy = towerCenter.y - enemyCenter.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance <= range;
    }

    void update(float deltaTime, std::vector<class Bullet>& bullets, const std::vector<Enemy>& enemies);
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

};