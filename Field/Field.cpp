#include "Field.h"
#include <stdexcept>

Field::Field(int p_width, int p_height) {
    if (p_width < 10 || p_width > 70 || p_height < 5 || p_height > 25) {
        throw std::invalid_argument("Invalid field size!");
    }
    width = p_width;
    height = p_height;
    grid.resize(height, std::vector<Cell>(width, Cell(true)));
}

int Field::GetWidth() const { return width; }
int Field::GetHeight() const { return height; }

Cell& Field::GetCell(int x, int y) {
    return grid[y][x];
}

bool Field::IsWalkable(int x, int y) const {
    return grid[y][x].IsWalkable();
}
