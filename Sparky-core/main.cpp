#include "src\graphics\window.h"
#include "src\maths\maths.h"
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
	vec2 vector1(2.0f, 3.0f);
	vec4 v(1.0f, 2.0f, 3.0f, 4);
	vec4 d(2, 3, 4, 5);
	
	v *= d;

	mat4 position = mat4::translation(vec3(2, 3, 4));
	position *= mat4::identity();

	position.elements[12] = 2.0f;

	vec4 column = position.columns[3];

	cout << column << endl;

	while (!window.closed()) {
		// cout << window.getWidth() << " " << window.getHeight() << endl;
		window.clear();
		//cout << v << endl;
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