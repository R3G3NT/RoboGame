#pragma once
#include "../Robot/Robot.h"

class Cell {
private:
    bool walkable;

public:
    Robot *robot; // робот, который находится на клетке

    Cell();
    Cell(bool p_walkable, Robot *p_robot);

    bool is_walkable() const;
    bool has_robot() const;
};