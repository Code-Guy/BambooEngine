#include "shader_manager.h"
#include "config/config_manager.h"
#include "io/asset_loader.h"
#include "utility/utility.h"
#include <boost/format.hpp>
#include <cstdlib>

ShaderManager& ShaderManager::getInstance()
{
	static ShaderManager instance;
	return instance;
}

void ShaderManager::init()
{
	std::string shaderCompilerPath = ConfigManager::getInstance().getShaderCompilerPath();
	std::vector<std::string> shaderFilenames = Utility::traverseFiles("asset/shader/src/");

	for (const std::string& shaderFilename : shaderFilenames)
	{
		std::string spvFilename = shaderFilename;
		Utility::replace(spvFilename, "src", "spv");
		Utility::replace(spvFilename, ".", "_");
		spvFilename.append(".spv");
		std::string cmd = (boost::format("%s %s -o %s") % shaderCompilerPath % shaderFilename % spvFilename).str();

		int result = std::system(cmd.c_str());
		if (result != 0)
		{
			throw std::runtime_error((boost::format("failed to compile shader: %s") % shaderFilename).str());
		}
	}
}

void ShaderManager::destroy()
{

}
