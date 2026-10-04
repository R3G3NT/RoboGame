#include "GameEngine.h"
#include "Visualizer.h"
#include <ncurses.h>
#include <algorithm>

GameEngine::GameEngine(int width, int height) : field(width, height) {}

void GameEngine::SpawnRobot(std::unique_ptr<Robot> robot, int x, int y) {
    if (field.IsWalkable(x, y)) {
        if (robot->GetType() == Robot::Type::Player) {
            player_ptr = robot.get();
            player_x = x;
            player_y = y;
        }

        robot->x = x;
        robot->y = y;

        field.GetCell(x, y).SetRobot(robot.get());
        robots.push_back(std::move(robot));
    }
}

void GameEngine::MoveRobot(int from_x, int from_y, int dx, int dy) {
    if (!field.GetCell(from_x, from_y).HasRobot()) return;
    Robot* moving_robot = field.GetCell(from_x, from_y).GetRobot();
    int budget = moving_robot->GetSpeed();

    int x = from_x, y = from_y;
    while (budget > 0) {
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
            if (moving_robot == player_ptr) {
                player_x = x;
                player_y = y;
            }
            moving_robot->x = x;
            moving_robot->y = y;
        } 
        else { 
            break; 
        }

        budget--;
    }

    if (field.GetCell(moving_robot->x, moving_robot->y).GetTerrain() == Terrain::Swamp) {
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
            player_ptr->ChangeEnergy(10);
            break;
        case 'q': case 'Q':
            Visualizer::Shutdown();
            exit(0);
    }

    if (dx != 0 || dy != 0) {
        MoveRobot(player_x, player_y, dx, dy);
    }
}

void GameEngine::HandleEnemiesTurn() {
    if (!player_ptr) return;

    for (int i = 0; i < robots.size(); i++) {
        if (robots[i]->GetType() == Robot::Type::Enemy) {
            int dx = 0, dy = 0;

            if (player_x > robots[i]->x) dx = 1;
            else if (player_x < robots[i]->x) dx = -1;
            else if (player_y > robots[i]->y) dy = 1;
            else if (player_y < robots[i]->y) dy = -1;

            MoveRobot(robots[i]->x, robots[i]->y, dx, dy);
        }
    }
}

void GameEngine::UpdateVisibility() {
    if (!player_ptr) return;

    int radius = 3;
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
                    field.GetCell(x, y).SetTerrain(Terrain::Wall);
                    break;
                case '~':
                    field.GetCell(x, y).SetTerrain(Terrain::Swamp);
                    break;
                case 'R':
                    SpawnRobot(
                        std::make_unique<Robot>("Optimus", Robot::Type::Player, 100, 20, 60, 2), 
                        x, y);
                    break;
                case 'F':
                    SpawnRobot(
                        std::make_unique<Robot>("Friend" + std::to_string(f), Robot::Type::Friend, 30, 5, 20, 0), 
                        x, y);
                    f++;
                    break;
                case 'E':
                    SpawnRobot(
                        std::make_unique<Robot>("Enemy" + std::to_string(e), Robot::Type::Enemy, 50, 10, 30, 1), 
                        x, y);
                    e++;
                    break;
            }
        }
    }
}

void GameEngine::Run() {
    field.ClearVisibility();
    UpdateVisibility();

    while (player_ptr != nullptr) { // Играем, пока игрок жив
        erase();
        Visualizer::Draw(field);
        printw("\n");
        Visualizer::DrawStatus(player_ptr);
        printw("\n");
        refresh();

        HandlePlayerTurn();
        CleanDeadRobots();
        HandleEnemiesTurn();
        CleanDeadRobots();

        UpdateVisibility();

        for (auto& robot : robots) {
            robot->ChangeEnergy(10);
        }
    }

    erase();
    Visualizer::Draw(field);
    printw("\n");
    printw("GAME OVER! Your robot has been destroyed. Press any key...");
    refresh();
    getch();
}
