#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "constants.h"

extern std::vector<std::vector<int>> tileMap;      // 0 = đất, 1 = đường
extern std::vector<sf::Vector2i>     pathPoints;   // lộ trình ô
struct WaveInfo {
    int numEnemies;
    float spawnInterval;
    sf::Color color;
};
void findPath();            // thuật toán BFS gốc
void loadAllMaps(std::vector<std::vector<std::vector<int>>>& maps, const int MAX_MAPS = 5);
