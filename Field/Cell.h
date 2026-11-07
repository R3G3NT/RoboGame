#ifndef CELL_H_
#define CELL_H_

#include <vector>
#include <memory>

class Robot;
class IStatus;

class Cell {
public:
    enum class Terrain { Floor, Wall, Swamp };

private:
    Terrain terrain = Terrain::Floor;
    bool visible = false; 
    Robot* robot = nullptr;
    
    std::vector<std::shared_ptr<IStatus>> cell_statuses;

public:
    Cell(Terrain terrain);

    Terrain GetTerrain() const;
    bool IsWalkable() const;
    bool IsVisible() const;

    void SetTerrain(Terrain terrain);
    void SetVisible(bool visible);

    Robot* GetRobot() const;
    bool HasRobot() const;
    void SetRobot(Robot* other);
    void RemoveRobot();

    void AddCellStatus(std::shared_ptr<IStatus> status);
    const std::vector<std::shared_ptr<IStatus>>& GetCellStatuses() const;
    
    void TransferRandomStatusToRobot(Robot* target_robot);
};

#endif