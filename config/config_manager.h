#pragma once

#include <yaml-cpp/yaml.h>

class ConfigManager
{
public:
	static ConfigManager& getInstance();
	void init();
	void destroy();

	std::string getShaderCompilerPath();

private:
	YAML::Node engineConfigNode;
};