#include "src\graphics\window.h"

using namespace std;

int main() {
	using namespace sparky;
	using namespace graphics;

	Window window("Sparky", 960, 540);
	glClearColor(0.2f, 0.3f, .8f, 1.0f);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	while (!window.closed()) {
		// cout << window.getWidth() << " " << window.getHeight() << endl;
		window.clear();
		double x, y;
		window.getMousePosition(x, y);
		cout << x << ", " << y << endl;
#if 1
		glBegin(GL_QUADS);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(-0.5f, 0.5f);
		glVertex2f(0.5f,0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();
#else
		glDrawArrays(GL_ARRAY_BUFFER, 0, 6);
#endif
		window.update();
	}
	
	return 0;
}