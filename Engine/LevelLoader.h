#ifndef LEVEL_LOADER_H_
#define LEVEL_LOADER_H_

#include <vector>
#include <string>

class GameEngine;

class LevelLoader {
public:
    static void Load(GameEngine& engine, const std::vector<std::string>& level);
};

#endif