#include "LevelLoader.h"
#include "Engine/GameEngine.h"
#include "Engine/Config.h"
#include "Robot/Robot.h"
#include "Robot/IAbility.h"
#include "Robot/Abilities.h"

void LevelLoader::Load(GameEngine& engine, const std::vector<std::string>& level) {
    int e = 1, f = 1;
    for (int y = 0; y < engine.field.GetHeight(); y++) {
        for (int x = 0; x < engine.field.GetWidth(); x++) {
            char c = level[y][x];
            switch (c) {
                case '#':
                    engine.field.GetCell(x, y).SetTerrain(Cell::Terrain::Wall);
                    break;
                case '~':
                    engine.field.GetCell(x, y).SetTerrain(Cell::Terrain::Swamp);
                    break;
                case 'R': {
                    auto player = std::make_shared<Robot>("Optimus", Robot::Type::Player,
                                        Config::PlayerHp, Config::PlayerAttack,
                                        Config::PlayerEnergy, Config::PlayerSpeed);
                    
                    player->AddAbility(std::make_shared<AreaAttackAbility>());
                    player->AddAbility(std::make_shared<RangedAttackAbility>());
                    player->AddAbility(std::make_shared<HealAbility>());
                    player->AddAbility(std::make_shared<TeleportAbility>());
                    
                    engine.SpawnRobot(player, x, y);
                    break;
                }
                case 'F': {
                    auto friend_robot = std::make_shared<Robot>("Friend" + std::to_string(f), Robot::Type::Friend,
                                        Config::FriendHp, Config::FriendAttack,
                                        Config::FriendEnergy, Config::FriendSpeed);
                    
                    engine.SpawnRobot(friend_robot, x, y);
                    f++;
                    break;
                }
                case 'E': {
                    auto enemy = std::make_shared<Robot>("Enemy" + std::to_string(e), Robot::Type::Enemy,
                                        Config::EnemyHp, Config::EnemyAttack,
                                        Config::EnemyEnergy, Config::EnemySpeed);
                    
                    // Вражеские роботы получают случайно способность Лечение или Телепортация
                    if (std::rand() % 2 == 0) {
                        enemy->AddAbility(std::make_shared<HealAbility>());
                    } else {
                        enemy->AddAbility(std::make_shared<TeleportAbility>());
                    }
                    
                    engine.SpawnRobot(enemy, x, y);
                    e++;
                    break;
                }
                default:
                    break;
            }
        }
    }
}