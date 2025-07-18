#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"       // thêm dòng này

class Bullet {
private:
    sf::CircleShape shape;
    sf::Vector2f    velocity;
    int             damage;
    BulletType      type;    // ★ mới

    float           slowFactor;   // chỉ dùng với SLOW
    float           lifeTime;     // tự hủy sau N giây

public:
    Bullet(sf::Vector2f pos,
        sf::Vector2f dir,
        int dmg,
        BulletType bt = BulletType::NORMAL);

    void   update(float dt);
    void   draw(sf::RenderWindow& win) const;
    bool   isActive()   const;
    void   deactivate();
    BulletType getType()  const { return type; }
    int    getDamage() const { return damage; }

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
};
