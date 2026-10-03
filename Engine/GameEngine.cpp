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

void GameEngine::MoveRobot(int fromX, int fromY, int toX, int toY) {
    if (toX < 0 || toX >= field.GetWidth() || toY < 0 || toY >= field.GetHeight()) {
        return;
    }

    Cell& current_cell = field.GetCell(fromX, fromY);
    Cell& target_cell = field.GetCell(toX, toY);

    if (!current_cell.HasRobot()) return;
    Robot* moving_robot = current_cell.GetRobot();

    if (target_cell.HasRobot()) {
        moving_robot->Interact(target_cell.GetRobot());
        return;
    }

    if (field.IsWalkable(toX, toY)) {
        current_cell.RemoveRobot();
        target_cell.SetRobot(moving_robot);

        if (moving_robot == player_ptr) {
            player_x = toX;
            player_y = toY;
        }

        moving_robot->x = toX;
        moving_robot->y = toY;
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

    int target_x = player_x;
    int target_y = player_y;

    switch (ch) {
        case 'w': case 'W': case KEY_UP:
            target_y--;
            break;
        case 's': case 'S': case KEY_DOWN:
            target_y++;
            break;
        case 'a': case 'A': case KEY_LEFT:
            target_x--;
            break;
        case 'd': case 'D': case KEY_RIGHT:
            target_x++;
            break;
        case '\n':
            player_ptr->RestoreEnergy();
            break;
        case 'q': case 'Q':
            Visualizer::Shutdown();
            exit(0);
    }

    if (target_x != player_x || target_y != player_y) {
        MoveRobot(player_x, player_y, target_x, target_y);
    }
}

void GameEngine::HandleEnemiesTurn() {
    if (!player_ptr) return;

    for (int i = 0; i < robots.size(); i++) {
        if (robots[i]->GetType() == Robot::Type::Enemy) {
            int next_x = robots[i]->x;
            int next_y = robots[i]->y;

            if (player_x > robots[i]->x) next_x++;
            else if (player_x < robots[i]->x) next_x--;
            else if (player_y > robots[i]->y) next_y++;
            else if (player_y < robots[i]->y) next_y--;

            MoveRobot(robots[i]->x, robots[i]->y, next_x, next_y);
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
                        std::make_unique<Robot>("Optimus", Robot::Type::Player, 100, 20, 60), x, y);
                    break;
                case 'F':
                    SpawnRobot(
                        std::make_unique<Robot>("Friend" + std::to_string(f), Robot::Type::Friend, 30, 5, 20), 
                        x, y);
                    f++;
                    break;
                case 'E':
                    SpawnRobot(
                        std::make_unique<Robot>("Enemy" + std::to_string(e), Robot::Type::Enemy, 50, 10, 30), 
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
            robot->RestoreEnergy();
        }
    }

    erase();
    Visualizer::Draw(field);
    printw("\n");
    printw("GAME OVER! Your robot has been destroyed. Press any key...");
    refresh();
    getch();
}
