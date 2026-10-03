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
    Robot* moving_robot = current_cell.get_robot();

    if (target_cell.HasRobot()) {
        moving_robot->Interact(*target_cell.get_robot());
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
        case KEY_ENTER:
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

// ХОД ВРАГОВ: Простейший ИИ
void GameEngine::HandleEnemiesTurn() {
    // В рамках лабораторной работы: каждый враг находит игрока и делает шаг в его сторону
    if (!player_ptr) return;

    for (int y = 0; y < field.GetHeight(); ++y) {
        for (int x = 0; x < field.GetWidth(); ++x) {
            Cell& cell = field.GetCell(x, y);
            if (cell.HasRobot() && cell.get_robot()->GetType() == Robot::Type::Enemy) {
                Robot* enemy = cell.get_robot();
                if (enemy->GetHp() <= 0) continue;

                int next_x = x;
                int next_y = y;

                // Движение по оси X в сторону игрока
                if (player_x > x) next_x++;
                else if (player_x < x) next_x--;
                // Движение по оси Y
                if (player_y > y) next_y++;
                else if (player_y < y) next_y--;

                MoveRobot(x, y, next_x, next_y);
            }
        }
    }
}

void GameEngine::Run() {
    while (player_ptr != nullptr) { // Играем, пока игрок жив
        erase();
        Visualizer::Draw(field);
        refresh();

        HandlePlayerTurn();
        CleanDeadRobots();
        HandleEnemiesTurn();
        CleanDeadRobots();

        for (auto& robot : robots) {
            robot->RestoreEnergy();
        }
    }

    Visualizer::Draw(field);
    mvprintw(field.GetHeight() + 2, 0, "GAME OVER! Your robot has been destroyed. Press any key...");
    refresh();
    getch();
}
