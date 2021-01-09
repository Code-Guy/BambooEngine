#include "scene.h"
#include "entity.h"
#include "input/input_manager.h"
#include "component/component.h"
#include "io/asset_loader.h"
#include "rendering/renderer.h"
#include "utility/utility.h"

void Scene::init(std::shared_ptr<class Renderer> renderer)
{
	m_renderer = renderer;

	// 初始化计时管理器
	m_timerManager = std::make_shared<TimerManager>();
	m_timerManager->addTimer(0.0166f, std::bind(&Scene::tickTransform, this, std::placeholders::_1), true);
	//m_timerManager->addTimer(0.5f, std::bind(&Scene::tickEvent, this, std::placeholders::_1), true);
	m_timerManager->addTimer(0.0333f, std::bind(&Scene::tickAnimation, this, std::placeholders::_1), true);

	// 初始化摄像机
	m_camera = std::make_unique<Camera>(glm::vec3(12.0f, 8.0f, 5.0f), 220.0f, -18.0f, 50.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 1000.0f);

	// 绑定摄像机输入事件
	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera.get(), std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera.get(), std::placeholders::_1));

	// 添加根节点
	m_rootEntity = createEntity("root");

	// 加载模型资源，生成组件
	std::vector<std::string> meshNames = {
		//"asset/model/ground/ground.fbx",
		//"asset/model/dinosaur/dinosaur.fbx",
		//"asset/model/armadillo/armadillo.fbx",
		"asset/model/mannequin/mannequin.fbx",
		"asset/model/dragon/dragon.fbx",
		//"asset/model/sponza/sponza.fbx",

		"asset/model/mannequin/mannequin_run.fbx"
	};

	for (const std::string& meshName : meshNames)
	{
		StaticMeshComponent staticMeshComp;
		SkeletalMeshComponent skeletalMeshComp;
		AnimatorComponent animatorComp;
		AssetLoader::getInstance().loadModel(meshName, staticMeshComp, skeletalMeshComp, animatorComp);

		if (animatorComp.isValid())
		{
			// 处理动画
			m_registry.view<SkeletalMeshComponent>().each([this, &animatorComp](auto entity, SkeletalMeshComponent& skeletalMeshComp) {
				if (animatorComp.isCompatible(skeletalMeshComp.skeleton))
				{
					animatorComp.skeleton = skeletalMeshComp.skeleton;
					if (m_registry.has<AnimatorComponent>(entity))
					{
						m_registry.get<AnimatorComponent>(entity).merge(animatorComp);
					}
					else
					{
						m_registry.emplace<AnimatorComponent>(entity, animatorComp);
					}
				}
			});
		}
		else
		{
			// 处理模型
			auto entity = createEntity(Utility::basename(meshName));
			if (staticMeshComp.isValid())
			{
				entity->addComponent<StaticMeshComponent>(staticMeshComp);
			}
			else if (skeletalMeshComp.isValid())
			{
				entity->addComponent<SkeletalMeshComponent>(skeletalMeshComp);
			}
			entity->attach(m_rootEntity);
		}
	}

	//m_entities["mannequin"]->attach(m_entities["sponza"]);
	m_entities["dragon"]->attach(m_entities["mannequin"]);
	m_entities["dragon"]->getComponent<TransformComponent>().position = glm::vec3(4.0f, 4.0f, 4.0f);
	m_entities["dragon"]->getComponent<TransformComponent>().rotation = glm::vec3(20.0f, 6.0f, 8.0f);
}

void Scene::destroy()
{
	m_registry.clear();
	m_entities.clear();
}

void Scene::pre()
{
	m_registry.view<StaticMeshComponent>().each([this](auto entity, StaticMeshComponent& staticMeshComp) {
		staticMeshComp.initBatchResource(m_renderer);
	});

	m_registry.view<SkeletalMeshComponent>().each([this](auto entity, SkeletalMeshComponent& skeletalMeshComp) {
		skeletalMeshComp.initBatchResource(m_renderer);
	});
}

void Scene::begin()
{
	m_timerManager->begin();
}

