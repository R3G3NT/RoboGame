#include <ncurses.h>
#include "Visualizer.h"
#include "../Field/Field.h"
#include "../Robot/Robot.h"

void Visualizer::init_screen() {
    initscr();             // Врубаем ncurses
    cbreak();              // Отключаем буферизацию строк
    noecho();              // Не выводим нажатые клавиши на экране
    keypad(stdscr, TRUE);  // Разрешаем стрелочки и F-клавиши
    curs_set(0);           // Прячем мигающий курсор
    clear();               // Чистим экран
}

void Visualizer::shutdown() {
    endwin(); // Восстанавливаем стандартные настройки терминала
}

void Visualizer::draw(Field& field) {
    for (int y = 0; y < field.get_height(); y++) {
        for (int x = 0; x < field.get_width(); x++) {
            Cell& cell = field.get_cell(x, y);

            if (!cell.is_walkable() && !cell.has_robot()) {
                printw("#");
            } else if (cell.has_robot()) {
                Robot* robot = cell.get_robot();
                if (robot->get_type() == Robot::Type::Player) {
                    printw("P");
                } else if (robot->get_type() == Robot::Type::Friend) {
                    printw("F");
                } else if (robot->get_type() == Robot::Type::Enemy) {
                    printw("E");
                }
            } else {
                printw(".");
            }
        }
        printw("\n");
    }
    refresh();
}