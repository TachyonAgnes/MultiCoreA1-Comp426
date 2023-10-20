#pragma once
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"

// declare struct
struct Color {
	GLfloat r, g, b, a;
};

struct DisplaySize {
	int width, height;
};

// declare global variables
class Render {
    private: 
		Render() {}
	public:
		const Color colorGreen = {0.533f, 0.82f, 0.753f, 1.0f};
		const Color colorYellow = {0.906f, 0.639f, 0.173f, 1.0f};
		const Color colorRed = {0.604f, 0.122f, 0.118f, 1.0f};
		const DisplaySize displaySize = {1024, 768};
		const int cellNumInTotal = displaySize.width * displaySize.height;

		// singleton pattern
		static Render& GetInstance() {
			static Render instance;
			return instance;
		}
		Render(const Render&) = delete;
		Render& operator=(const Render&) = delete;

		static void Display() {
			GetInstance().ActualDisplay();
		}	
		void ActualDisplay();
		static void Reshape(int w, int h);
};

