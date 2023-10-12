#pragma once

#include <string>
#include <vector>

class File {
	public:
		static std::string ReadTextFile(const std::string& filename);
		static std::vector<unsigned char> ReadBinaryFile(const std::string& filename);
};
