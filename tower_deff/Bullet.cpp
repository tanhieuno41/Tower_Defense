#include "Bullet.h"

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f dir) : direction(dir), speed(700.f), active(true) {
    shape.setRadius(5);
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(pos);
}

void Bullet::update(float dt) {
    if (active) {
        shape.move(direction * speed * dt);
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
