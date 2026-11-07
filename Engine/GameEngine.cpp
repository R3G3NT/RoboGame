#include "GameEngine.h"
#include "Visualizer.h"
#include "InputHandler.h"
#include "Config.h"
#include "LevelLoader.h"
#include "Robot/IAbility.h"
#include "Robot/Abilities.h"
#include <ncurses.h> 

GameEngine::GameEngine(int width, int height) : field(width, height) {}

void GameEngine::SpawnRobot(std::shared_ptr<Robot> robot, int x, int y) {
    if (field.IsWalkable(x, y)) {
        if (robot->GetType() == Robot::Type::Player) {
            player_ptr = robot;
        }
        robot->x = x;
        robot->y = y;
        field.GetCell(x, y).SetRobot(robot.get());
        robots.push_back(robot);
    }
}

/**
 * @brief Перемещает робота по игровому полю в заданном направлении с учетом его скорости.
 * @return true — если робот наткнулся на другого робота и произошло взаимодействие (Interact).
 * @return false — если робот просто успешно прошел весь маршрут или уперся в стену/границу поля.
 */
bool GameEngine::MoveRobot(int from_x, int from_y, int dx, int dy) {
    if (!field.GetCell(from_x, from_y).HasRobot()) {
        return false;
    }

    Robot* moving_robot = field.GetCell(from_x, from_y).GetRobot();
    int speed = moving_robot->GetSpeed();
    if (moving_robot == player_ptr.get()) {
        speed = 1;
    }
    int x = from_x, y = from_y;
    while (speed != 0) {
        x += dx;
        y += dy;

        if (x < 0 || x >= field.GetWidth() ||
            y < 0 || y >= field.GetHeight()) break;

        Cell& target_cell = field.GetCell(x, y);
        if (target_cell.HasRobot()) {
            moving_robot->Interact(target_cell.GetRobot());
            return true;
        }

        if (field.IsWalkable(x, y)) {
            field.GetCell(from_x, from_y).RemoveRobot();
            target_cell.SetRobot(moving_robot);
            from_x = x;
            from_y = y;
            moving_robot->x = x;
            moving_robot->y = y;

            if (target_cell.GetCellStatuses().size() > 0) {
                target_cell.TransferRandomStatusToRobot(moving_robot);
            }
        }
        else {
            break;
        }
        speed--;
    }

    if (field.GetCell(moving_robot->x, moving_robot->y).GetTerrain() == Cell::Terrain::Swamp) {
        moving_robot->SetMaxSpeed(1);
    }

    return false;
}

void GameEngine::CleanDeadRobots() {
    std::erase_if(robots, [this](std::shared_ptr<Robot>& robot) {
        if (robot->GetHp() <= 0) {
            field.GetCell(robot->x, robot->y).RemoveRobot();
            if (robot == player_ptr) {
                player_ptr = nullptr;
            }
            return true;
        }
        return false;
    });
}

void GameEngine::HandleEnemiesTurn() {
    if (!player_ptr) return;

    for (size_t i = 0; i < robots.size(); i++) {
        if (!player_ptr) return;

        if (robots[i]->GetType() == Robot::Type::Enemy) {
            Robot* enemy = robots[i].get();
            
            enemy->UpdateStatusesAtTurnStart();
            if (enemy->GetHp() <= 0) {
                continue; 
            }

            int dx = 0, dy = 0;
            if (player_ptr->x > enemy->x)      dx = 1;
            else if (player_ptr->x < enemy->x) dx = -1;
            else if (player_ptr->y > enemy->y) dy = 1;
            else if (player_ptr->y < enemy->y) dy = -1;

            const auto& enemy_abilities = enemy->GetAbilities();
            
            if (enemy_abilities.empty()) {
                if (enemy->CanMove()) {
                    MoveRobot(robots[i]->x, robots[i]->y, dx, dy);
                }
                enemy->UpdateStatusesAtTurnEnd();
                continue;
            }

            std::string ability_name = enemy_abilities[0]->GetName();

            if (ability_name == "Self Heal" && enemy->GetHp() <= (enemy->GetMaxHp() / 4)) {
                if (enemy->CanUseAbilities()) {
                    bool used_heal = enemy->UseAbility(0, field);
                    if (used_heal) {
                        enemy->UpdateStatusesAtTurnEnd();
                        continue; 
                    }
                }
            }

            int next_x = enemy->x + dx;
            int next_y = enemy->y + dy;
            bool path_blocked = false;
            if (next_x >= 0 && next_x < field.GetWidth() && next_y >= 0 && next_y < field.GetHeight()) {
                Cell& next_cell = field.GetCell(next_x, next_y);
                if (next_cell.GetTerrain() == Cell::Terrain::Wall || next_cell.HasRobot()) {
                    path_blocked = true;
                }
            }

            if (ability_name == "Teleport" && path_blocked) {
                if (enemy->CanUseAbilities()) {
                    int jump_x = enemy->x + (dx * 2);
                    int jump_y = enemy->y + (dy * 2);

                    if (jump_x >= 0 && jump_x < field.GetWidth() && jump_y >= 0 && jump_y < field.GetHeight()) {
                        Cell& jump_cell = field.GetCell(jump_x, jump_y);
                        if (jump_cell.GetTerrain() != Cell::Terrain::Wall && !jump_cell.HasRobot()) {
                            bool jumped = enemy->UseAbility(0, field, jump_x, jump_y);
                            if (jumped) {
                                enemy->UpdateStatusesAtTurnEnd();
                                continue; 
                            }
                        }
                    }
                }
            }

            if (enemy->CanMove()) {
                MoveRobot(robots[i]->x, robots[i]->y, dx, dy);
            }
            
            enemy->UpdateStatusesAtTurnEnd();
        }
    }
}

