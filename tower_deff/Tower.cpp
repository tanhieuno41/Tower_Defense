#include "Tower.h"
#include <cmath>


Tower::Tower(sf::Vector2f pos) : shootTimer(0.f), shootInterval(1.0f), level(1), range(150.f), damage(20) {
    shape.setSize(sf::Vector2f(50, 50));
    shape.setOrigin(25, 25);
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(pos);
}

void Tower::update(float dt, std::vector<Bullet>& bullets, const std::vector<Enemy>& enemies) {
    shootTimer += dt;

    if (shootTimer >= shootInterval) {
        // Tìm enemy gần nhất trong tầm bắn
        Enemy* target = nullptr;
        float minDistance = range + 1;

        for (const auto& enemy : enemies) {
            if (!enemy.isAlive()) continue;

            sf::Vector2f towerPos = shape.getPosition();
            sf::Vector2f enemyPos = enemy.getBounds().getPosition();
            enemyPos.x += enemy.getBounds().width / 2;
            enemyPos.y += enemy.getBounds().height / 2;

            float distance = sqrt(pow(towerPos.x - enemyPos.x, 2) + pow(towerPos.y - enemyPos.y, 2));

            if (distance < range && distance < minDistance) {
                target = const_cast<Enemy*>(&enemy);
                minDistance = distance;
            }
        }

        if (target) {
            sf::Vector2f towerPos = shape.getPosition();
            sf::Vector2f targetPos = target->getBounds().getPosition();
            targetPos.x += target->getBounds().width / 2;
            targetPos.y += target->getBounds().height / 2;

            sf::Vector2f dir = targetPos - towerPos;
            float length = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (length != 0)
                dir /= length;
            bullets.emplace_back(towerPos, dir, damage);
            shootTimer = 0;
        }
    }
}

void Tower::draw(sf::RenderWindow& window) const {
    window.draw(shape);

    // Vẽ level indicator
    sf::Text levelText;
    // levelText.setFont(font); // Cần truyền font vào hoặc dùng font global
    levelText.setString(std::to_string(level));
    levelText.setCharacterSize(16);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(shape.getPosition().x - 5, shape.getPosition().y - 30);
    window.draw(levelText);
}

void Tower::upgrade() {
    level++;
    damage += 10;
    range += 20;
    shootInterval *= 0.9f; // Bắn nhanh hơn

    // Thay đổi màu theo level
    switch (level) {
    case 2:
        shape.setFillColor(sf::Color(0, 150, 255)); // Xanh đậm hơn
        break;
    case 3:
        shape.setFillColor(sf::Color(150, 0, 255)); // Tím
        break;
    default:
        shape.setFillColor(sf::Color(255, 0, 150)); // Hồng
        break;
    }
}

sf::Vector2f Tower::getPosition() const {
    return shape.getPosition();
}

int Tower::getLevel() const {
    return level;
}

float Tower::getRange() const {
    return range;
}
