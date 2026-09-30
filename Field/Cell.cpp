#include "Cell.h"

Cell::Cell() {
    robot = nullptr;
    walkable = true;
}

Cell::Cell(Robot *p_robot, bool p_walkable) {
    walkable = p_walkable;
    robot = p_robot;
}

bool Cell::get_walkable() const {
    return walkable;
}

bool Cell::has_robot() const {
    return robot != nullptr;
}