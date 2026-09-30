#pragma once
#include "../Robot/Robot.h"

class Cell {
private:
    bool walkable;
    Robot *robot;

public:
    Cell();
    Cell(Robot *p_robot, bool p_walkable);

    bool get_walkable() const;
    bool has_robot() const;
};