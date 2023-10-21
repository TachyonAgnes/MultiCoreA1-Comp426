#pragma once 
#include <windows.h>
#include <vector>
#include "Cell.h"

// initialize the grid
void GridInit();

// initialize GLUT, GLEW, and register callbacks
void InitGLUT(int *argc, char **argv, DisplaySize displaySize);
void InitGLEW();
void InitOrthoProjection(DisplaySize displaySize);
void RegisterCallbacks();

// main loop
void MainLoop();