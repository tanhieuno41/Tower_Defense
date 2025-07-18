#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include "Constants.h"
#include "GameScreen.h"
#include "Map.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Tower.h"
#include "GameState.h"

// -------------------- LỚP GAME (KHAI BÁO) --------------------
class Game {
public:
    // ---------- thuộc tính ----------
    sf::RenderWindow window;
    GameScreen currentScreen;
    sf::Font font;

    GameState gameState;
    std::vector<Enemy> enemies;
    std::queue<std::pair<int, sf::Color>> enemyQueue;
    std::vector<Tower> towers;
    std::vector<Bullet> bullets;

    sf::Clock      clock;
    float          spawnTimer;
    float          spawnInterval;

    std::vector<sf::Text> menuOptions;
    int  selectedOption;

    sf::Text            moneyText;
    sf::Text            healthText;
    sf::Text            waveText;
    sf::RectangleShape  uiBackground;

    bool          buildingMode;
    sf::CircleShape towerPreview;
    sf::CircleShape rangePreview;
    int           selectedTowerIndex;

    std::vector<WaveInfo> waves;
    int   currentWaveIdx;
    int   enemiesSpawned;
    int   enemiesToSpawn;
    float waveSpawnTimer;
    bool  waveInProgress;
    bool  waitingForNextWave;

    static const int   TARGET_FPS = 60;
    static const float TARGET_FRAME_TIME;
    sf::Clock frameClock;

    sf::VertexArray mapVertices;
    bool            mapNeedsUpdate;

    static const int MAX_MAPS = 5;
    int currentMapIndex;
    std::vector<std::vector<std::vector<int>>> maps;
    std::vector<std::string> mapNames;

    // ---------- hàm khởi tạo / vòng đời ----------
    Game();
    void run();

    // ---------- xử lý sự kiện ----------
    void handleEvents();
    void handleMenuInput(sf::Keyboard::Key key);
    void handlePauseInput(sf::Keyboard::Key key);
    void handleGameInput(sf::Keyboard::Key key);
    void handleMouseInput(sf::Event::MouseButtonEvent mouse);
    void handleMapSelectionInput(sf::Keyboard::Key key);

    // ---------- cập-nhật và vẽ ----------
    void update(float deltaTime);
    void render();
    void renderMenu();
    void renderGame();
    void renderPauseMenu(sf::RenderWindow& win, sf::Font& f, int selOpt);
    void renderGameOver();
    void renderMapSelection();

    // ---------- thiết lập ----------
    void setupMenu();
    void setupUI();
    void setupMapVertices();

    // ---------- quản lý trò chơi ----------
    void startNewGame();
    void saveGame();
    void loadGame();
    void loadMap(int mapIndex);
    void resetGameState();


private:            // (nếu muốn ẩn chi tiết)
    void updateMenuColors();
};
