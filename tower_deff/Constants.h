#pragma once
// Hằng & biến toàn cục
extern const int TILE_SIZE;
extern int MAP_WIDTH;
extern int MAP_HEIGHT;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
enum class BulletType {
    NORMAL,     // sát thương chuẩn
    PIERCING,   // xuyên qua nạn nhân, bắn tiếp
    SLOW        // gây ít damage nhưng giảm speed
};