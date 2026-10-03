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

        auto enemy = std::make_unique<Robot>("Decepticon", Robot::Type::Enemy, 1, 15, 20);
        engine.SpawnRobot(std::move(enemy), 4, 1);

        engine.Run();

    } catch (const std::exception& e) {
        Visualizer::Shutdown();
        return 1;
    }

    Visualizer::Shutdown();
    return 0;
}
