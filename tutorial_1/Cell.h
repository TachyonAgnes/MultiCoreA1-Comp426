#pragma once
#include "Render.h"
#include <vector>

enum CellType {
    HEALTHY,
    CANCER,
    MEDICINE
};

struct PosInGrid {
    unsigned short x, y;
};

class Cell {
    private:
        CellType type;
        PosInGrid posInGrid;
    public:
        Cell(CellType type, PosInGrid posInGrid): type(type), posInGrid(posInGrid) {}
        Cell(): type(CellType::HEALTHY), posInGrid({0,0}) {}

        void SetType(CellType newType);
        void SetPosition(PosInGrid newPos);

        CellType GetType() const;

        PosInGrid GetPosition() const;
};

class Grid {
    private: 
        static Grid *instance;
        int width, height;
        Grid(int width, int height): width(width), height(height) {}

    public:
        // singleton pattern
        static Grid &GetInstance() {
            if (instance == nullptr) {
				instance = new Grid(Render::GetInstance().displaySize.width, Render::GetInstance().displaySize.height);
			}
			return *instance;
		}   
        Grid(const Grid&) = delete;
        Grid& operator=(const Grid&) = delete;

        // row, col
        // I am going to implement cell update method by using double buffer
        std::vector<std::vector<Cell>> currentCellsBuffer; // for rendering
        std::vector<std::vector<Cell>> nextCellsBuffer; // for data update
};