#include "Bullet.h"

static constexpr float BULLET_SPEED = 450.f;
static constexpr float BULLET_LIFE = 4.f;   // giây

Bullet::Bullet(sf::Vector2f pos,
    sf::Vector2f dir,
    int dmg,
    BulletType bt)
    : damage(dmg),
    type(bt),
    slowFactor(0.5f),
    lifeTime(BULLET_LIFE)
{
    shape.setRadius(6);
    shape.setOrigin(6, 6);
    shape.setPosition(pos);

    switch (type) {
    case BulletType::NORMAL:   shape.setFillColor(sf::Color::Yellow); break;
    case BulletType::PIERCING: shape.setFillColor(sf::Color::Cyan);   break;
    case BulletType::SLOW:     shape.setFillColor(sf::Color::Magenta);break;
    }

    velocity = dir * BULLET_SPEED;
}

void Bullet::update(float dt)
{
    shape.move(velocity * dt);
    lifeTime -= dt;
}

bool Bullet::isActive() const { return lifeTime > 0.f; }
void Bullet::deactivate() { lifeTime = 0.f; }

void Bullet::draw(sf::RenderWindow& win) const { win.draw(shape); }
