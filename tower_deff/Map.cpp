#include "Map.h"
#include <queue>
#include <algorithm>
#include <fstream>

/* ==== BEGIN ORIGINAL ==== */
// copy NGUYÊN toàn bộ tileMap, pathPoints, hàm findPath(),
std::vector<std::vector<int>> tileMap;        // <-- duy nhất ở đây
std::vector<sf::Vector2i>     pathPoints;
// Map.cpp
void findPath() {
    pathPoints.clear();

    /* ---------- 1. XÁC ĐỊNH START / END ---------- */
    sf::Vector2i start(-1, -1), end(-1, -1);

    // 1.a  ─ quét toàn bộ map tìm ô 2 (START) & 3 (END)
    for (int y = 0; y < MAP_HEIGHT; ++y)
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (tileMap[y][x] == 2) start = { x, y };
            else if (tileMap[y][x] == 3) end = { x, y };
        }

    // 1.b  ─  nếu không gắn 2/3, fallback: lấy ô đường (1) bám mép trên & dưới
    if (start.x == -1) {
        for (int x = 0; x < MAP_WIDTH; ++x)
            if (tileMap[0][x] == 1) { start = { x, 0 }; break; }
    }
    if (end.x == -1) {
        for (int x = MAP_WIDTH - 1; x >= 0; --x)
            if (tileMap[MAP_HEIGHT - 1][x] == 1) { end = { x, MAP_HEIGHT - 1 }; break; }
    }

    // 1.c  ─  mép trái / phải (phòng xa)
    if (start.x == -1) {
        for (int y = 0; y < MAP_HEIGHT; ++y)
            if (tileMap[y][0] == 1) { start = { 0, y }; break; }
    }
    if (end.x == -1) {
        for (int y = MAP_HEIGHT - 1; y >= 0; --y)
            if (tileMap[y][MAP_WIDTH - 1] == 1) { end = { MAP_WIDTH - 1, y }; break; }
    }

    // Không tìm thấy đường –> thoát, tránh crash
    if (start.x == -1 || end.x == -1) return;

    /* ---------- 2. BFS TRÊN LƯỚI ---------- */
    std::vector<std::vector<bool>> visited(MAP_HEIGHT,
        std::vector<bool>(MAP_WIDTH, false));
    std::vector<std::vector<sf::Vector2i>> parent(MAP_HEIGHT,
        std::vector<sf::Vector2i>(MAP_WIDTH, { -1, -1 }));
    std::queue<sf::Vector2i> q;

    q.push(start);
    visited[start.y][start.x] = true;

    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { -1, 0, 1, 0 };

    while (!q.empty()) {
        sf::Vector2i cur = q.front(); q.pop();
        if (cur == end) break;

        for (int d = 0; d < 4; ++d) {
            int nx = cur.x + dx[d], ny = cur.y + dy[d];
            if (nx >= 0 && ny >= 0 && nx < MAP_WIDTH && ny < MAP_HEIGHT &&
                (tileMap[ny][nx] == 1 || tileMap[ny][nx] == 3) &&   // 1: đường, 3: END
                !visited[ny][nx]) {
                visited[ny][nx] = true;
                parent[ny][nx] = cur;
                q.push({ nx, ny });
            }
        }
    }

    /* ---------- 3. TRUY VẾT NGƯỢC ---------- */
    if (parent[end.y][end.x] == sf::Vector2i(-1, -1)) return;   // không tìm ra đường

    for (sf::Vector2i cur = end; cur != start; cur = parent[cur.y][cur.x])
        pathPoints.push_back(cur);
    pathPoints.push_back(start);
    std::reverse(pathPoints.begin(), pathPoints.end());
}


void loadAllMaps(std::vector<std::vector<std::vector<int>>>& maps, const int MAX_MAPS) {
	maps.clear();
	for (int i = 1; i <= MAX_MAPS; i++) {
		std::string filename = "maps/map" + std::to_string(i) + ".txt";
		std::ifstream file(filename);
		if (file.is_open()) {
			int width, height;
			file >> width >> height;

			std::vector<std::vector<int>> map(height, std::vector<int>(width));
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					file >> map[y][x];
				}
			}
			maps.push_back(map);
			file.close();
		}
	}
}
// và hàm loadAllMaps (nếu muốn tách ra) từ main.cpp
/* ==== END ORIGINAL ==== */  
