#pragma once

#include <yaml-cpp/yaml.h>

class ConfigManager
{
public:
	static ConfigManager& getInstance();
	void init();
	void destroy();

	std::string getShaderCompilerPath();
	void getResolution(uint32_t& width, uint32_t& height);

private:
	YAML::Node engineConfigNode;
};