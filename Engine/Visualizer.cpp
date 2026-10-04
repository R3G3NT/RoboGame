#include "Visualizer.h"
#include "../Field/Field.h"
#include "../Robot/Robot.h"
#include <ncurses.h>

void Visualizer::InitScreen() {
    initscr();             // Врубаем ncurses
    cbreak();              // Отключаем буферизацию строк
    noecho();              // Не выводим нажатые клавиши на экране
    keypad(stdscr, TRUE);  // Разрешаем стрелочки и F-клавиши
    curs_set(0);           // Прячем мигающий курсор
    clear();               // Чистим экран
}

void Visualizer::Shutdown() {
    endwin(); // Восстанавливаем стандартные настройки терминала
}

void Visualizer::Draw(Field& field) {
    for (int y = 0; y < field.GetHeight(); y++) {
        for (int x = 0; x < field.GetWidth(); x++) {
            Cell& cell = field.GetCell(x, y);

            if (!cell.IsVisible()) {
                printw(".");
            } 
            else if (cell.GetTerrain() == Terrain::Wall) {
                printw("#");
            } 
            else if (cell.HasRobot()) {
                Robot* robot = cell.GetRobot();
                
                if (robot->GetType() == Robot::Type::Player) {
                    printw("R");
                } 
                else if (robot->GetType() == Robot::Type::Friend) {
                    printw("F");
                } 
                else if (robot->GetType() == Robot::Type::Enemy) {
                    printw("E");
                }
            } 
            else if (cell.GetTerrain() == Terrain::Swamp) {
                printw("~");
            } 
            else {
                printw(" ");
            }
        }
        printw("\n");
    }
}

void Visualizer::DrawStatus(Robot* robot) {
    printw("%s\n", robot->GetName().c_str());
    printw("HP: %d/%d\n", robot->GetHp(), robot->GetMaxHp());
    printw("EN: %d/%d\n", robot->GetEnergy(), robot->GetMaxEnergy());
    printw("ATK: %d\n", robot->GetAttackPower());
    printw("SPD: %d\n", robot->GetSpeed());
    printw("RANK: %d\n", robot->GetRank());
    printw("EXP: %d/%d\n", robot->GetExp(), robot->GetExpToNextRank());
    printw("POS: (%d, %d)\n", robot->x, robot->y);
}
