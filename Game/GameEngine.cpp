#include <ncurses.h>
#include <algorithm>
#include "GameEngine.h"
#include "Visualizer.h"

GameEngine::GameEngine(int width, int height) : field(width, height) {}

void GameEngine::spawn_robot(std::unique_ptr<Robot> robot, int x, int y) {
    if (field.is_walkable(x, y)) {
        if (robot->get_type() == Robot::Type::Player) {
            player_ptr = robot.get();
            player_x = x;
            player_y = y;
        }

        robot->x = x;
        robot->y = y;

        field.get_cell(x, y).set_robot(robot.get());
        robots.push_back(std::move(robot));
    }
}

void GameEngine::move_robot(int fromX, int fromY, int toX, int toY) {
    if (toX < 0 || toX >= field.get_width() || toY < 0 || toY >= field.get_height()) {
        return;
    }

    Cell& current_cell = field.get_cell(fromX, fromY);
    Cell& target_cell = field.get_cell(toX, toY);

    if (!current_cell.has_robot()) return;
    Robot* moving_robot = current_cell.get_robot();

    if (target_cell.has_robot()) {
        moving_robot->interact(*target_cell.get_robot());
        return;
    }

    if (field.is_walkable(toX, toY)) {
        current_cell.remove_robot();
        target_cell.set_robot(moving_robot);

        if (moving_robot == player_ptr) {
            player_x = toX;
            player_y = toY;
        }

        moving_robot->x = toX;
        moving_robot->y = toY;
    }
}

void GameEngine::clean_dead_robots() {
    std::erase_if(robots, [this](std::unique_ptr<Robot>& robot) {
        if (robot->get_hp() <= 0) {
            field.get_cell(robot->x, robot->y).remove_robot();
            if (robot.get() == player_ptr) {
                player_ptr = nullptr;
            }
            return true;
        }
        return false;
    });
}

void GameEngine::handle_player_turn() {
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
            player_ptr->restore_energy();
            break;
        case 'q': case 'Q':
            Visualizer::shutdown();
            exit(0);
    }

    if (target_x != player_x || target_y != player_y) {
        move_robot(player_x, player_y, target_x, target_y);
    }
}

// ХОД ВРАГОВ: Простейший ИИ
void GameEngine::handle_enemies_turn() {
    // В рамках лабораторной работы: каждый враг находит игрока и делает шаг в его сторону
    if (!player_ptr) return;

    for (int y = 0; y < field.get_height(); ++y) {
        for (int x = 0; x < field.get_width(); ++x) {
            Cell& cell = field.get_cell(x, y);
            if (cell.has_robot() && cell.get_robot()->get_type() == Robot::Type::Enemy) {
                Robot* enemy = cell.get_robot();
                if (enemy->get_hp() <= 0) continue;

                int next_x = x;
                int next_y = y;

                // Движение по оси X в сторону игрока
                if (player_x > x) next_x++;
                else if (player_x < x) next_x--;
                // Движение по оси Y
                if (player_y > y) next_y++;
                else if (player_y < y) next_y--;

                move_robot(x, y, next_x, next_y);
            }
        }
    }
}

void GameEngine::run() {
    while (player_ptr != nullptr) { // Играем, пока игрок жив
        erase();
        Visualizer::draw(field);
        refresh();

        handle_player_turn();
        clean_dead_robots();
        handle_enemies_turn();
        clean_dead_robots();

        for (auto& robot : robots) {
            robot->restore_energy();
        }
    }

    Visualizer::draw(field);
    mvprintw(field.get_height() + 2, 0, "GAME OVER! Your robot has been destroyed. Press any key...");
    refresh();
    getch();
}