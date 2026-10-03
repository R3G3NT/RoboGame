#pragma once

#include <vector>
#include <memory>
#include "../Field/Field.h"
#include "../Robot/Robot.h"

class GameEngine {
private:
    Field field;
    std::vector<std::unique_ptr<Robot>> robots;

    Robot* player_ptr = nullptr;
    int player_x = 0;
    int player_y = 0;

    void handle_player_turn();
    void handle_enemies_turn();
    void clean_dead_robots();

public:
    GameEngine(int width, int height);

    void spawn_robot(std::unique_ptr<Robot> robot, int x, int y);
    void move_robot(int fromX, int fromY, int toX, int toY);
    void run();
};
