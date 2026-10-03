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
    int player_x = 0;
    int player_y = 0;

    void HandlePlayerTurn();
    void HandleEnemiesTurn();
    void CleanDeadRobots();

public:
    Field field;
    GameEngine(int width, int height);

    void SpawnRobot(std::unique_ptr<Robot> robot, int x, int y);
    void MoveRobot(int fromX, int fromY, int toX, int toY);
    void UpdateVisibility();
    void LoadLevel(std::vector<std::string>& level);
    void Run();
};

#endif
