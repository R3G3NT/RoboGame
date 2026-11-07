#ifndef LEVEL_LOADER_H_
#define LEVEL_LOADER_H_

#include <vector>
#include <string>
#include <memory>

class GameEngine;
class Robot;
class IAbility;

class LevelLoader {
private:
    static std::shared_ptr<IAbility> ShowAbilitySelectionMenu();

public:
    static void Load(GameEngine& engine, const std::vector<std::string>& level);
    static void ShowUpgradeMenu(std::shared_ptr<Robot> player);
};

#endif