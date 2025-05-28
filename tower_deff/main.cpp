#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include "Tower.h"
#include "Bullet.h"
#include "GameState.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

const int TILE_SIZE = 64;
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 12;
const int WINDOW_WIDTH = MAP_WIDTH * TILE_SIZE;
const int WINDOW_HEIGHT = MAP_HEIGHT * TILE_SIZE + 100; // Thêm chỗ cho UI

// Game States
enum class GameScreen {
	MAIN_MENU,
	PLAYING,
	PAUSED,
	GAME_OVER
};

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

// Thông tin cho mỗi wave
struct WaveInfo {
	int numEnemies;
	float spawnInterval;
	sf::Color color;
};

class Game {
public:
	sf::RenderWindow window;
	GameScreen currentScreen;
	sf::Font font;

	// Game state
	GameState gameState;
	std::vector<Enemy> enemies;
	std::queue<std::pair<int, sf::Color>> enemyQueue;
	std::vector<Tower> towers;
	std::vector<Bullet> bullets;

	sf::Clock clock;
	float spawnTimer;
	float spawnInterval;

	// Menu
	std::vector<sf::Text> menuOptions;
	int selectedOption;

	// UI
	sf::Text moneyText;
	sf::Text healthText;
	sf::Text waveText;
	sf::RectangleShape uiBackground;

	// Tower building
	bool buildingMode;
	sf::CircleShape towerPreview;
	sf::CircleShape rangePreview;

	// Selected tower for upgrade
	int selectedTowerIndex;

	// --- Wave system ---
	std::vector<WaveInfo> waves;
	int currentWaveIdx = 0;
	int enemiesSpawned = 0;
	int enemiesToSpawn = 0;
	float waveSpawnTimer = 0.f;
	bool waveInProgress = false;
	bool waitingForNextWave = false;

	static const int TARGET_FPS = 60;
	static const float TARGET_FRAME_TIME;
	sf::Clock frameClock;

	sf::VertexArray mapVertices;  // Vertex array cho map
	bool mapNeedsUpdate;  // Flag để biết khi nào cần update map

	Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tower Defense Game"),
		currentScreen(GameScreen::MAIN_MENU), selectedOption(0), spawnTimer(0.f),
		spawnInterval(1.5f), buildingMode(false), selectedTowerIndex(-1),
		mapVertices(sf::Quads), mapNeedsUpdate(true) {
		window.setFramerateLimit(TARGET_FPS);  // Giới hạn FPS

		if (!font.loadFromFile("arial.ttf")) {
			// Fallback nếu không load được font
			std::cout << "Không thể load font, sử dụng font mặc định\n";
		}

		setupMenu();
		setupUI();
		findPath();

		gameState.money = 500;
		gameState.health = 100;
		gameState.wave = 1;

		// Khởi tạo các wave mẫu
		waves.push_back({ 5, 1.0f, sf::Color::Red });
		waves.push_back({ 8, 0.8f, sf::Color::Blue });
		waves.push_back({ 12, 0.7f, sf::Color::Magenta });
		waves.push_back({ 16, 0.6f, sf::Color::Cyan });
		waves.push_back({ 20, 0.5f, sf::Color::Yellow });
		waves.push_back({ 25, 0.4f, sf::Color::Green });
		currentWaveIdx = 0;
		waveInProgress = false;
		waitingForNextWave = true;
		enemiesSpawned = 0;
		enemiesToSpawn = 0;
		waveSpawnTimer = 0.f;

		setupMapVertices();  // Khởi tạo vertex array cho map
	}

