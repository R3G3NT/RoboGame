#include "GameEngine.h"
#include "Visualizer.h"
#include "InputHandler.h"
#include "Config.h"
#include "Robot/IAbility.h"
#include "Robot/Abilities.h"
#include <ncurses.h> 

GameEngine::GameEngine(int width, int height) : field(width, height) {}

void GameEngine::SpawnRobot(std::shared_ptr<Robot> robot, int x, int y) {
    if (field.IsWalkable(x, y)) {
        if (robot->GetType() == Robot::Type::Player) {
            player_ptr = robot.get();
        }
        robot->x = x;
        robot->y = y;
        field.GetCell(x, y).SetRobot(robot.get());
        robots.push_back(robot);
    }
}

bool GameEngine::MoveRobot(int from_x, int from_y, int dx, int dy) {
    if (!field.GetCell(from_x, from_y).HasRobot()) {
        return false;
    }

    Robot* moving_robot = field.GetCell(from_x, from_y).GetRobot();
    int speed = moving_robot->GetSpeed();
    if (moving_robot == player_ptr) {
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
            if (robot.get() == player_ptr) {
                player_ptr = nullptr;
            }
            return true;
        }
        return false;
    });
}

void GameEngine::HandleEnemiesTurn() {
    if (!player_ptr) return;

    for (int i = 0; i < robots.size(); i++) {
        if (!player_ptr) return;

        if (robots[i]->GetType() == Robot::Type::Enemy) {
            int dx = 0, dy = 0;
            if (player_ptr->x > robots[i]->x)      dx = 1;
            else if (player_ptr->x < robots[i]->x) dx = -1;
            else if (player_ptr->y > robots[i]->y) dy = 1;
            else if (player_ptr->y < robots[i]->y) dy = -1;

            MoveRobot(robots[i]->x, robots[i]->y, dx, dy);
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

void GameEngine::HandlePlayerTurn(Command cmd) {
    if (!player_ptr) return;

    switch (cmd.type) {
        case Command::Type::Quit:
            Visualizer::Shutdown();
            exit(0);
        case Command::Type::Wait:
            player_ptr->ChangeEnergy(Config::EnergyPerWait);
            return;
        case Command::Type::Move:
            MovePlayer(cmd.dx, cmd.dy);
            return;
            
        case Command::Type::UseAbility: {
            const auto& abilities = player_ptr->GetAbilities();
            
            if (cmd.ability_index >= abilities.size()) {
                return; 
            }
            
            std::shared_ptr<IAbility> chosen_ability = abilities[cmd.ability_index];
            std::string name = chosen_ability->GetName();
            
            int tx = 0, ty = 0;
            // Проверяем, требует ли способность интерактивного прицеливания
            if (name == "Ranged Attack" || name == "Teleport") {
                auto [target_x, target_y] = SelectTargetCoordinate();
                if (target_x == -1 && target_y == -1) {
                    return;
                }
                tx = target_x;
                ty = target_y;
            }
            
            bool success = player_ptr->UseAbility(cmd.ability_index, field, tx, ty);
            
            if (!success) {
                mvprintw(field.GetHeight() + 1, 0, "[!] Action failed: check energy or target validation.");
                getch();
            }
            
            field.ClearVisibility();
            UpdateVisibility();
            Visualizer::UpdateScreen(field, current_move, player_ptr);
            return;
        }
    }
}

void GameEngine::MovePlayer(int dx, int dy) {
    while ((dx != 0 || dy != 0) && player_ptr->GetSpeed() != 0) {
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

void GameEngine::ShowMessage(const std::string& message) {
    erase();
    Visualizer::Draw(field);
    printw("%s\n", message.c_str());
    refresh();
    getch();
}

bool GameEngine::Step(Command cmd) {
    HandlePlayerTurn(cmd);
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
#include <ncurses.h> 
    ++current_move;
    field.ClearVisibility();
    UpdateVisibility();
    Visualizer::UpdateScreen(field, current_move, player_ptr);
    return true;
}

void GameEngine::Run() {
    UpdateVisibility();
    Visualizer::UpdateScreen(field, current_move, player_ptr);

    while (Step(InputHandler::ReadFromKeyboard())) { }
}