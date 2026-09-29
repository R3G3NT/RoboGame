#include "Cell.h"

Cell::Cell(bool p_walkable) {
    walkable = p_walkable;
}

bool Cell::get_walkable() const {
    return walkable;
}
