#include "Field.h"

Field::Field(int p_width, int p_height) {
    width = p_width;
    height = p_height;
}

int Field::get_width() const {
    return width;
}

int Field::get_height() const {
    return height;
}

const Cell& Field::get_cell(int y, int x) const {
    return grid[y][x];
}

bool Field::is_walkable(int y, int x) const {
    return get_cell(y, x).get_walkable();
}