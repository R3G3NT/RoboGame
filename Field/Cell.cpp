#include "Cell.h"
#include "Robot/Robot.h"
#include "Robot/IStatus.h"

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

void Cell::AddCellStatus(std::shared_ptr<IStatus> status) {
    if (status) {
        cell_statuses.push_back(status);
    }
}

const std::vector<std::shared_ptr<IStatus>>& Cell::GetCellStatuses() const {
    return cell_statuses;
}

void Cell::TransferRandomStatusToRobot(Robot* target_robot) {
    if (!target_robot || cell_statuses.empty()) return;

    int random_idx = std::rand() % cell_statuses.size();
    
    target_robot->ApplyStatus(cell_statuses[random_idx]);
    
    cell_statuses.erase(cell_statuses.begin() + random_idx);
}