#ifndef FIELD_H_
#define FIELD_H_

#include "Cell.h"
#include <vector>

class Field {
private:
    int width, height;
    std::vector<std::vector<Cell>> grid;

public:
    Field(int width, int height);

    int GetWidth() const;
    int GetHeight() const;
    Cell& GetCell(int x, int y);

    bool IsWalkable(int x, int y) const;
    void ClearVisibility();
};

#endif