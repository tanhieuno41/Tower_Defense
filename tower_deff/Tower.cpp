#include "Bullet.h"
#include "Tower.h"
void Tower::update(float deltaTime, std::vector<Bullet>& bullets, const std::vector<Enemy>& enemies) {
    fireTimer -= deltaTime;
    if (fireTimer > 0.f) return;

    for (const Enemy& enemy : enemies) {
        if (inRange(enemy)) {
            // Bắn đạn vào enemy
            sf::Vector2f towerCenter = shape.getPosition() + sf::Vector2f(shape.getRadius(), shape.getRadius());
            sf::Vector2f enemyCenter = enemy.shape.getPosition() + sf::Vector2f(enemy.shape.getRadius(), enemy.shape.getRadius() +10);
            bullets.emplace_back(towerCenter, enemyCenter);

            fireTimer = fireCooldown;  // reset timer
            break;
        }
    }
}
