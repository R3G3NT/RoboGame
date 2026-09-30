#include <iostream>
#include <ncurses.h>
#include "Field/Field.h"
#include "Game/Visualizer.h"

int main() {
    Field field(25, 10);
    Visualizer::init_screen();
    Visualizer::draw(field);
    getch();
    return 0;
}