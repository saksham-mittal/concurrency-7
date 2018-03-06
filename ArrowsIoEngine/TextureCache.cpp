#include "TextureCache.h"
#include "ImageLoader.h"
#include <iostream>

namespace ArrowsIoEngine {

	TextureCache::TextureCache()
	{
	}


	TextureCache::~TextureCache()
	{
	}

	GLTexture TextureCache::getTexture(std::string texturePath)
	{
		//loopup for the texture in map
		auto mit = _textureMap.find(texturePath);

		//if texture is not in map
		if (mit == _textureMap.end())
		{
			GLTexture newTexture = ImageLoader::loadPNG(texturePath);

			//insert texture to map
			_textureMap.insert(make_pair(texturePath, newTexture));

			//std::cout << "Loaded Texture! \n";
			return newTexture;
		}

		//std::cout << "Used Cached Texture! \n";
		return mit->second;
	}

}