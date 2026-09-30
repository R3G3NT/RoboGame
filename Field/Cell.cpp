#include "Cell.h"
#include "../Robot/Robot.h"

Cell::Cell(bool walkable) : walkable(walkable) {}

bool Cell::is_walkable() const {
    return walkable && (robot == nullptr);
}

bool Cell::has_robot() const {
    return robot != nullptr;
}

Robot* Cell::get_robot() const { return robot; }

void Cell::set_robot(Robot* other) {
    robot = other;
}

void Cell::remove_robot() {
    robot = nullptr;
}