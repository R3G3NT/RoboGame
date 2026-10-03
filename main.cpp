#include "Engine/GameEngine.h"
#include "Engine/Visualizer.h"
#include <ncurses.h>
#include <memory>

int main() {
    Visualizer::InitScreen();

    try {
        GameEngine engine(20, 12);

        auto player = std::make_unique<Robot>("Optimus", Robot::Type::Player, 100, 20, 60);
        engine.SpawnRobot(std::move(player), 2, 1);

        auto enemy = std::make_unique<Robot>("Decepticon", Robot::Type::Enemy, 10000, 1, 1);
        engine.SpawnRobot(std::move(enemy), 10, 5);

        //auto friend_bot = std::make_unique<Robot>("Бамблби", Robot::Type::Friend, 50, 10, 40);
        //engine.SpawnRobot(std::move(friend_bot), 4, 3);

        engine.Run();

    } catch (const std::exception& e) {
        Visualizer::Shutdown();
        return 1;
    }

    Visualizer::Shutdown();
    return 0;
}
