#include "Files.h"

std::string Files::ReadFile(const std::string& path)
{
	std::ifstream stream(path);

	if (!stream.is_open())
	{
		std::cout << "Error opening file!\n";
		return "";
	}

	std::stringstream ss;

	std::string line;
	while (getline(stream, line))
	{
		ss << line << "\n";
	}

	return ss.str();
}