void GameEngine::UpdateVisibility() {
    if (!player_ptr) return;

    int radius = Config::VisibilityRadius;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius - 3; dx <= radius + 3; dx++) {
            int x = player_ptr->x + dx;
            int y = player_ptr->y + dy;

            if (x < 0 || x >= field.GetWidth() ||
                y < 0 || y >= field.GetHeight()) continue;
                
            field.GetCell(x, y).SetVisible(true);
        }
    }
}

std::pair<int, int> GameEngine::SelectTargetCoordinate() {
    int target_x = player_ptr->x;
    int target_y = player_ptr->y;
    
    curs_set(1); 
    
    while (true) {
        move(target_y, target_x); 
        refresh();
        
        Command input = InputHandler::ReadFromKeyboard();
        
        if (input.type == Command::Type::Move) {
            int next_x = target_x + input.dx;
            int next_y = target_y + input.dy;
            
            if (next_x >= 0 && next_x < field.GetWidth() &&
                next_y >= 0 && next_y < field.GetHeight()) {
                target_x = next_x;
                target_y = next_y;
            }
        }
        else if (input.type == Command::Type::Wait) {
            curs_set(0);
            return {target_x, target_y};
        }
        else if (input.type == Command::Type::Quit) {
            curs_set(0);
            return {-1, -1};
        }
    }
}

void GameEngine::MovePlayer(int dx, int dy) {
    while ((dx != 0 || dy != 0) && player_ptr->GetSpeed() != 0) {
        if (!player_ptr->CanMove()) {
            mvprintw(field.GetHeight() + 1, 0, "[!] Action failed: Robot is Slowed!");
            getch();
            Visualizer::UpdateScreen(field, current_move, player_ptr);
            break;
        }

        bool was_interacted = MoveRobot(player_ptr->x, player_ptr->y, dx, dy);
        player_ptr->ChangeSpeed(-1);

        field.ClearVisibility();
        UpdateVisibility();
        Visualizer::UpdateScreen(field, current_move, player_ptr);

        dx = dy = 0;
        if (player_ptr->GetSpeed() != 0 && !was_interacted) {
            Command next = InputHandler::ReadFromKeyboard();
            if (next.type == Command::Type::Quit) {
                Visualizer::Shutdown();
                exit(0);
            }
            else if (next.type == Command::Type::Move) {
                dx = next.dx;
                dy = next.dy;
            }
        }
    }

    while (player_ptr->GetSpeed() < player_ptr->GetMaxSpeed()) {
        player_ptr->ChangeSpeed(1);
    }
}

/**
 * @brief Обрабатывает одиночную команду игрока в рамках его комплексного хода.
 * @return true — если игрок совершил глобальное действие, ЗАВЕРШАЮЩЕЕ его ход (шаг или пропуск хода).
 * @return false — если игрок применил способность (ход продолжается, он может скастовать что-то еще).
 */
