#include <iostream>
#include <fstream>
#include "File.h"

std::string File::ReadTextFile(const std::string &filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return "";
	}

	std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
	file.close();
	return content;
}

std::vector<unsigned char> File::ReadBinaryFile(const std::string &filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return std::vector<unsigned char>();
	}

	std::vector<unsigned char> content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
	file.close();
	return content;
}
