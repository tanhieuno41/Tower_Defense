#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include "Tower.h"
#include "Bullet.h"
#include <queue>
#include <vector>
#include <algorithm>

const int TILE_SIZE = 64;
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 12;

// 0 = đất, 1 = đường đi
int tileMap[MAP_HEIGHT][MAP_WIDTH] = {
    {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,1,1,1,1,1,0,0,0,0},
    {0,1,1,0,1,0,1,0,0,0,1,0,0,0,0},
    {0,1,1,0,1,1,1,0,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,0,1,0,1,1,1,1,0},
    {0,1,0,0,0,0,0,0,1,0,1,0,0,1,0},
    {0,1,0,0,0,0,0,0,1,0,1,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}
};

std::vector<sf::Vector2i> pathPoints;

void findPath() {
    pathPoints.clear();

    std::vector<std::vector<bool>> visited(MAP_HEIGHT, std::vector<bool>(MAP_WIDTH, false));
    std::vector<std::vector<sf::Vector2i>> parent(MAP_HEIGHT, std::vector<sf::Vector2i>(MAP_WIDTH, { -1, -1 }));

    std::queue<sf::Vector2i> q;
    

    // Tìm điểm bắt đầu: ô đầu tiên tile == 1 trên hàng 0
    sf::Vector2i start = { 2, 0 };
    sf::Vector2i end = { 13, 11 };

    q.push(start);
    visited[start.y][start.x] = true;

    int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };

    while (!q.empty()) {
        sf::Vector2i current = q.front(); q.pop();
        if (current == end) break;

        for (int d = 0; d < 4; ++d) {
            int nx = current.x + dx[d];
            int ny = current.y + dy[d];

            if (nx >= 0 && ny >= 0 && nx < MAP_WIDTH && ny < MAP_HEIGHT
                && tileMap[ny][nx] == 1 && !visited[ny][nx]) {
                visited[ny][nx] = true;
                parent[ny][nx] = current;
                q.push({ nx, ny });
            }
        }
    }

    sf::Vector2i cur = end;
    while (cur != start && parent[cur.y][cur.x] != sf::Vector2i(-1, -1)) {
        pathPoints.push_back(cur);
        cur = parent[cur.y][cur.x];
    }
    pathPoints.push_back(start);
    std::reverse(pathPoints.begin(), pathPoints.end());
}

int main() {
    findPath();

    sf::RenderWindow window(sf::VideoMode(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE), "Tower Defense - Bigger Map");

    std::vector<Enemy> enemies;
    std::queue<std::pair<int, sf::Color>> enemyQueue;
    enemyQueue.push({ 100, sf::Color::Red });
    enemyQueue.push({ 150, sf::Color::Blue });
    enemyQueue.push({ 200, sf::Color::Magenta });
    enemyQueue.push({ 180, sf::Color::Cyan });
    enemyQueue.push({ 220, sf::Color::Yellow });
    enemyQueue.push({ 250, sf::Color::Green });

    // 4 tower trên map
    std::vector<Tower> towers;
    towers.emplace_back(sf::Vector2f(3 * TILE_SIZE + TILE_SIZE / 2, 5 * TILE_SIZE + TILE_SIZE / 2));
    towers.emplace_back(sf::Vector2f(7 * TILE_SIZE + TILE_SIZE / 2, 3 * TILE_SIZE + TILE_SIZE / 2));
    towers.emplace_back(sf::Vector2f(10 * TILE_SIZE + TILE_SIZE / 2, 7 * TILE_SIZE + TILE_SIZE / 2));
    towers.emplace_back(sf::Vector2f(13 * TILE_SIZE + TILE_SIZE / 2, 9 * TILE_SIZE + TILE_SIZE / 2));

    std::vector<Bullet> bullets;

    sf::Clock clock;
    float spawnTimer = 0.f;
    float spawnInterval = 1.5f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        spawnTimer += deltaTime;

        if (spawnTimer >= spawnInterval && !enemyQueue.empty()) {
            auto data = enemyQueue.front();
            enemyQueue.pop();
            // Truyền đường đi và màu cho enemy mới, speed và hp tùy ý
            enemies.emplace_back(pathPoints, data.second, 100.f, data.first);
            spawnTimer = 0.f;
        }

        for (auto& e : enemies) {
            e.update(deltaTime);
        }

        for (auto& tower : towers) {
            tower.update(deltaTime, bullets, enemies);
        }

        for (auto& b : bullets) {
            b.update(deltaTime);
            for (auto& e : enemies) {
                if (b.getBounds().intersects(e.getBounds())) {
                    e.takeDamage(20);
                    b.setActive(false);
                }
            }
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) {
            return !b.isActive();
            }), bullets.end());

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) {
            return !e.isAlive();
            }), enemies.end());

        window.clear();

        // Vẽ map
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                sf::RectangleShape tile(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
                tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                if (tileMap[y][x] == 1)
                    tile.setFillColor(sf::Color(192, 128, 0)); // màu đường
                else
                    tile.setFillColor(sf::Color(0, 128, 0)); // màu đất
                window.draw(tile);
            }
        }

        // Vẽ path (đường đi) để dễ nhìn
        for (auto& p : pathPoints) {
            sf::CircleShape circle(10);
            circle.setFillColor(sf::Color::White);
            circle.setPosition(p.x * TILE_SIZE + TILE_SIZE / 2 - 10, p.y * TILE_SIZE + TILE_SIZE / 2 - 10);
            window.draw(circle);
        }

        for (auto& e : enemies)
            e.draw(window);

        for (auto& tower : towers)
            tower.draw(window);

        for (auto& b : bullets)
            b.draw(window);
        for (auto& p : pathPoints) {
            sf::CircleShape circle(20);
            circle.setOrigin(20, 20);
            circle.setPosition(p.x * TILE_SIZE + TILE_SIZE / 2, p.y * TILE_SIZE + TILE_SIZE / 2);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(3);
            window.draw(circle);
        }
        window.display();
    }

    return 0;
}
