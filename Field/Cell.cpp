#include "Cell.h"

Cell::Cell() {
    robot = nullptr;
    walkable = true;
}

Cell::Cell(bool p_walkable, Robot *p_robot) {
    walkable = p_walkable;
    robot = p_robot;
}

bool Cell::is_walkable() const {
    return walkable && !has_robot();
}

bool Cell::has_robot() const {
    return robot != nullptr;
}