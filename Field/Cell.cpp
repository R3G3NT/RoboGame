#include "Cell.h"
#include "../Robot/Robot.h"

Cell::Cell(bool walkable) : walkable(walkable) {}

bool Cell::IsWalkable() const {
    return walkable && (robot == nullptr);
}

void Cell::SetWalkable(bool p_walkable) {
    walkable = p_walkable;
}

bool Cell::IsVisible() const {
    return visible;
}

void Cell::SetVisible(bool p_visible) {
    visible = p_visible;
}

bool Cell::HasRobot() const {
    return robot != nullptr;
}

Robot* Cell::GetRobot() const { return robot; }

void Cell::SetRobot(Robot* other) {
    robot = other;
}

void Cell::RemoveRobot() {
    robot = nullptr;
}
