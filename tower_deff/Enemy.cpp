#include "Enemy.h"
#include <cmath>

Enemy::Enemy(const std::vector<sf::Vector2i>& pathTiles, sf::Color color, float speed_, int hp_)
    : currentTargetIndex(0), speed(speed_), hp(hp_), maxHp(hp_), alive(true)
{
    for (auto& tile : pathTiles) {
        path.emplace_back(tile.x * 64 + 32, tile.y * 64 + 32);
    }
    shape.setRadius(20);
    shape.setOrigin(20, 20);
    shape.setFillColor(color);
    shape.setPosition(path[0]);
}

void Enemy::update(float deltaTime) {
    if (!alive) return;
    if (currentTargetIndex + 1 >= path.size()) return;

    sf::Vector2f currentPos = shape.getPosition();
    sf::Vector2f targetPos = path[currentTargetIndex + 1];
    sf::Vector2f direction = targetPos - currentPos;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance < 1.f) {
        currentTargetIndex++;
    }
    else {
        sf::Vector2f velocity = (direction / distance) * speed * deltaTime;
        shape.move(velocity);
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (alive) {
        window.draw(shape);

        // Draw health bar background
        sf::RectangleShape healthBg(sf::Vector2f(40, 6));
        healthBg.setPosition(shape.getPosition().x - 20, shape.getPosition().y - 30);
        healthBg.setFillColor(sf::Color::Black);
        window.draw(healthBg);

        // Draw health bar with color based on health percentage
        float healthPercent = (float)hp / (float)maxHp;
        sf::RectangleShape healthBar(sf::Vector2f(40 * healthPercent, 6));
        healthBar.setPosition(shape.getPosition().x - 20, shape.getPosition().y - 30);

        if (healthPercent > 0.6f) {
            healthBar.setFillColor(sf::Color::Green);
        }
        else if (healthPercent > 0.3f) {
            healthBar.setFillColor(sf::Color::Yellow);
        }
        else {
            healthBar.setFillColor(sf::Color::Red);
        }

        window.draw(healthBar);
    }
}

sf::FloatRect Enemy::getBounds() const {
    return shape.getGlobalBounds();
}

void Enemy::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        alive = false;
        hp = 0;
    }
}

bool Enemy::isAlive() const {
    return alive;
}

void Enemy::setAlive(bool status) {
    alive = status;
}

bool Enemy::reachedEnd() const {
    return currentTargetIndex + 1 >= path.size();
}

sf::Vector2f Enemy::getPosition() const {
    return shape.getPosition();
}