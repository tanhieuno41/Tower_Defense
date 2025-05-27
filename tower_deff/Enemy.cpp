#include "Enemy.h"

Enemy::Enemy(const std::vector<sf::Vector2i>& pathTiles, sf::Color color, float speed_, int hp_)
    : currentTargetIndex(0), speed(speed_), hp(hp_), alive(true)
{
    // Chuyển path ô tile (x,y) sang tọa độ pixel ở giữa ô
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
    if (currentTargetIndex + 1 >= path.size()) return; // tới cuối rồi

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

void Enemy::draw(sf::RenderWindow& window) {
    if (alive)
        window.draw(shape);
}
