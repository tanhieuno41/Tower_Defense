#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

class Bullet {
public:
    void draw(sf::RenderWindow& window) const {
        window.draw(shape);
    }
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed = 300.f;
    bool active = true;

    Bullet(sf::Vector2f startPos, sf::Vector2f targetPos) {
        position = startPos;
        shape.setRadius(5.f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(position);

        sf::Vector2f direction = targetPos - startPos;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0) direction /= length;
        velocity = direction * speed;
    }

    void update(float deltaTime) {
        position += velocity * deltaTime;
        shape.setPosition(position);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};
