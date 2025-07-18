#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Enemy {
private:
    /* --- di chuyển --- */
    std::vector<sf::Vector2f> path;
    std::size_t  currentTargetIndex;

    float  baseSpeed;       // tốc cơ bản
    float  currentSpeed;    // tốc hiện tại (bị slow sẽ giảm)
    float  slowTimer;       // còn bao nhiêu giây slow

    /* --- máu --- */
    int    hp;
    int    maxHp;
    bool   alive;

    /* --- hiển thị --- */
    sf::CircleShape shape;

public:
    Enemy(const std::vector<sf::Vector2i>& pathTiles,
        sf::Color color,
        float speed,
        int   hp);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    /* --- va chạm & trạng thái --- */
    sf::FloatRect getBounds() const;
    void  takeDamage(int dmg);
    bool  isAlive() const;
    void  setAlive(bool status);
    bool  reachedEnd() const;

    sf::Vector2f getPosition() const;

    /* --- NEW: hiệu ứng slow --- */
    void  applySlow(float factor, float duration);
};
