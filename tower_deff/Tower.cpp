#include "Tower.h"
#include <cmath>
#include <iostream>
#include <cassert>

/* ========== BẬT / TẮT log ========== */
#define TOWER_DEBUG 1        // 0 = tắt

/* ------- biến tĩnh ------- */
int Tower::nextId = 0;

/* ===== CONSTRUCTOR ===== */
Tower::Tower(sf::Vector2f pos)
{
    id = nextId++;                 // gán id duy nhất

    shape.setSize({ 50.f, 50.f });
    shape.setOrigin(25.f, 25.f);   // tâm
    assert(shape.getOrigin() == sf::Vector2f(25.f, 25.f));
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(pos);
}

/* ===== UPDATE ===== */
void Tower::update(float dt,
    std::vector<Bullet>& bullets,
    const std::vector<Enemy>& enemies)
{
    shootTimer += dt;
    if (shootTimer < shootInterval) return;

    const float range2 = range * range;
    Enemy* target = nullptr;
    float   bestDist2 = range2;         // chỉ nhận enemy < range

#if TOWER_DEBUG
    std::cout << "[T" << id << "] cooldown ok  ("
        << enemies.size() << " enemies)\n";
#endif

    const sf::Vector2f pos = shape.getPosition();

    /* --- quét enemy --- */
    for (const auto& e : enemies)
    {
        if (!e.isAlive()) continue;

        sf::FloatRect b = e.getBounds();
        sf::Vector2f   ep(b.left + b.width * 0.5f,
            b.top + b.height * 0.5f);

        sf::Vector2f d = ep - pos;
        float dist2 = d.x * d.x + d.y * d.y;

#if TOWER_DEBUG
        std::cout << "    dist = " << std::sqrt(dist2) << '\n';
#endif
        if (dist2 < bestDist2)
        {
            bestDist2 = dist2;
            target = const_cast<Enemy*>(&e);
        }
    }

#if TOWER_DEBUG
    if (!target) std::cout << "  -> NO TARGET\n\n";
#endif
    if (!target) return;

    /* --- tạo đạn --- */
    sf::FloatRect tb = target->getBounds();
    sf::Vector2f tgt(tb.left + tb.width * 0.5f,
        tb.top + tb.height * 0.5f);
    sf::Vector2f dir = tgt - pos;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f) dir /= len;

    /* xác định BulletType theo level (tùy ý) */
    BulletType bt = (level == 1) ? BulletType::NORMAL
        : (level == 2) ? BulletType::PIERCING
        : BulletType::SLOW;

    bullets.emplace_back(pos, dir, damage, bt);
    shootTimer = 0.f;

#if TOWER_DEBUG
    std::cout << "  -> FIRE (lvl " << level << ", damage "
        << damage << ", type " << int(bt) << ")\n\n";
#endif
}

/* ===== RENDER ===== */
void Tower::draw(sf::RenderWindow& w) const
{
    w.draw(shape);

    // level text (nếu cần font toàn cục thì thêm vào)
}

/* ===== UPGRADE ===== */
void Tower::upgrade()
{
    ++level;
    damage += 10;
    range += 20.f;
    shootInterval *= 0.9f;

    switch (level)
    {
    case 2: shape.setFillColor(sf::Color(0, 150, 255)); break;
    case 3: shape.setFillColor(sf::Color(150, 0, 255)); break;
    default:shape.setFillColor(sf::Color(255, 0, 150)); break;
    }
}

/* ===== GETTERS ===== */
sf::Vector2f Tower::getPosition() const { return shape.getPosition(); }
int   Tower::getLevel()   const { return level; }
float Tower::getRange()   const { return range; }
