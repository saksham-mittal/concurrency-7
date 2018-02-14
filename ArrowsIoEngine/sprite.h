#pragma once

#include <GL\glew.h>
#include "GLTexture.h"
#include <string>

namespace ArrowsIoEngine {

	class sprite
	{
	public:
		sprite();
		~sprite();

		void init(float x, float y, float width, float height, std::string texturePath);

		void draw();

	private:
		int _x;
		int _y;
		int _width;
		int _height;
		GLuint _vboID;
		GLTexture _texture;
	};

}