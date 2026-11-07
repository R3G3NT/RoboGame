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
    std::shared_ptr<Robot> player_ptr = nullptr;
    int last_player_rank = 1;
    int current_move = 1;

    bool HandlePlayerTurn(Command cmd);
    void MovePlayer(int dx, int dy);
    void HandleEnemiesTurn();
    void CleanDeadRobots();
    void ShowMessage(const std::string& message);

public:
    Field field;

    GameEngine(int width, int height);
    ~GameEngine() = default;

    void SpawnRobot(std::shared_ptr<Robot> robot, int x, int y);
    std::pair<int, int> SelectTargetCoordinate();
    bool MoveRobot(int from_x, int from_y, int dx, int dy);
    void UpdateVisibility();

    bool Step();
    void Run();
};

#endif