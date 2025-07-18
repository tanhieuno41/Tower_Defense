#include "Enemy.h"
#include <cmath>

/* ===== ctor ===== */
Enemy::Enemy(const std::vector<sf::Vector2i>& pathTiles,
    sf::Color color,
    float speed,
    int   hp_)
    : currentTargetIndex(0),
    baseSpeed(speed),
    currentSpeed(speed),
    slowTimer(0.f),
    hp(hp_),
    maxHp(hp_),
    alive(true)
{
    for (auto t : pathTiles)
        path.emplace_back(t.x * 64.f + 32.f, t.y * 64.f + 32.f);

    shape.setRadius(20.f);
    shape.setOrigin(20.f, 20.f);
    shape.setFillColor(color);
    shape.setPosition(path[0]);
}

/* ===== update ===== */
void Enemy::update(float dt)
{
    if (!alive) return;
    if (currentTargetIndex + 1 >= path.size()) return;

    /* --- cập nhật slow --- */
    if (slowTimer > 0.f) {
        slowTimer -= dt;
        if (slowTimer <= 0.f)            // hết slow
            currentSpeed = baseSpeed;
    }

    /* --- di chuyển --- */
    sf::Vector2f pos = shape.getPosition();
    sf::Vector2f target = path[currentTargetIndex + 1];
    sf::Vector2f dir = target - pos;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    if (dist < 1.f)
        ++currentTargetIndex;
    else
        shape.move(dir / dist * currentSpeed * dt);
}

/* ===== vẽ ===== */
void Enemy::draw(sf::RenderWindow& window) const
{
    if (!alive) return;

    window.draw(shape);

    /* --- thanh máu --- */
    sf::RectangleShape bg({ 40.f, 6.f });
    bg.setPosition(shape.getPosition().x - 20.f,
        shape.getPosition().y - 30.f);
    bg.setFillColor(sf::Color::Black);
    window.draw(bg);

    float hpPercent = static_cast<float>(hp) / maxHp;
    sf::RectangleShape bar({ 40.f * hpPercent, 6.f });
    bar.setPosition(bg.getPosition());

    if (hpPercent > 0.6f) bar.setFillColor(sf::Color::Green);
    else if (hpPercent > 0.3f) bar.setFillColor(sf::Color::Yellow);
    else                       bar.setFillColor(sf::Color::Red);
    window.draw(bar);
}

/* ===== các hàm tiện ích ===== */
sf::FloatRect Enemy::getBounds() const { return shape.getGlobalBounds(); }

void Enemy::takeDamage(int dmg)
{
    hp -= dmg;
    if (hp <= 0) { alive = false; hp = 0; }
}

bool  Enemy::isAlive()      const { return alive; }
void  Enemy::setAlive(bool s) { alive = s; }
bool  Enemy::reachedEnd()   const { return currentTargetIndex + 1 >= path.size(); }
sf::Vector2f Enemy::getPosition() const { return shape.getPosition(); }

/* ===== slow effect ===== */
void Enemy::applySlow(float factor, float duration)
{
    currentSpeed = baseSpeed * factor;   // factor < 1
    slowTimer = duration;
}
