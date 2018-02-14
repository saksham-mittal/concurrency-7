#pragma once

#include <vector>

namespace ArrowsIoEngine {

	class IOManager
	{
	public:
		static bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
	};

}