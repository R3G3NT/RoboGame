#include "Engine/GameEngine.h"
#include "Engine/Visualizer.h"
#include "Engine/LevelLoader.h"
#include <memory>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(std::time(nullptr));
    Visualizer::InitScreen();

    try {
        std::vector<std::string> level = {
            "##################################################",
            "#.............###.F........#############.........#",
            "#..R..........###...............#####............#",
            "#.............########..........#####...E......~~#",
            "#.....~~~.....................................~~~#",
            "#....~~~~~....................#######.........~~~#",
            "#.....~~~.....................#..F..#.........~~~#",
            "#.............###.............#.....#.........~~~#",
            "#.............###.............##...##..........~~#",
            "#..E..........###.....E........幕........E.....####",
            "#.............###.............................####",
            "##################################################"
        };

        GameEngine engine(level[0].size(), level.size());
        LevelLoader::Load(engine, level);
        engine.Run();
    } 
    catch (const std::exception& e) {
        Visualizer::Shutdown();
        return 1;
    }

    Visualizer::Shutdown();
    return 0;
}