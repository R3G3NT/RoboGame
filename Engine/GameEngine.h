#ifndef GAME_ENGINE_H_
#define GAME_ENGINE_H_

#include "../Field/Field.h"
#include "../Robot/Robot.h"
#include <vector>
#include <memory>

class GameEngine {
private:
    std::vector<std::unique_ptr<Robot>> robots;
    Robot* player_ptr = nullptr;
    int current_move = 1; // Количество ходов

    void HandlePlayerTurn();
    void HandleEnemiesTurn();
    void CleanDeadRobots();

public:
    Field field;
    GameEngine(int width, int height);

    void SpawnRobot(std::unique_ptr<Robot> robot, int x, int y);
    bool MoveRobot(int from_x, int from_y, int dx, int dy);
    void UpdateVisibility();
    void LoadLevel(std::vector<std::string>& level);
    void Run();
};

#endif
