#pragma once

#include <GL/glew.h>

namespace ArrowsIoEngine {
	struct Position {
		float x;
		float y;
	};

	struct UV {
		float u;
		float v;
	};

	struct Color {
		GLubyte r;
		GLubyte g;
		GLubyte b;
		GLubyte a;
	};

	struct Vertex {
		Position position;

		Color color;

		UV uv;

		void setUV(float u, float v)
		{
			uv.u = u;
			uv.v = v;
		}

		void setPosition(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

		void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
		{
			color.r = r;
			color.b = b;
			color.g = g;
			color.a = a;
		}
	};

}