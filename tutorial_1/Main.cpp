#include "Main.h"
#include "Render.h"
#include "Logic.h"
#include "Cell.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <sstream>

// Initializes the grid of cells.
void GridInit(){
	int cellNumInTotal = Render::GetInstance().cellNumInTotal;
	int cancerCellNum = (int)(cellNumInTotal * 0.25);
	int height = Render::GetInstance().displaySize.height;
	int width = Render::GetInstance().displaySize.width;

	// initialize healty cell
	Grid::GetInstance().currentCellsBuffer = std::vector<std::vector<Cell>>(height, std::vector<Cell>(width));
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			Cell& cell = Grid::GetInstance().currentCellsBuffer[row][col];
			cell.SetType(CellType::HEALTHY);
			cell.SetPosition({static_cast<unsigned short>(col), static_cast<unsigned short>(row)});
		}
	}

	// draw cancerCellNum points with random position
	for (int i = 0; i < cancerCellNum; i++) {
		int row = (rand() % (height));
		int col = (rand() % (width));
		Cell &cell = Grid::GetInstance().currentCellsBuffer[row][col];
		cell.SetType(CellType::CANCER);
	}
	Grid::GetInstance().currentCellsBuffer = Grid::GetInstance().currentCellsBuffer;
}

/// ENVIRONMENT INITIALIZATION
void InitGLUT(int *argc, char **argv, DisplaySize displaySize) {
	// Setting up window size, display mode and creating window for the simulation.
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(Render::GetInstance().displaySize.width, Render::GetInstance().displaySize.height);
	glutCreateWindow("2D Cell Growth Simulation");
}

void InitGLEW() {
	// Throws runtime error if GLEW fails to initialize.
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Failed to initialize GLEW");
	}
}

// Sets up Orthographic Projection for 2D rendering.
void InitOrthoProjection(DisplaySize displaySize) {
	// change to orthographic projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, displaySize.width, 0, displaySize.height);
	glMatrixMode(GL_MODELVIEW);
}

// Registers Callback functions for reshaping and displaying.
void RegisterCallbacks() {
	// register callbacks
	glutReshapeFunc(Render::Reshape);
	glutDisplayFunc(Render::Display);
}

/// THREAD FUNCTIONS
// Contains the main loop of the program where cells are updated every 33 milliseconds (1s/30).
void MainLoop(int ms) {
	while (true) {
		Logic::GetInstance().UpdateAllCell(ms);
		std::cout << "__MainLoopOnUpdate__" << std::endl;
	}
}

/// MAIN FUNCTION
int main(int argc, char **argv) {
	try {
	// Render::init variables
    DisplaySize displaySize = Render::GetInstance().displaySize;
	Color colorGreen = Render::GetInstance().colorGreen;
	
	// Initialize GLUT, GLEW, and register callbacks
	InitGLUT(&argc, argv, displaySize);
	InitGLEW();
	InitOrthoProjection(displaySize);
	glViewport(0, 0, displaySize.width, displaySize.height);
	glClearColor(0, 0, 0, 1.0f);
	// Register Callbacks
	RegisterCallbacks();
	GridInit();
	glutMouseFunc(Logic::MedicineInjection);

	/// Main Loop
	std::thread t1(MainLoop, 33);
	t1.detach();

	/// Render loop
	glutMainLoop();
	

	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}
