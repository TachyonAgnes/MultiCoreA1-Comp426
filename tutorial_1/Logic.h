#pragma once
#include "Cell.h"
class Logic {
	private: 
		Logic() {};
	public:
		// singleton pattern
		static Logic &GetInstance() {
			static Logic instance;
			return instance;
		}
		Logic(const Logic &) = delete;
		Logic &operator=(const Logic &) = delete;

		// operate the cells
		static void MedicineInjection(int button, int state, int x, int y);
		void UpdateAllCell(unsigned short ms);
		void UpdateCell(Cell &currentCell, Cell &nextCell);
};