void Scene::tick(float deltaTime)
{
	// 更新计时器管理器
	m_timerManager->tick(deltaTime);

	// 更新相机画面长宽比
	glm::ivec2 viewportSize = m_renderer->getViewportSize();
	if (viewportSize.x != 0 && viewportSize.y != 0)
	{
		m_camera->setAspect(static_cast<float>(viewportSize.x) / viewportSize.y);
	}

	// 更新动画，上传骨骼数据
	m_registry.view<SkeletalMeshComponent, AnimatorComponent>().each([this](auto entity, SkeletalMeshComponent& skeletalMeshComp, AnimatorComponent& animatorComp) {
		TransformComponent transform;
		transform.rotation = glm::vec3(0.0f, 0.0f, m_timerManager->time() * 90.0f);
		//transform.position = glm::vec3(std::sin(m_timerManager->time()) * 100.0f, 0.0f, 0.0f);
		glm::mat4 mat = transform.calcModelMatrix();
		animatorComp.gBones[0] = mat;
		skeletalMeshComp.updateUniformBuffer(m_renderer, sizeof(SkeletalMeshUBO), static_cast<void*>(animatorComp.gBones));
	});
}

void Scene::end()
{
	m_timerManager->end();
}

void Scene::post()
{
	m_registry.view<StaticMeshComponent>().each([this](auto entity, auto& staticMeshComp) {
		staticMeshComp.destroyBatchResource(m_renderer);
	});

	m_registry.view<SkeletalMeshComponent>().each([this](auto entity, SkeletalMeshComponent& skeletalMeshComp) {
		skeletalMeshComp.destroyBatchResource(m_renderer);
	});
}

std::shared_ptr<Entity> Scene::createEntity(const std::string& name)
{
	auto entity = std::make_shared<Entity>(this, m_registry.create());

	TagComponent tagComp;
	tagComp.name = name;
	entity->addComponent<TagComponent>(tagComp);
	entity->addComponent<TransformComponent>();

	m_entities[name] = entity;
	return entity;
}

void Scene::removeEntity(const std::string& name)
{
	m_entities.erase(name);
}

void Scene::tickTransform(float deltaTime)
{
	// 更新相机姿态
	m_camera->tick(deltaTime);

	// 更新TransformComponent
	m_entities["dragon"]->getComponent<TransformComponent>().rotation = glm::vec3(0.0f, 0.0f, m_timerManager->time() * 90.0f);
	m_entities["mannequin"]->getComponent<TransformComponent>().position = glm::vec3(std::sin(m_timerManager->time()) * 100.0f, 0.0f, 0.0f);
	m_rootEntity->tick();

	// 更新StaticMeshComponent
	m_registry.view<TransformComponent, StaticMeshComponent>().each([this](auto entity, TransformComponent& transformComp, StaticMeshComponent& staticMeshComp) {
		staticMeshComp.batchResource->vpco.m = transformComp.worldMatrix;
		staticMeshComp.batchResource->vpco.mvp = m_camera->getViewPerspectiveMatrix() * transformComp.worldMatrix;
		staticMeshComp.batchResource->fpco.cameraPosition = m_camera->getPosition();
		staticMeshComp.batchResource->fpco.lightDirection = glm::vec3(-1.0f, 1.0f, -1.0f);
	});

	// 更新SkeletalMeshComponent
	m_registry.view<TransformComponent, SkeletalMeshComponent>().each([this](auto entity, TransformComponent& transformComp, SkeletalMeshComponent& skeletalMeshComp) {
		skeletalMeshComp.batchResource->vpco.m = transformComp.worldMatrix;
		skeletalMeshComp.batchResource->vpco.mvp = m_camera->getViewPerspectiveMatrix() * transformComp.worldMatrix;
		skeletalMeshComp.batchResource->fpco.cameraPosition = m_camera->getPosition();
		skeletalMeshComp.batchResource->fpco.lightDirection = glm::vec3(-1.0f, 1.0f, -1.0f);
	});
}

void Scene::tickEvent(float deltaTime)
{

}

void Scene::tickAnimation(float deltaTime)
{

}