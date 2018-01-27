#include "src\graphics\window.h"
#include "src\maths\vec2.h"
using namespace std;

int main() {
	using namespace sparky;
	using namespace graphics;
	using namespace maths;


	Window window("Sparky", 960, 540);
	glClearColor(0.2f, 0.3f, .8f, 1.0f);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	vec2 vector(1.0f, 2.0f);

	while (!window.closed()) {
		// cout << window.getWidth() << " " << window.getHeight() << endl;
		window.clear();
		cout << vector << endl;
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