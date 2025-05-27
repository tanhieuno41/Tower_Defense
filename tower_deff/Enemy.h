#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath> // để dùng sqrt()

// Biến toàn cục bạn cần khai báo ở đâu đó (ví dụ trong main.cpp)
extern std::vector<sf::Vector2i> pathPoints;
extern const int TILE_SIZE;

class Enemy {
public:
    sf::CircleShape shape;
    float speed = 100.f;
    int currentPoint = 0;
    sf::Vector2f position;
    void draw(sf::RenderWindow& window) const {
        window.draw(shape);
    }
    Enemy() {
        // Khởi tạo vị trí dựa trên điểm đầu tiên trong đường đi
        position = sf::Vector2f(pathPoints[0].x * TILE_SIZE, pathPoints[0].y * TILE_SIZE);
        shape.setRadius(20.f);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(position);
    }

    void update(float deltaTime) {
        if (currentPoint + 1 >= pathPoints.size()) return;

        sf::Vector2f target(pathPoints[currentPoint + 1].x * TILE_SIZE,
            pathPoints[currentPoint + 1].y * TILE_SIZE);

        sf::Vector2f direction = target - position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance != 0) direction /= distance;

        position += direction * speed * deltaTime;

        // Nếu đã đến gần điểm tiếp theo thì chuyển sang điểm kế tiếp
        if (distance < 5.f)
            currentPoint++;

        shape.setPosition(position);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};