bool GameEngine::HandlePlayerTurn(Command cmd) {
    if (!player_ptr) return true;

    switch (cmd.type) {
        case Command::Type::Quit:
            Visualizer::Shutdown();
            exit(0);
        case Command::Type::Wait:
            player_ptr->ChangeEnergy(Config::EnergyPerWait);
            return true; 
        case Command::Type::Move:
            MovePlayer(cmd.dx, cmd.dy);
            return true; 
            
        case Command::Type::UseAbility: {
            if (!player_ptr->CanUseAbilities()) {
                mvprintw(field.GetHeight() + 1, 0, "[!] Action failed: Robot is OVERLOADED!");
                getch();
                Visualizer::UpdateScreen(field, current_move, player_ptr);
                return true;
            }

            const auto& abilities = player_ptr->GetAbilities();
            
            if (cmd.ability_index >= abilities.size()) {
                return false; 
            }
            
            std::shared_ptr<IAbility> chosen_ability = abilities[cmd.ability_index];
            std::string name = chosen_ability->GetName();
            
            int tx = 0, ty = 0;
            if (name == "Ranged Attack" || name == "Teleport") {
                auto [target_x, target_y] = SelectTargetCoordinate();
                if (target_x == -1 && target_y == -1) {
                    return false;
                }
                tx = target_x;
                ty = target_y;
            }
            
            bool success = player_ptr->UseAbility(cmd.ability_index, field, tx, ty);
            CleanDeadRobots();
            
            if (!success) {
                mvprintw(field.GetHeight() + 1, 0, "[!] Action failed: check energy or target validation.");
                getch();
            }
            
            field.ClearVisibility();
            UpdateVisibility();
            Visualizer::UpdateScreen(field, current_move, player_ptr);
            return false; 
        }
        case Command::Type::ToggleMode: {
            switch (cmd.mode_index) {
                case 1:
                    Visualizer::SetRenderMode(Visualizer::RenderMode::Normal);
                    break;
                case 2:
                    Visualizer::SetRenderMode(Visualizer::RenderMode::HighlightEnemies);
                    break;
                case 3:
                    Visualizer::SetRenderMode(Visualizer::RenderMode::HighlightWalls);
                    break;
                case 4:
                    Visualizer::SetRenderMode(Visualizer::RenderMode::HighlightTraps);
                    break;
            }

            field.ClearVisibility();
            UpdateVisibility();
            Visualizer::UpdateScreen(field, current_move, player_ptr);
            return false;
        }
        default:
            return false;
    }
}

void GameEngine::ShowMessage(const std::string& message) {
    erase();
    Visualizer::Draw(field);
    printw("%s\n", message.c_str());
    refresh();
    getch();
}

/**
 * @brief Выполняет ход игры (ход ИИ врагов, очистка погибших, проверка победы/проигрыша).
 * @return true — игра продолжается,
 * @return false — игра завершена (игрок погиб или все враги уничтожены).
 */
bool GameEngine::Step() {
    CleanDeadRobots();
    HandleEnemiesTurn();
    CleanDeadRobots();

    if (!player_ptr) {
        ShowMessage("\nGAME OVER! Your robot has been destroyed :(\nPress any key...");
        return false;
    }

    bool no_enemy = true;
    for (const auto& r : robots) {
        if (r->GetType() == Robot::Type::Enemy) {
            no_enemy = false;
            break;
        }
    }
    if (no_enemy) {
        ShowMessage("\nYou destroyed all the enemies! You WON :)\nPress any key...\n");
        return false;
    }

    for (auto& r : robots) {
        r->ChangeEnergy(Config::EnergyPerTurn);
    }

    ++current_move;
    field.ClearVisibility();
    UpdateVisibility();
    Visualizer::UpdateScreen(field, current_move, player_ptr);
    return true;
}

void GameEngine::Run() {
    UpdateVisibility();
    Visualizer::UpdateScreen(field, current_move, player_ptr);

    if (player_ptr) {
        last_player_rank = player_ptr->GetRank();
    }

    while (true) {
        if (player_ptr) {
            player_ptr->UpdateStatusesAtTurnStart();
            
            if (player_ptr->GetHp() <= 0) {
                if (!Step()) break; 
                continue;
            }
        }

        bool player_turn_ended = false;

        while (!player_turn_ended) {
            const auto& abilities = player_ptr->GetAbilities();
            bool can_use_any_ability = false;
            
            for (const auto& ability : abilities) {
                if (player_ptr->GetEnergy() >= ability->GetEnergyCost()) {
                    can_use_any_ability = true;
                    break;
                }
            }

            if (!can_use_any_ability) {
                mvprintw(field.GetHeight() + 1, 0, "[!] No energy for abilities! You must Move or Wait.");
                refresh();
            }

            Command cmd = InputHandler::ReadFromKeyboard();

            if (!can_use_any_ability && cmd.type == Command::Type::UseAbility) {
                continue; 
            }

            player_turn_ended = HandlePlayerTurn(cmd);
        }

        if (player_ptr) {
            player_ptr->UpdateStatusesAtTurnEnd();
        }
            
        if (player_ptr && player_ptr->GetRank() != last_player_rank) {
            Visualizer::ShowUpgradeMenu(player_ptr);
            
            last_player_rank = player_ptr->GetRank();
            
            field.ClearVisibility();
            UpdateVisibility();
            Visualizer::UpdateScreen(field, current_move, player_ptr);
        }

        if (!Step()) {
            break;
        }
    }
}