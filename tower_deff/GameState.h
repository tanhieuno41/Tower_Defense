#pragma once

class GameState {
public:
    int money;
    int health;
    int wave;

    GameState() : money(500), health(100), wave(1) {}

    void reset() {
        money = 500;
        health = 100;
        wave = 1;
    }
};