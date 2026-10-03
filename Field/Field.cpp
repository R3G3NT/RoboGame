#include "Field.h"
#include <stdexcept>

Field::Field(int p_width, int p_height) {
    if (p_width < 10 || p_width > 60 || p_height < 5 || p_height > 20) {
        throw std::invalid_argument("Invalid field size!");
    }
    width = p_width;
    height = p_height;
    grid.resize(height, std::vector<Cell>(width, Cell(Terrain::Floor)));
}

int Field::GetWidth() const { return width; }
int Field::GetHeight() const { return height; }

Cell& Field::GetCell(int x, int y) {
    return grid[y][x];
}

bool Field::IsWalkable(int x, int y) const {
    return grid[y][x].IsWalkable();
}

void Field::ClearVisibility() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            grid[y][x].SetVisible(false);
        }
    }
}
