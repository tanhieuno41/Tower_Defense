#include "Bullet.h"

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f dir, int dmg) : direction(dir), speed(700.f), active(true), damage(dmg) {
    shape.setRadius(5);
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(pos);
}


void Bullet::update(float dt) {
    if (active) {
        shape.move(direction * speed * dt);

        // Remove bullet if it goes out of bounds
        sf::Vector2f pos = shape.getPosition();
        if (pos.x < 0 || pos.x > 960 || pos.y < 0 || pos.y > 768) {
            active = false;
        }
    }
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (active) window.draw(shape);
}

sf::FloatRect Bullet::getBounds() const {
    return shape.getGlobalBounds();
}

bool Bullet::isActive() const {
    return active;
}

void Bullet::setActive(bool status) {
    active = status;
}

int Bullet::getDamage() const {
    return damage;
}
