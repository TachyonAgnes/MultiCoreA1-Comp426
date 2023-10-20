#include "Cell.h"

Grid *Grid::instance = nullptr;

void Cell::SetType(CellType newType) {
    type = newType;
}

void Cell::SetPosition(PosInGrid newPos) {
    posInGrid = newPos;
}

CellType Cell::GetType() const {
    return type;
}

PosInGrid Cell::GetPosition() const {
    return posInGrid;
}
