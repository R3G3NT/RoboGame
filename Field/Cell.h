#pragma once

class Cell {
private:
    bool walkable;

public:
    Cell(bool p_walkable);

    bool get_walkable() const;
};