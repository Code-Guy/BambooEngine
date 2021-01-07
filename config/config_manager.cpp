#include "config_manager.h"
#include <boost/format.hpp>

ConfigManager& ConfigManager::getInstance()
{
	static ConfigManager instance;
	return instance;
}

void ConfigManager::init()
{
	// º”‘ÿ≈‰÷√Œƒº˛
	const std::string configFilename("asset/config/engine.yaml");
	engineConfigNode = YAML::LoadFile(configFilename);
	if (engineConfigNode.IsNull())
	{
		throw std::runtime_error((boost::format("failed to config£∫%s") % configFilename).str());
	}
}

void ConfigManager::destroy()
{

}

std::string ConfigManager::getShaderCompilerPath()
{
	return engineConfigNode["shader_compiler_path"].as<std::string>();
}

void ConfigManager::getResolution(uint32_t& width, uint32_t& height)
{
	width = engineConfigNode["res_x"].as<uint32_t>();
	height = engineConfigNode["res_y"].as<uint32_t>();
}
