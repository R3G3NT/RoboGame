#include "Engine/GameEngine.h"
#include "Engine/Visualizer.h"
#include <ncurses.h>
#include <memory>

void LoadLevel(GameEngine& engine, std::vector<std::string>& level) {
    int e = 1, f = 1; // Счётчик врагов и друзей
    for (int y = 0; y < engine.field.GetHeight(); y++) {
        for (int x = 0; x < engine.field.GetWidth(); x++){
            char c = level[y][x];

            if (c == '#') {
                engine.field.GetCell(x, y).SetWalkable(false);
                continue;
            }

            if (c == 'R') {
                engine.SpawnRobot(
                    std::make_unique<Robot>("Optimus", Robot::Type::Player, 100, 20, 60), x, y);
            }
            else if (c == 'F') {
                engine.SpawnRobot(
                    std::make_unique<Robot>("Friend" + std::to_string(f), Robot::Type::Friend, 30, 5, 20), 
                    x, y);
                f++;
            }
            else if (c == 'E') {
                engine.SpawnRobot(
                    std::make_unique<Robot>("Enemy" + std::to_string(e), Robot::Type::Enemy, 50, 10, 30), 
                    x, y);
                e++;
            }
        }
    }
}

int main() {
    Visualizer::InitScreen();

    try {
        std::vector<std::string> level = {
            "##################################################",
            "#.............###..........##########............#",
            "#..R..........###..F............#####.......E....#",
            "#.............###...............#####............#",
            "#................................................#",
            "#.............................#######............#",
            "#.............................#..F..#...E........#",
            "#....E........###.............#.....#.........####",
            "#.............###.............##...##.........####",
            "#.............###....E........................####",
            "#.............###........................#########",
            "##################################################"
        };

        GameEngine engine(level[0].size(), level.size());
        LoadLevel(engine, level);
        engine.Run();

    } catch (const std::exception& e) {
        Visualizer::Shutdown();
        return 1;
    }

    Visualizer::Shutdown();
    return 0;
}