	void setupMenu() {
		menuOptions.resize(3);
		std::vector<std::string> options = { "New Game", "Continue", "Exit" };

		for (int i = 0; i < 3; i++) {
			menuOptions[i].setFont(font);
			menuOptions[i].setString(options[i]);
			menuOptions[i].setCharacterSize(48);
			menuOptions[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
			menuOptions[i].setPosition(WINDOW_WIDTH / 2 - 100, 200 + i * 80);
		}
	}

	void setupUI() {
		uiBackground.setSize(sf::Vector2f(WINDOW_WIDTH, 100));
		uiBackground.setPosition(0, MAP_HEIGHT * TILE_SIZE);
		uiBackground.setFillColor(sf::Color(50, 50, 50));

		moneyText.setFont(font);
		moneyText.setCharacterSize(24);
		moneyText.setFillColor(sf::Color::Yellow);
		moneyText.setPosition(10, MAP_HEIGHT * TILE_SIZE + 10);

		healthText.setFont(font);
		healthText.setCharacterSize(24);
		healthText.setFillColor(sf::Color::Red);
		healthText.setPosition(10, MAP_HEIGHT * TILE_SIZE + 40);

		waveText.setFont(font);
		waveText.setCharacterSize(24);
		waveText.setFillColor(sf::Color::Cyan);
		waveText.setPosition(10, MAP_HEIGHT * TILE_SIZE + 70);

		// Tower preview
		towerPreview.setRadius(25);
		towerPreview.setOrigin(25, 25);
		towerPreview.setFillColor(sf::Color(0, 0, 255, 128));

		rangePreview.setRadius(150);
		rangePreview.setOrigin(150, 150);
		rangePreview.setFillColor(sf::Color::Transparent);
		rangePreview.setOutlineColor(sf::Color(255, 255, 255, 100));
		rangePreview.setOutlineThickness(2);
	}

	void setupMapVertices() {
		mapVertices.clear();
		for (int y = 0; y < MAP_HEIGHT; ++y) {
			for (int x = 0; x < MAP_WIDTH; ++x) {
				sf::Color tileColor = (tileMap[y][x] == 1) ? 
					sf::Color(192, 128, 0) : sf::Color(0, 128, 0);
				
				// Thêm 4 vertex cho mỗi tile
				sf::Vertex v1(sf::Vector2f(x * TILE_SIZE, y * TILE_SIZE), tileColor);
				sf::Vertex v2(sf::Vector2f((x + 1) * TILE_SIZE, y * TILE_SIZE), tileColor);
				sf::Vertex v3(sf::Vector2f((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE), tileColor);
				sf::Vertex v4(sf::Vector2f(x * TILE_SIZE, (y + 1) * TILE_SIZE), tileColor);
				
				mapVertices.append(v1);
				mapVertices.append(v2);
				mapVertices.append(v3);
				mapVertices.append(v4);
			}
		}
	}

	void handleEvents() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (currentScreen) {
				case GameScreen::MAIN_MENU:
					handleMenuInput(event.key.code);
					break;
				case GameScreen::PLAYING:
					handleGameInput(event.key.code);
					break;
				case GameScreen::PAUSED:
					handlePauseInput(event.key.code);
					break;
					break;
				case GameScreen::GAME_OVER:
					if (event.key.code == sf::Keyboard::Escape)
						currentScreen = GameScreen::MAIN_MENU;
					break;
				}

			}

			if (event.type == sf::Event::MouseButtonPressed && currentScreen == GameScreen::PLAYING) {
				handleMouseInput(event.mouseButton);
			}
		}
	}

	void handleMenuInput(sf::Keyboard::Key key) {
		switch (key) {
		case sf::Keyboard::Up:
			selectedOption = (selectedOption - 1 + 3) % 3;
			updateMenuColors();
			break;
		case sf::Keyboard::Down:
			selectedOption = (selectedOption + 1) % 3;
			updateMenuColors();
			break;
		case sf::Keyboard::Enter:
			switch (selectedOption) {
			case 0: // New Game
				startNewGame();
				break;
			case 1: // Continue
				loadGame();
				break;
			case 2: // Exit
				window.close();
				break;
			}
			break;
		}
	}
	void handlePauseInput(sf::Keyboard::Key key) {

		switch (key) {
		case sf::Keyboard::Up:
			selectedOption = (selectedOption - 1 + 3) % 3;
			updateMenuColors();
			break;
		case sf::Keyboard::Down:
			selectedOption = (selectedOption + 1) % 3;
			updateMenuColors();
			break;
		case sf::Keyboard::Enter:
			switch (selectedOption) {
			case 0: // New Game
				currentScreen = GameScreen::PLAYING;
				break;
				
			case 1: // Continue
				saveGame();
				currentScreen = GameScreen::MAIN_MENU;
				break;
				
			}
			break;
		}
	}

		void handleGameInput(sf::Keyboard::Key key) {
			switch (key) {
			case sf::Keyboard::Escape:
				currentScreen = GameScreen::PAUSED;
				break;
			case sf::Keyboard::S:
				saveGame();
				break;
			case sf::Keyboard::B:
				buildingMode = !buildingMode;
				break;
			}
		}

		void handleMouseInput(sf::Event::MouseButtonEvent mouse) {
			if (buildingMode && mouse.button == sf::Mouse::Left) {
				// Build tower
				int gridX = mouse.x / TILE_SIZE;
				int gridY = mouse.y / TILE_SIZE;

				if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT
					&& tileMap[gridY][gridX] == 0 && gameState.money >= 100) {

					sf::Vector2f pos(gridX * TILE_SIZE + TILE_SIZE / 2, gridY * TILE_SIZE + TILE_SIZE / 2);

					// Kiểm tra không có tower nào ở vị trí này
					bool canBuild = true;
					for (const auto& tower : towers) {
						sf::Vector2f towerPos = tower.getPosition();
						float distance = sqrt(pow(pos.x - towerPos.x, 2) + pow(pos.y - towerPos.y, 2));
						if (distance < 60) {
							canBuild = false;
							break;
						}
					}

					if (canBuild) {
						towers.emplace_back(pos);
						gameState.money -= 100;
						buildingMode = false;
					}
				}
			}
			else if (mouse.button == sf::Mouse::Right) {
				// Select tower for upgrade
				selectedTowerIndex = -1;
				for (int i = 0; i < towers.size(); i++) {
					sf::Vector2f towerPos = towers[i].getPosition();
					float distance = sqrt(pow(mouse.x - towerPos.x, 2) + pow(mouse.y - towerPos.y, 2));
					if (distance < 30) {
						selectedTowerIndex = i;
						break;
					}
				}
			}
			else if (mouse.button == sf::Mouse::Middle && selectedTowerIndex != -1) {
				// Upgrade tower
				if (gameState.money >= 150) {
					towers[selectedTowerIndex].upgrade();
					gameState.money -= 150;
					selectedTowerIndex = -1;
				}
			}
		}

		void updateMenuColors() {
			for (int i = 0; i < menuOptions.size(); i++) {
				menuOptions[i].setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
			}
		}

		void startNewGame() {
			currentScreen = GameScreen::PLAYING;
			gameState.reset();
			gameState.money = 500;
			gameState.health = 100;
			gameState.wave = 1;
			enemies.clear();
			towers.clear();
			bullets.clear();
			currentWaveIdx = 0;
			waveInProgress = false;
			waitingForNextWave = true;
			enemiesSpawned = 0;
			enemiesToSpawn = 0;
			waveSpawnTimer = 0.f;
		}

		void saveGame() {
			std::ofstream file("savegame.dat");
			if (file.is_open()) {
				file << gameState.money << " " << gameState.health << " " << gameState.wave << "\n";
				file << towers.size() << "\n";
				for (const auto& tower : towers) {
					sf::Vector2f pos = tower.getPosition();
					file << pos.x << " " << pos.y << " " << tower.getLevel() << "\n";
				}
				file.close();
				std::cout << "Game saved!\n";
			}
		}

		void loadGame() {
			std::ifstream file("savegame.dat");
			if (file.is_open()) {
				file >> gameState.money >> gameState.health >> gameState.wave;
				int towerCount;
				file >> towerCount;
				towers.clear();
				for (int i = 0; i < towerCount; i++) {
					float x, y;
					int level;
					file >> x >> y >> level;
					towers.emplace_back(sf::Vector2f(x, y));
					for (int j = 1; j < level; j++) {
						towers.back().upgrade();
					}
				}
				file.close();
				currentScreen = GameScreen::PLAYING;
				enemies.clear();
				bullets.clear();
				// Khôi phục wave
				currentWaveIdx = gameState.wave - 1;
				waveInProgress = false;
				waitingForNextWave = true;
				enemiesSpawned = 0;
				enemiesToSpawn = 0;
				waveSpawnTimer = 0.f;
				std::cout << "Game loaded!\n";
			}
			else {
				std::cout << "No save file found!\n";
			}
		}

		void update(float deltaTime) {
			if (currentScreen != GameScreen::PLAYING) return;

			waveSpawnTimer += deltaTime;

			// Bắt đầu wave mới khi nhấn Space
			if (waitingForNextWave && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				if (currentWaveIdx < waves.size()) {
					waveInProgress = true;
					waitingForNextWave = false;
					enemiesSpawned = 0;
					enemiesToSpawn = waves[currentWaveIdx].numEnemies;
					waveSpawnTimer = 0.f;
					gameState.wave = currentWaveIdx + 1;
				}
			}

			// Spawn enemies theo wave
			if (waveInProgress && enemiesSpawned < enemiesToSpawn) {
				if (waveSpawnTimer >= waves[currentWaveIdx].spawnInterval) {
					enemies.emplace_back(pathPoints, waves[currentWaveIdx].color, 100.f, 100 + 10 * currentWaveIdx);
					enemiesSpawned++;
					waveSpawnTimer = 0.f;
				}
			}

			// Khi đã spawn hết và không còn quái trên bản đồ, chuyển sang wave tiếp
			if (waveInProgress && enemiesSpawned >= enemiesToSpawn && enemies.empty()) {
				currentWaveIdx++;
				waveInProgress = false;
				waitingForNextWave = true;
			}

			// Update enemies
			for (auto& e : enemies) {
				e.update(deltaTime);
				if (e.reachedEnd()) {
					gameState.health -= 10;
					e.setAlive(false);
				}
			}

			// Update towers
			for (auto& tower : towers) {
				tower.update(deltaTime, bullets, enemies);
			}

			// Update bullets và kiểm tra va chạm tối ưu
			for (auto& b : bullets) {
				if (!b.isActive()) continue;  // Bỏ qua bullet không active
				
				b.update(deltaTime);
				sf::FloatRect bulletBounds = b.getBounds();
				
				// Chỉ kiểm tra va chạm với enemies trong phạm vi
				for (auto& e : enemies) {
					if (!e.isAlive()) continue;  // Bỏ qua enemy đã chết
					
					// Kiểm tra khoảng cách trước khi kiểm tra va chạm
					sf::Vector2f bulletPos = b.getPosition();
					sf::Vector2f enemyPos = e.getPosition();
					float dx = bulletPos.x - enemyPos.x;
					float dy = bulletPos.y - enemyPos.y;
					float distanceSquared = dx * dx + dy * dy;
					
					// Nếu khoảng cách nhỏ hơn tổng bán kính + một khoảng buffer
					if (distanceSquared < 1000) {  // 1000 = (20 + 20)^2 + buffer
						if (b.getBounds().intersects(e.getBounds())) {
							e.takeDamage(20);
							if (!e.isAlive()) {
								gameState.money += 25;
							}
							b.setActive(false);
							break;  // Bullet đã trúng, không cần kiểm tra với enemy khác
						}
					}
				}
			}

			// Remove inactive bullets and dead enemies
			bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) {
				return !b.isActive();
			}), bullets.end());

			enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) {
				return !e.isAlive();
			}), enemies.end());

			// Check game over
			if (gameState.health <= 0) {
				currentScreen = GameScreen::GAME_OVER;
			}

			// Update UI text
			moneyText.setString("Money: $" + std::to_string(gameState.money));
			healthText.setString("Health: " + std::to_string(gameState.health));
			waveText.setString("Wave: " + std::to_string(gameState.wave) + "/" + std::to_string((int)waves.size()));
		}

		void render() {
			window.clear();

			switch (currentScreen) {
			case GameScreen::MAIN_MENU:
				renderMenu();
				break;
			case GameScreen::PLAYING:
				renderGame();
				break;
			case GameScreen::PAUSED:
				renderGame();
				renderPauseMenu(window, font, selectedOption);
				break;
			case GameScreen::GAME_OVER:
				renderGameOver();
				break;
			}

			window.display();
		}

		void renderMenu() {
			sf::Text title;
			title.setFont(font);
			title.setString("Tower Defense");
			title.setCharacterSize(72);
			title.setFillColor(sf::Color::White);
			title.setPosition(WINDOW_WIDTH / 2 - 200, 50);
			window.draw(title);

			for (const auto& option : menuOptions) {
				window.draw(option);
			}
		}

		void renderGame() {
			// Draw map using vertex array
			window.draw(mapVertices);

			// Draw path (chỉ vẽ một lần khi cần)
			static sf::VertexArray pathVertices(sf::Quads);
			if (pathVertices.getVertexCount() == 0) {
				for (auto& p : pathPoints) {
					float x = p.x * TILE_SIZE + TILE_SIZE / 2 - 10;
					float y = p.y * TILE_SIZE + TILE_SIZE / 2 - 10;
					
					sf::Vertex v1(sf::Vector2f(x, y), sf::Color::White);
					sf::Vertex v2(sf::Vector2f(x + 20, y), sf::Color::White);
					sf::Vertex v3(sf::Vector2f(x + 20, y + 20), sf::Color::White);
					sf::Vertex v4(sf::Vector2f(x, y + 20), sf::Color::White);
					
					pathVertices.append(v1);
					pathVertices.append(v2);
					pathVertices.append(v3);
					pathVertices.append(v4);
				}
			}
			window.draw(pathVertices);

			// Draw game objects
			for (auto& e : enemies)
				e.draw(window);

			for (auto& tower : towers)
				tower.draw(window);

			for (auto& b : bullets)
				b.draw(window);

			// Draw selected tower highlight
			if (selectedTowerIndex != -1) {
				sf::CircleShape highlight(40);
				highlight.setOrigin(40, 40);
				highlight.setPosition(towers[selectedTowerIndex].getPosition());
				highlight.setFillColor(sf::Color::Transparent);
				highlight.setOutlineColor(sf::Color::Yellow);
				highlight.setOutlineThickness(3);
				window.draw(highlight);
			}

			// Draw building mode preview
			if (buildingMode) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				towerPreview.setPosition(mousePos.x, mousePos.y);
				rangePreview.setPosition(mousePos.x, mousePos.y);
				window.draw(rangePreview);
				window.draw(towerPreview);
			}

			// Draw UI
			window.draw(uiBackground);
			window.draw(moneyText);
			window.draw(healthText);
			window.draw(waveText);

			// Draw instructions
			sf::Text instructions;
			instructions.setFont(font);
			std::string instr = "B: Build Tower ($100) | Right Click: Select Tower | Middle Click: Upgrade ($150) | S: Save | ESC: Pause";
			if (waitingForNextWave && currentWaveIdx < waves.size()) {
				instr += " | SPACE: Next Wave";
			}
			instructions.setString(instr);
			instructions.setCharacterSize(16);
			instructions.setFillColor(sf::Color::White);
			instructions.setPosition(200, MAP_HEIGHT * TILE_SIZE + 10);
			window.draw(instructions);
		}

		void renderPauseMenu(sf::RenderWindow & window, sf::Font & font, int selectedOption) {
			// Tạo lớp phủ mờ
			sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
			overlay.setFillColor(sf::Color(0, 0, 0, 180));
			window.draw(overlay);

			// Tiêu đề PAUSED
			sf::Text title;
			title.setFont(font);
			title.setString("PAUSED");
			title.setCharacterSize(48);
			title.setFillColor(sf::Color::White);
			title.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 150);
			window.draw(title);

			// Tùy chọn menu
			std::vector<std::string> options = { "Continue", "Return to Main Menu" };
			for (int i = 0; i < options.size(); ++i) {
				sf::Text optionText;
				optionText.setFont(font);
				optionText.setString(options[i]);
				optionText.setCharacterSize(36);
				optionText.setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
				optionText.setPosition(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 30 + i * 60);
				window.draw(optionText);
			}
		}

		void renderGameOver() {
			sf::Text gameOverText;
			gameOverText.setFont(font);
			gameOverText.setString("GAME OVER\nPress ESC to return to menu");
			gameOverText.setCharacterSize(48);
			gameOverText.setFillColor(sf::Color::Red);
			gameOverText.setPosition(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 50);
			window.draw(gameOverText);
		}

		void run() {
			while (window.isOpen()) {
				float deltaTime = frameClock.restart().asSeconds();
				
				// Đảm bảo deltaTime không quá lớn để tránh physics glitch
				if (deltaTime > 0.25f) deltaTime = 0.25f;
				
				handleEvents();
				update(deltaTime);  // Truyền deltaTime vào update
				render();
				
				// Sleep nếu frame quá nhanh
				sf::Time frameTime = frameClock.getElapsedTime();
				if (frameTime < sf::seconds(1.0f / TARGET_FPS)) {
					sf::sleep(sf::seconds(1.0f / TARGET_FPS) - frameTime);
				}
			}
		}
	};

	int main() {
		Game game;
		game.run();
		return 0;
	}
	