#ifndef CELL_H_
#define CELL_H_

class Robot;

class Cell {
private:
    bool walkable = true;
    bool visible = false; 
    Robot* robot = nullptr;

public:
    Cell(bool is_walkable);

    bool IsWalkable() const;
    bool IsVisible() const;
    bool HasRobot() const;

    Robot* GetRobot() const;

    void SetWalkable(bool p_walkable);
    void SetVisible(bool p_visible);
    void SetRobot(Robot* other);
    void RemoveRobot();
};

#endif
