#include "Field.h"
#include <stdexcept>

Field::Field(int width, int height) {
    if (width < 10 || width > 60 || height < 5 || height > 20) {
        throw std::invalid_argument("Invalid field size!");
    }
    this->width = width;
    this->height = height;
    grid.resize(height, std::vector<Cell>(width, Cell(Cell::Terrain::Floor)));
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
            if (grid[y][x].GetTerrain() == Cell::Terrain::Floor ||
                grid[y][x].GetTerrain() == Cell::Terrain::Swamp) {
                grid[y][x].SetVisible(false);
            }
        }
    }
}