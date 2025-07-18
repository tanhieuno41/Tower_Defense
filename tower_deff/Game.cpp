#include "Game.h"
#include "Map.h"
#include "Constants.h"


Game::Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tower Defense Game"),
currentScreen(GameScreen::MAIN_MENU), selectedOption(0), spawnTimer(0.f),
spawnInterval(1.5f), buildingMode(false), selectedTowerIndex(-1),
mapVertices(sf::Quads), mapNeedsUpdate(true) {
	window.setFramerateLimit(TARGET_FPS);  // Giới hạn FPS

	if (!font.loadFromFile("arial.ttf")) {
		// Fallback nếu không load được font
		std::cout << "Không thể load font, sử dụng font mặc định\n";
	}

	// Khởi tạo tên map
	mapNames = {
		"Map 1: S-Shaped Path",
		"Map 2: Spiral Path",
		"Map 3: Zigzag Path",
		"Map 4: Maze Path",
		"Map 5: U-Shaped Path"
	};

	// Load tất cả map
	loadAllMaps(maps, MAX_MAPS);

	// Khởi tạo tileMap với kích thước mặc định
	tileMap.resize(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, 0));

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

void Game::setupMenu() {
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

void Game::setupUI() {
	uiBackground.setSize(sf::Vector2f(WINDOW_WIDTH, 184));  // Tăng kích thước UI background
	uiBackground.setPosition(0, MAP_HEIGHT * TILE_SIZE);
	uiBackground.setFillColor(sf::Color(50, 50, 50));

	moneyText.setFont(font);
	moneyText.setCharacterSize(32);  // Tăng kích thước chữ
	moneyText.setFillColor(sf::Color::Yellow);
	moneyText.setPosition(20, MAP_HEIGHT * TILE_SIZE + 20);

	healthText.setFont(font);
	healthText.setCharacterSize(32);  // Tăng kích thước chữ
	healthText.setFillColor(sf::Color::Red);
	healthText.setPosition(20, MAP_HEIGHT * TILE_SIZE + 70);

	waveText.setFont(font);
	waveText.setCharacterSize(32);  // Tăng kích thước chữ
	waveText.setFillColor(sf::Color::Cyan);
	waveText.setPosition(20, MAP_HEIGHT * TILE_SIZE + 120);

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

void Game::setupMapVertices() {
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

void Game::handleEvents() {
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
			case GameScreen::GAME_OVER:
				if (event.key.code == sf::Keyboard::Escape)
					currentScreen = GameScreen::MAIN_MENU;
				break;
			case GameScreen::MAP_SELECTION:
				handleMapSelectionInput(event.key.code);
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed && currentScreen == GameScreen::PLAYING) {
			handleMouseInput(event.mouseButton);
		}
	}
}

void Game::handleMenuInput(sf::Keyboard::Key key) {
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
void Game::handlePauseInput(sf::Keyboard::Key key) {
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

void Game::handleGameInput(sf::Keyboard::Key key) {
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

void Game::handleMouseInput(sf::Event::MouseButtonEvent mouse) {
	if (buildingMode && mouse.button == sf::Mouse::Left) {
		// Build tower
		sf::Vector2i pixelPos(mouse.x, mouse.y);
		sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

		int gridX = static_cast<int>(worldPos.x) / TILE_SIZE;
		int gridY = static_cast<int>(worldPos.y) / TILE_SIZE;

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
				towers.emplace_back(worldPos);   // dùng worldPos
				gameState.money -= 100;
				buildingMode = false;
			}
		}
	}
	else if (mouse.button == sf::Mouse::Right) {
		sf::Vector2f worldPos = window.mapPixelToCoords({ mouse.x, mouse.y });

		selectedTowerIndex = -1;
		for (int i = 0; i < towers.size(); ++i) {
			sf::Vector2f tp = towers[i].getPosition();
			float dx = worldPos.x - tp.x;
			float dy = worldPos.y - tp.y;
			if (std::sqrt(dx * dx + dy * dy) < 30.f) {    // √ khoảng cách
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

void Game::updateMenuColors() {
	for (int i = 0; i < menuOptions.size(); i++) {
		menuOptions[i].setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
	}
}

void Game::startNewGame() {
	resetGameState();
	currentScreen = GameScreen::MAP_SELECTION;  // Chuyển sang màn chọn map
	selectedOption = 0;
}

void Game::saveGame() {
	std::ofstream file("savegame.dat");
	if (file.is_open()) {
		file << gameState.money << " " << gameState.health << " " << gameState.wave << "\n";
		file << currentMapIndex << "\n";  // Lưu map hiện tại
		file << towers.size() << "\n";
		for (const auto& tower : towers) {
			sf::Vector2f pos = tower.getPosition();
			file << pos.x << " " << pos.y << " " << tower.getLevel() << "\n";
		}
		file.close();
		std::cout << "Game saved!\n";
	}
}

void Game::loadGame() {
	std::ifstream file("savegame.dat");
	if (file.is_open()) {
		file >> gameState.money >> gameState.health >> gameState.wave;
		file >> currentMapIndex;  // Load map đã lưu
		loadMap(currentMapIndex);  // Load map trước

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
void Game::loadMap(int mapIndex) {
	if (mapIndex >= 0 && mapIndex < maps.size()) {
		currentMapIndex = mapIndex;
		// Cập nhật kích thước map
		MAP_WIDTH = maps[mapIndex][0].size();
		MAP_HEIGHT = maps[mapIndex].size();
		WINDOW_WIDTH = MAP_WIDTH * TILE_SIZE;
		WINDOW_HEIGHT = MAP_HEIGHT * TILE_SIZE + 184;

		// Cập nhật window size
		window.setSize(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
		window.setView(sf::View(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)));
		// Cập nhật tileMap
		tileMap = maps[mapIndex];
		pathPoints.clear();         // xoá lộ trình cũ   
		// Tìm đường đi mới
		findPath();

		// Cập nhật UI
		setupUI();
		mapNeedsUpdate = true;
	}
}

void Game::renderMapSelection() {
	// Vẽ background
	sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
	background.setFillColor(sf::Color(50, 50, 50));
	window.draw(background);

	// Vẽ tiêu đề
	sf::Text title;
	title.setFont(font);
	title.setString("Select Map");
	title.setCharacterSize(48);
	title.setFillColor(sf::Color::White);
	title.setPosition(WINDOW_WIDTH / 2 - 100, 50);
	window.draw(title);

	// Vẽ danh sách map
	for (int i = 0; i < mapNames.size(); i++) {
		sf::Text mapText;
		mapText.setFont(font);
		mapText.setString(mapNames[i]);
		mapText.setCharacterSize(32);
		mapText.setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
		mapText.setPosition(WINDOW_WIDTH / 2 - 200, 150 + i * 60);
		window.draw(mapText);
	}

	// Vẽ hướng dẫn
	sf::Text instructions;
	instructions.setFont(font);
	instructions.setString("Use UP/DOWN to select map, ENTER to confirm, ESC to return");
	instructions.setCharacterSize(24);
	instructions.setFillColor(sf::Color::White);
	instructions.setPosition(WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT - 50);
	window.draw(instructions);
}

void Game::handleMapSelectionInput(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Up:
		selectedOption = (selectedOption - 1 + mapNames.size()) % mapNames.size();
		break;
	case sf::Keyboard::Down:
		selectedOption = (selectedOption + 1) % mapNames.size();
		break;
	case sf::Keyboard::Enter:
		loadMap(selectedOption);
		resetGameState();
		currentScreen = GameScreen::PLAYING;
		break;
	case sf::Keyboard::Escape:
		currentScreen = GameScreen::MAIN_MENU;
		break;
	}
}

void Game::update(float deltaTime) {
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
// Spawn enemies theo wave  (phòng path rỗng → không crash)
	if (waveInProgress && enemiesSpawned < enemiesToSpawn) {
		if (!pathPoints.empty() &&                         // ← thêm guard
			waveSpawnTimer >= waves[currentWaveIdx].spawnInterval) {

			enemies.emplace_back(pathPoints,
				waves[currentWaveIdx].color,
				100.f,
				100 + 10 * currentWaveIdx);

			enemiesSpawned++;
			waveSpawnTimer = 0.f;
		}
		else if (pathPoints.empty()) {
			// Tùy chọn: in cảnh báo để bạn biết map chưa có path hợp lệ
			std::cerr << "⚠ findPath() trả về rỗng – quái không được spawn!\n";
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
		if (!b.isActive()) continue;
		b.update(deltaTime);

		for (auto& e : enemies) {
			if (!e.isAlive()) continue;
			if (b.getBounds().intersects(e.getBounds())) {

				if (b.getType() == BulletType::SLOW)
					e.applySlow(0.5f, 2.f);        // 50 % tốc trong 2 s

				e.takeDamage(b.getDamage());

				if (b.getType() != BulletType::PIERCING)
					b.deactivate();                // piercing không tự hủy

				if (!e.isAlive()) gameState.money += 25;
				if (b.getType() != BulletType::PIERCING)
					break;                         // đạn thường dừng
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

void Game::render() {
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
	case GameScreen::MAP_SELECTION:
		renderMapSelection();
		break;
	}

	window.display();
}

void Game::renderMenu() {
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

void Game::renderGame() {
	// Draw map using vertex array
	if (mapNeedsUpdate) {
		setupMapVertices();
		mapNeedsUpdate = false;
	}
	window.draw(mapVertices);

	// Draw path
	sf::VertexArray pathVertices(sf::Quads);
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
		sf::Vector2i pixelPos = sf::Mouse::getPosition(window);   // toạ độ pixel
		sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos); // ⇐ thêm dòng này

		towerPreview.setPosition(worldPos);
		rangePreview.setPosition(worldPos);
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
	std::string instr = "B: Build Tower ($100) | Right Click: Select Tower | \n Middle Click: Upgrade ($150) | S: Save | ESC: Pause";
	if (waitingForNextWave && currentWaveIdx < waves.size()) {
		instr += " | SPACE: Next Wave";
	}
	instructions.setString(instr);
	instructions.setCharacterSize(24);  // Tăng kích thước chữ
	instructions.setFillColor(sf::Color::White);
	instructions.setPosition(400, MAP_HEIGHT * TILE_SIZE + 20);  // Điều chỉnh vị trí
	window.draw(instructions);
}

void Game::renderPauseMenu(sf::RenderWindow& window, sf::Font& font, int selectedOption) {
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

void Game::renderGameOver() {
	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setString("GAME OVER\nPress ESC to return to menu");
	gameOverText.setCharacterSize(48);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setPosition(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 50);
	window.draw(gameOverText);
}

void Game::run() {
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
// Game.cpp
void Game::resetGameState() {
	towers.clear();
	bullets.clear();
	enemies.clear();

	selectedTowerIndex = -1;
	buildingMode = false;

	currentWaveIdx = 0;
	enemiesSpawned = 0;
	enemiesToSpawn = 0;
	waveSpawnTimer = 0.f;
	waveInProgress = false;
	waitingForNextWave = true;

	gameState.money = 500;   // hoặc giá trị gốc bạn muốn
	gameState.health = 100;
	gameState.wave = 1;
}



