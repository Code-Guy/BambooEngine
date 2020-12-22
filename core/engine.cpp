#include "engine.h"
#include "io/asset_loader.h"

void Engine::init()
{
	backend.init(1280, 720);
	//renderer.init(&backend);
	
	std::vector<StaticMeshComponent> staticMeshComponents;
	AssetLoader::getInstance().loadModel("asset/model/dinosaur/dinosaur.fbx", staticMeshComponents);
}

void Engine::run()
{
	std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(backend.getWindow()))
	{
		glfwPollEvents();

		//renderer.render();

		std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
		float deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - beginTime).count() * 1e-9);

		char title[100];
		snprintf(title, sizeof(title), "Bamboo Engine | FPS: %d", static_cast<int>(1.0f / deltaTime));
		glfwSetWindowTitle(backend.getWindow(), title);

		beginTime = std::chrono::steady_clock::now();
	}

	vkDeviceWaitIdle(backend.getDevice());
}

void Engine::destroy()
{
	//renderer.destroy();
	backend.destroy();
}
