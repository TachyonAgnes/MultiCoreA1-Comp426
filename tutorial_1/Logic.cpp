#include "Logic.h"
#include "Cell.h"
#include "Render.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>

// Mutex for synchronizing access to shared resources
std::mutex mtx;

// Callback function to handle mouse clicks for injecting medicine into cells.
void Logic::MedicineInjection(int button, int state, int x, int y) {
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

	// Get reference to Render instance to access display size and other properties.
	const auto &renderInstance = Render::GetInstance();
	unsigned short radius = 1;
	int winY = renderInstance.displaySize.height - y;
	// Find the cell pointed to by the mouse cursor.
	Cell &pointedCell = Grid::GetInstance().currentCellsBuffer[winY][x];

	// make sure the pointed cell is not cancer, if is not cancer, set it to medicine
	if (pointedCell.GetType() == CellType::CANCER) { return; }
	pointedCell.SetType(CellType::MEDICINE);

	// update cell around that point
	PosInGrid pos = pointedCell.GetPosition();

	unsigned short minHeight = (0 > (pos.y - radius)) ? 0 : pos.y - radius;
	unsigned short maxHeight = (renderInstance.displaySize.height < (pos.y + radius + 1)) ? renderInstance.displaySize.height : pos.y + radius + 1;
	unsigned short minWidth = (0 > (pos.x - radius)) ? 0 : pos.x - radius;
	unsigned short maxWidth = (renderInstance.displaySize.width < (pos.x + radius + 1)) ? renderInstance.displaySize.width : pos.x + radius + 1;

	int cancerNeighborCount = 0;
	int medicineNeighborCount = 0;

	for (unsigned short newRow = minHeight; newRow < maxHeight; newRow++) {
		for (unsigned short newCol = minWidth; newCol < maxWidth; newCol++) {
			if (newRow == pos.y && newCol == pos.x) continue; // ignore center point
			Cell &currNeighborCell = Grid::GetInstance().currentCellsBuffer[newRow][newCol];
			Cell &nextNeighborCell = Grid::GetInstance().nextCellsBuffer[newRow][newCol];
			if (currNeighborCell.GetType() != CellType::CANCER) {
				nextNeighborCell.SetType(CellType::MEDICINE);
			}
		}
	}
}

// Function to safely print to the console with mutual exclusion to avoid race conditions.
void safePrint(const std::string &msg) {
	std::lock_guard<std::mutex> lock(mtx);
	std::cout << msg << std::endl;
}

// Function to update all cells in parallel, split amongst available cores.
void Logic::UpdateAllCell(unsigned short ms) {
	int height = Render::GetInstance().displaySize.height;
	int width = Render::GetInstance().displaySize.width;
	Grid::GetInstance().nextCellsBuffer = Grid::GetInstance().currentCellsBuffer;

	//// inquire number of core 
	//// acturaly it should return number of parall thread, if cpu support hyperthreading, it will return number of logical core
	int coreNum = std::thread::hardware_concurrency();

	int rowsPerThread = height / coreNum;
	std::vector<std::thread> threads;

	for (int i = 0; i < coreNum; i++) {
		int startRow = i * rowsPerThread;
		// if it is last thread, it will return the rest of rows
		int endRow = (i == coreNum - 1) ? height : startRow + rowsPerThread;

		// Parallelized loop to handle updating cells in different row ranges per thread.
		threads.emplace_back([this, startRow, endRow, width]() {
			auto start = std::chrono::high_resolution_clock::now();

			// every threads deal with start row to end row
			for (int row = startRow; row < endRow; row++) {
				for (int col = 0; col < width; col++) {
					Cell &currentCell = Grid::GetInstance().currentCellsBuffer[row][col];
					Cell &nextCell = Grid::GetInstance().nextCellsBuffer[row][col];
					UpdateCell(currentCell, nextCell);
				}
			}
			std::stringstream ss;
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			ss << "Thread ID: " << std::this_thread::get_id() << ", Total time taken by function: " << duration / 1000 << "ms";
			safePrint(ss.str());
		});
	}
	// Wait for all threads to finish execution.
	for (auto &thread : threads) {
		thread.join();
	}

	// Swap buffers and re-display.
	Grid::GetInstance().currentCellsBuffer = Grid::GetInstance().nextCellsBuffer;
	glutPostRedisplay();
	// halt for 1/30 second
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Function to update the state of an individual cell based on its neighbors.
void Logic::UpdateCell(Cell &currentCell, Cell &nextCell) {
	// Get the type and position of the current cell.
	CellType cellType = currentCell.GetType();
	PosInGrid pos = currentCell.GetPosition();
	unsigned short radius = 1;

	unsigned short minHeight = (0 > (pos.y - radius)) ? 0 : pos.y - radius;
	unsigned short maxHeight = (Render::GetInstance().displaySize.height < (pos.y + radius + 1)) ? Render::GetInstance().displaySize.height : pos.y + radius + 1;
	unsigned short minWidth = (0 > (pos.x - radius)) ? 0 : pos.x - radius;
	unsigned short maxWidth = (Render::GetInstance().displaySize.width < (pos.x + radius + 1)) ? Render::GetInstance().displaySize.width : pos.x + radius + 1;

	int cancerNeighborCount = 0;
	int medicineNeighborCount = 0;

	// Loop through neighboring cells to determine the next state of the current cell.
	bool shouldBreak = false;
	for (unsigned short newRow = minHeight; newRow < maxHeight && !shouldBreak; newRow++) {
		for (unsigned short newCol = minWidth; newCol < maxWidth && !shouldBreak; newCol++) {
			if (newRow == pos.y && newCol == pos.x) continue;
			Cell &currNeighborCell = Grid::GetInstance().currentCellsBuffer[newRow][newCol];
			Cell &nextNeighborCell = Grid::GetInstance().nextCellsBuffer[newRow][newCol];

			switch (cellType) {
			case CellType::MEDICINE:
				if (currNeighborCell.GetType() == CellType::HEALTHY) {
					Grid::GetInstance().nextCellsBuffer[newRow][newCol].SetType(CellType::MEDICINE);
				}
				break;
			case CellType::HEALTHY:
				if (currNeighborCell.GetType() == CellType::CANCER) {
					cancerNeighborCount++;
					if (cancerNeighborCount >= 6) {
						nextCell.SetType(CellType::CANCER);
						shouldBreak = true;
					}
				}
				break;
			case CellType::CANCER:
				if (currNeighborCell.GetType() == CellType::MEDICINE) {
					medicineNeighborCount++;
					if (medicineNeighborCount >= 6) {
						nextCell.SetType(CellType::HEALTHY);
						for (unsigned short i = minHeight; i < maxHeight; i++) {
							for (unsigned short j = minWidth; j < maxWidth; j++) {
								if (i == pos.y && j == pos.x) continue;
								Cell &surroundingCell = Grid::GetInstance().nextCellsBuffer[i][j];
								if (surroundingCell.GetType() == CellType::MEDICINE) {
									surroundingCell.SetType(CellType::HEALTHY);
								}
							}
						}
						shouldBreak = true;
					}
				}
				break;
			default:
				break;
			}
		}
	}
}