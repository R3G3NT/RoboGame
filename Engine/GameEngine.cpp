#include "GameEngine.h"
#include "Visualizer.h"
#include "../Engine/Config.h"
#include <ncurses.h>
#include <algorithm>

GameEngine::GameEngine(int width, int height) : field(width, height) {}

void GameEngine::SpawnRobot(std::unique_ptr<Robot> robot, int x, int y) {
    if (field.IsWalkable(x, y)) {
        if (robot->GetType() == Robot::Type::Player) {
            player_ptr = robot.get();
        }

        robot->x = x;
        robot->y = y;

        field.GetCell(x, y).SetRobot(robot.get());
        robots.push_back(std::move(robot));
    }
}

void GameEngine::MoveRobot(int from_x, int from_y, int dx, int dy) {
    if (!field.GetCell(from_x, from_y).HasRobot()) {
        return;
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
            return;
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
        moving_robot->ChangeSpeed(-1);
    }
}

void GameEngine::CleanDeadRobots() {
    std::erase_if(robots, [this](std::unique_ptr<Robot>& robot) {
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

void GameEngine::HandlePlayerTurn() {
    int ch = getch();

    int dx = 0, dy = 0;

    switch (ch) {
        case 'w': case 'W': case KEY_UP:
            dy = -1;
            break;
        case 's': case 'S': case KEY_DOWN:
            dy = 1;
            break;
        case 'a': case 'A': case KEY_LEFT:
            dx = -1;
            break;
        case 'd': case 'D': case KEY_RIGHT:
            dx = 1;
            break;
        case '\n':
            player_ptr->ChangeEnergy(static_cast<int>(Config::EnergyPerWait));
            break;
        case 'q': case 'Q':
            Visualizer::Shutdown();
            exit(0);
    }

    int budget = player_ptr->GetSpeed();
    while ((dx != 0 || dy != 0) && budget != 0) {
        MoveRobot(player_ptr->x, player_ptr->y, dx, dy);
        budget--;
        UpdateVisibility();
        Visualizer::UpdateScreen(field, current_move, player_ptr);

        dx = dy = 0;
        if (budget != 0) {
            ch = getch();
            switch (ch) {
                case 'w': case 'W': case KEY_UP:
                    dy = -1;
                    break;
                case 's': case 'S': case KEY_DOWN:
                    dy = 1;
                    break;
                case 'a': case 'A': case KEY_LEFT:
                    dx = -1;
                    break;
                case 'd': case 'D': case KEY_RIGHT:
                    dx = 1;
            }
        }
    }
}

void GameEngine::HandleEnemiesTurn() {
    if (!player_ptr) return;

    for (int i = 0; i < robots.size(); i++) {
        if (robots[i]->GetType() == Robot::Type::Enemy) {
            int dx = 0, dy = 0;

            if (player_ptr->x > robots[i]->x) { 
                dx = 1;
            }
            else if (player_ptr->x < robots[i]->x) { 
                dx = -1;
            }
            else if (player_ptr->y > robots[i]->y) { 
                dy = 1;
            }
            else if (player_ptr->y < robots[i]->y) { 
                dy = -1;
            }

            MoveRobot(robots[i]->x, robots[i]->y, dx, dy);
        }
    }
}

void GameEngine::UpdateVisibility() {
    if (!player_ptr) return;

    int radius = static_cast<int>(Config::VisibilityRadius);
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int x = player_ptr->x + dx;
            int y = player_ptr->y + dy;

            if (x < 0 || x >= field.GetWidth() ||
                y < 0 || y >= field.GetHeight()) continue;

            field.GetCell(x, y).SetVisible(true);
        }
    }
}

void GameEngine::LoadLevel(std::vector<std::string>& level) {
    int e = 1, f = 1; // Счётчик врагов и друзей
    for (int y = 0; y < field.GetHeight(); y++) {
        for (int x = 0; x < field.GetWidth(); x++){
            char c = level[y][x];

            switch (c)
            {
                case '#':
                    field.GetCell(x, y).SetTerrain(Cell::Terrain::Wall);
                    break;
                case '~':
                    field.GetCell(x, y).SetTerrain(Cell::Terrain::Swamp);
                    break;
                case 'R':
                    SpawnRobot(
                        std::make_unique<Robot>("Optimus", Robot::Type::Player, 
                            static_cast<int>(Config::PlayerHp), 
                            static_cast<int>(Config::PlayerAttack), 
                            static_cast<int>(Config::PlayerEnergy), 
                            static_cast<int>(Config::PlayerSpeed)), 
                        x, y);
                    break;
                case 'F':
                    SpawnRobot(
                        std::make_unique<Robot>("Friend" + std::to_string(f), Robot::Type::Friend, 
                            static_cast<int>(Config::FriendHp), 
                            static_cast<int>(Config::FriendAttack), 
                            static_cast<int>(Config::FriendEnergy), 
                            static_cast<int>(Config::FriendSpeed)),  
                        x, y);
                    f++;
                    break;
                case 'E':
                    SpawnRobot(
                        std::make_unique<Robot>("Enemy" + std::to_string(e), Robot::Type::Enemy,
                            static_cast<int>(Config::EnemyHp), 
                            static_cast<int>(Config::EnemyAttack), 
                            static_cast<int>(Config::EnemyEnergy), 
                            static_cast<int>(Config::EnemySpeed)), 
                        x, y);
                    e++;
            }
        }
    }
}

void GameEngine::Run() {
    field.ClearVisibility();
    UpdateVisibility();

    while (player_ptr != nullptr) { // Играем, пока игрок жив
        Visualizer::UpdateScreen(field, current_move, player_ptr);

        HandlePlayerTurn();
        CleanDeadRobots();
        HandleEnemiesTurn();
        CleanDeadRobots();

        for (auto& robot : robots) {
            robot->ChangeEnergy(static_cast<int>(Config::EnergyPerTurn));
        }

        current_move++;
    }

    erase();
    Visualizer::Draw(field);
    printw("\n");
    printw("GAME OVER! Your robot has been destroyed. Press any key...");
    refresh();
    getch();
}
