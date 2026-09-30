#pragma once

class Robot;

class Cell {
private:
    bool walkable = true;
    Robot* robot = nullptr;

public:
    Cell(bool is_walkable);

    bool is_walkable() const;
    bool has_robot() const;

    Robot* get_robot() const;
    void set_robot(Robot* other);
    void remove_robot();
};