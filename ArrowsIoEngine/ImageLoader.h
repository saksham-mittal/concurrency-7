#pragma once
#include "GLTexture.h"
#include <string>

namespace ArrowsIoEngine {

	class ImageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);
	};

}