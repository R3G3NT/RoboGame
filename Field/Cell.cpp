#include "Cell.h"
#include "../Robot/Robot.h"

Cell::Cell(bool walkable) : walkable(walkable) {}

bool Cell::IsWalkable() const {
    return walkable && (robot == nullptr);
}

bool Cell::HasRobot() const {
    return robot != nullptr;
}

Robot* Cell::get_robot() const { return robot; }

void Cell::SetWalkable(bool p_walkable) {
    walkable = p_walkable;
}

void Cell::SetRobot(Robot* other) {
    robot = other;
}

void Cell::RemoveRobot() {
    robot = nullptr;
}
