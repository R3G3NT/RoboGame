#include "Cell.h"
#include "../Robot/Robot.h"

Cell::Cell(Terrain p_terrain) : terrain(p_terrain) {}

Terrain Cell::GetTerrain() const {
    return terrain;
}

bool Cell::IsWalkable() const {
    return terrain != Terrain::Wall && robot == nullptr;
}

void Cell::SetTerrain(Terrain p_terrain) {
    terrain = p_terrain;
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
