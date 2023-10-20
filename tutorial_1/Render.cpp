#include "Render.h"
#include "Main.h"
#include "Cell.h"
#include <iostream>

void Render::ActualDisplay(){
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	// access currentCellsBuffer by row and col 
	for (const auto &row : Grid::GetInstance().currentCellsBuffer) {
		for (const auto &cell : row) {
			switch (cell.GetType()) {
				case CellType::CANCER:
					glColor3f(colorRed.r, colorRed.g, colorRed.b);
					break;
				case CellType::HEALTHY:
					glColor3f(colorGreen.r, colorGreen.g, colorGreen.b);
					break;
				case CellType::MEDICINE:
					glColor3f(colorYellow.r, colorYellow.g, colorYellow.b);
					break;
				default: 
					glColor3f(0, 0, 0);
					break;
			}
			PosInGrid pos = cell.GetPosition();
			glVertex2f((float)pos.x, (float)pos.y);
		}
	}
	glEnd();

	// flush buffer
	glutSwapBuffers();
}

void Render::Reshape(int w, int h) {
	glutReshapeWindow(GetInstance().displaySize.width, GetInstance().displaySize.height);
}
