#pragma once
#include"TextureCache.h"

namespace ArrowsIoEngine {

	class ResourceManager
	{
	public:
		static GLTexture getTexture(std::string texturePath);


	private:
		static TextureCache _textureCache;
	};

}