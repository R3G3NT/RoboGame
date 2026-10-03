#ifndef CELL_H_
#define CELL_H_

class Robot;

class Cell {
private:
    bool walkable = true;
    Robot* robot = nullptr;

public:
    Cell(bool is_walkable);

    bool IsWalkable() const;
    bool HasRobot() const;

    Robot* GetRobot() const;

    void SetWalkable(bool p_walkable);
    void SetRobot(Robot* other);
    void RemoveRobot();
};

#endif
