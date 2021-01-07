#pragma once

#include <string>

class Utility
{
public:
	static std::string exec(const char* cmd);

	static bool replace(std::string& str, const std::string& from, const std::string& to);
	static void replaceAll(std::string& str, const std::string& from, const std::string& to);

	static std::string basename(const std::string& filename);

private:

};