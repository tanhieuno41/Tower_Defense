#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include "Tower.h"
#include  "Bullet.h"
const int TILE_SIZE = 64;
const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 8;

int tileMap[MAP_HEIGHT][MAP_WIDTH] = {
	{0,0,1,1,1,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,1,1,1,1,1,0},
	{0,0,0,0,0,0,0,0,1,0},
	{0,0,0,0,0,0,0,0,1,0},
	{0,0,0,0,0,0,0,0,1,0},
	{0,0,0,0,0,0,0,0,1,0},
	{0,0,0,0,0,0,0,0,1,0}
};
std::vector<sf::Vector2i> pathPoints;
void findPath() {
	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			if (tileMap[y][x] == 1) {
				pathPoints.push_back(sf::Vector2i(x, y));
			}
		}
	}
}
int main() {
   
    findPath();  // Phải tìm đường trước

    sf::RenderWindow window(sf::VideoMode(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE), "Tower Defense - Map");

    Enemy enemy;
    Tower tower(sf::Vector2f(300, 200));
    Tower tower1(sf::Vector2f(450,450));
    std::vector<Enemy> enemies = { enemy };
    std::vector<Bullet> bullets;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        for (auto& e : enemies) {
            e.update(deltaTime);
        }

        tower.update(deltaTime, bullets, enemies);
        tower1.update(deltaTime, bullets, enemies);
        for (auto& b : bullets) {
            b.update(deltaTime);
        }

       
        window.clear();
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                sf::RectangleShape tile(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
                tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);

                if (tileMap[y][x] == 1)
                    tile.setFillColor(sf::Color(192, 128, 0));  
                else
                    tile.setFillColor(sf::Color(0, 128, 0));   

                window.draw(tile);
            }
        }
        for (const auto& e : enemies)
            e.draw(window);

        tower.draw(window);
        tower1.draw(window);
        for (const auto& b : bullets)
            b.draw(window);

        window.display();
    }

    return 0;
}
