#include "Tower.h"
#include <cmath>

Tower::Tower(sf::Vector2f pos) : shootTimer(0.f), shootInterval(1.0f) {
    shape.setSize(sf::Vector2f(50, 50));
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(pos);
}

void Tower::update(float dt, std::vector<Bullet>& bullets, const std::vector<Enemy>& enemies) {
    shootTimer += dt;

    if (shootTimer >= shootInterval && !enemies.empty()) {
        sf::Vector2f targetPos = enemies[0].getBounds().getPosition();
        sf::Vector2f towerPos = shape.getPosition();
        sf::Vector2f dir = targetPos - towerPos;

        float length = sqrt(dir.x * dir.x + dir.y * dir.y);
        if (length != 0)
            dir /= length;

        bullets.emplace_back(towerPos + sf::Vector2f(25, 25), dir);
        shootTimer = 0;
    }
}

void Tower::draw(sf::RenderWindow& window) {
    window.draw(shape);
}
