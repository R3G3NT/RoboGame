#pragma once
#include <vector>
#include "Cell.h"

class Field {
private:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

public:
    Field(int p_width, int p_height);

    int get_width() const;
    int get_height() const;
    const Cell& get_cell(int y, int x) const;
    bool is_walkable(int y, int x) const;
};