#include "engine.h"
#include "io/asset_loader.h"

void Engine::init()
{
	// 初始化图形后端和渲染器
	backend.init(1280, 720);

	// 初始化渲染资源工厂
	ResourceFactory::getInstance().init(&backend);

	// 初始化渲染器
	renderer.init(&backend);

	// 加载模型资源，生成组件
	std::vector<StaticMeshComponent> staticMeshComponents;
	AssetLoader::getInstance().loadModel("asset/model/dinosaur/dinosaur.fbx", staticMeshComponents);

	// 通过组件生成渲染资源
	std::vector<BatchResource> batchResources(staticMeshComponents.size());
	for (size_t i = 0; i < staticMeshComponents.size(); ++i)
	{
		ResourceFactory::getInstance().createBatchResource(staticMeshComponents[i], batchResources[i]);
	}

	// 设置渲染器渲染资源
	renderer.setBatchResources(batchResources);
}

void Engine::run()
{
	std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(backend.getWindow()))
	{
		glfwPollEvents();

		renderer.render();

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
	renderer.destroy();
	ResourceFactory::getInstance().destroy();
	backend.destroy();
}
