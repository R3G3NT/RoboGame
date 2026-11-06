#include "Cell.h"
#include "Robot/Robot.h"

Cell::Cell(Terrain terrain) : terrain(terrain) {}

Cell::Terrain Cell::GetTerrain() const {
    return terrain;
}

bool Cell::IsWalkable() const {
    return terrain != Terrain::Wall && robot == nullptr;
}

bool Cell::IsVisible() const {
    return visible;
}

void Cell::SetTerrain(Terrain terrain) {
    this->terrain = terrain;
}

void Cell::SetVisible(bool visible) {
    this->visible = visible;
}

Robot* Cell::GetRobot() const { return robot; }

bool Cell::HasRobot() const {
    return robot != nullptr;
}

void Cell::SetRobot(Robot* other) {
    robot = other;
}

void Cell::RemoveRobot() {
    robot = nullptr;
}