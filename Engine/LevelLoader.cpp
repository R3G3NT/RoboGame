#include "LevelLoader.h"
#include "GameEngine.h"
#include "Config.h"
#include "Robot/Robot.h"
#include "Robot/IAbility.h"
#include "Robot/Abilities.h"
#include <cstdlib>
#include <ncurses.h>

std::shared_ptr<IAbility> LevelLoader::ShowAbilitySelectionMenu() {
    int choice = 0;
    const int num_abilities = 4;
    
    curs_set(0); 

    while (true) {
        erase();
        attron(A_BOLD);
        printw("=== CHOOSE YOUR STARTING ABILITY ===");
        attroff(A_BOLD);

        // Отрисовка пунктов меню
        for (int i = 0; i < num_abilities; ++i) {
            std::string name;
            int cost = 0;
            switch(i) {
                case 0: 
                    name = "Area Attack"; 
                    cost = Config::AreaAttackEnergyCost; 
                    break;
                case 1: 
                    name = "Ranged Attack"; 
                    cost = Config::RangedAttackEnergyCost; 
                    break;
                case 2: 
                    name = "Self Heal"; cost = Config::SelfHealEnergyCost; 
                    break;
                case 3: 
                    name = "Teleport"; 
                    cost = Config::TeleportEnergyCost; 
                    break;
            }

            if (i == choice) {
                attron(A_REVERSE | A_BOLD);
                mvprintw(2 + i, 0, "-> %-15s (%d EN)", name.c_str(), cost);
                attroff(A_REVERSE | A_BOLD);
            } else {
                mvprintw(2 + i, 0, "   %-15s (%d EN)", name.c_str(), cost);
            }
        }

        mvprintw(7, 0, "Use Arrow Keys / WASD to Navigate, Enter to Confirm.");
        refresh();

        int ch = getch();
        switch (ch) {
            case 'w': case 'W': case KEY_UP:
                choice = (choice - 1 + num_abilities) % num_abilities;
                break;
            case 's': case 'S': case KEY_DOWN:
                choice = (choice + 1) % num_abilities;
                break;
            case '\n':
                switch (choice) {
                    case 0: 
                        return std::make_shared<AreaAttackAbility>();
                    case 1: 
                        return std::make_shared<RangedAttackAbility>();
                    case 2: 
                        return std::make_shared<HealAbility>();
                    case 3: 
                        return std::make_shared<TeleportAbility>();
                }
                break;
        }
    }
}

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
                    std::shared_ptr<IAbility> starting_ability = LevelLoader::ShowAbilitySelectionMenu();
                    erase();
                    
                    auto player = std::make_shared<Robot>("Optimus", Robot::Type::Player,
                                        Config::PlayerHp, Config::PlayerAttack,
                                        Config::PlayerEnergy, Config::PlayerSpeed);
                    
                    player->AddAbility(starting_ability);
                    
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