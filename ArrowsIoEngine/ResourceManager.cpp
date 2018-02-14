#include "ResourceManager.h"

namespace ArrowsIoEngine {

	TextureCache ResourceManager::_textureCache;

	GLTexture ResourceManager::getTexture(std::string texturePath)
	{
		return _textureCache.getTexture(texturePath);
	}
}