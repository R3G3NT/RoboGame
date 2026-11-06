#ifndef GAME_ENGINE_H_
#define GAME_ENGINE_H_

#include "Field/Field.h"
#include "Robot/Robot.h"
#include "Engine/Command.h"
#include <vector>
#include <memory>

class GameEngine {
private:
    std::vector<std::shared_ptr<Robot>> robots;
    Robot* player_ptr = nullptr;
    int current_move = 1;

    void HandlePlayerTurn(Command cmd);
    void MovePlayer(int dx, int dy);
    void HandleEnemiesTurn();
    void CleanDeadRobots();

public:
    Field field;

    GameEngine(int width, int height);
    ~GameEngine() = default;

    void SpawnRobot(std::shared_ptr<Robot> robot, int x, int y);
    bool MoveRobot(int from_x, int from_y, int dx, int dy);
    void UpdateVisibility();
    void LoadLevel(const std::vector<std::string>& level);

    bool Step(Command cmd);

    void Run();
};

#endif