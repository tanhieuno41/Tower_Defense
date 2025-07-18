#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"
#include "Enemy.h"
#include "Constants.h"     // (chứa BulletType)

class Tower {
private:
    sf::RectangleShape shape;

    /* combat */
    float shootTimer = 0.f;
    float shootInterval = 1.f;
    float range = 200.f;
    int   damage = 20;
    int   level = 1;

    /* debug / id */
    int   id;
    static int nextId;

public:
    explicit Tower(sf::Vector2f pos);

    void update(float dt,
        std::vector<Bullet>& bullets,
        const std::vector<Enemy>& enemies);

    void draw(sf::RenderWindow& w) const;
    void upgrade();

    sf::Vector2f getPosition() const;
    int   getLevel()   const;
    float getRange()   const;
};
