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

	// ��ʼ����ʱ������
	m_timerManager = std::make_shared<TimerManager>();
	m_timerManager->addTimer(0.0166f, std::bind(&Scene::tickTransform, this, std::placeholders::_1), true);
	m_timerManager->addTimer(0.0166f, std::bind(&Scene::tickAnimation, this, std::placeholders::_1), true);
	//m_timerManager->addTimer(0.5f, std::bind(&Scene::tickEvent, this, std::placeholders::_1), true);

	// ��ʼ�������
	m_camera = std::make_unique<Camera>(glm::vec3(8.5f, -1.9f, 3.9f), -194.4f, -18.7f, 2.0f, 0.1f);
	m_camera->setFovy(45.0f);
	m_camera->setClipping(0.1f, 1000.0f);

	// ������������¼�
	InputManager::getInstance().registerKeyPressed(std::bind(&Camera::onKeyPressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerKeyReleased(std::bind(&Camera::onKeyReleased, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseOffseted(std::bind(&Camera::onMouseOffseted, m_camera.get(), std::placeholders::_1, std::placeholders::_2));
	InputManager::getInstance().registerMousePressed(std::bind(&Camera::onMousePressed, m_camera.get(), std::placeholders::_1));
	InputManager::getInstance().registerMouseReleased(std::bind(&Camera::onMouseReleased, m_camera.get(), std::placeholders::_1));

	// ��Ӹ��ڵ�
	m_rootEntity = createEntity("root");

	// ����ģ����Դ���������
	std::vector<std::string> meshNames = {
		//"asset/model/ground/ground.fbx",
		//"asset/model/dinosaur/dinosaur.fbx",
		//"asset/model/armadillo/armadillo.fbx",
		"asset/model/mannequin/mannequin.fbx",
		"asset/model/dragon/dragon.fbx",
		"asset/model/sponza/sponza.fbx",
		"asset/model/exclamation/exclamation.fbx",

		"asset/model/mannequin/mannequin_punch.fbx",
		"asset/model/mannequin/mannequin_run.fbx",
		"asset/model/mannequin/mannequin_shoot.fbx",
		"asset/model/mannequin/mannequin_climb.fbx",
	};

	for (const std::string& meshName : meshNames)
	{
		StaticMeshComponent staticMeshComp;
		SkeletalMeshComponent skeletalMeshComp;
		AnimatorComponent animatorComp;
		AssetLoader::getInstance().loadModel(meshName, staticMeshComp, skeletalMeshComp, animatorComp);

		if (animatorComp.isValid())
		{
			// ������
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
			// ����ģ��
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
	//m_entities["dragon"]->attach(m_entities["mannequin"]);
	//m_entities["dragon"]->getComponent<TransformComponent>().position = glm::vec3(4.0f, 4.0f, 4.0f);
	//m_entities["dragon"]->getComponent<TransformComponent>().rotation = glm::vec3(20.0f, 6.0f, 8.0f);

	for (uint32_t i = 0; i < 3; i++)
	{
		auto entity = createEntity((boost::format("mannequin%d") % (i + 1)).str());
		entity->cloneComponent<SkeletalMeshComponent>(m_entities["mannequin"]);
		entity->cloneComponent<AnimatorComponent>(m_entities["mannequin"]);
		entity->attach(m_rootEntity);
	}

	m_entities["exclamation"]->attach(m_entities["dragon"]);
	m_entities["exclamation"]->getComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 1.2f);
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

	// ���Ŷ���
	//m_registry.view<AnimatorComponent>().each([](auto entity, AnimatorComponent& animatorComp) {
	//	animatorComp.play("mannequin_shoot");
	//});
	m_entities["mannequin"]->getComponent<AnimatorComponent>().play("mannequin_run");
	m_entities["mannequin1"]->getComponent<AnimatorComponent>().play("mannequin_punch");
	m_entities["mannequin2"]->getComponent<AnimatorComponent>().play("mannequin_shoot");
	m_entities["mannequin3"]->getComponent<AnimatorComponent>().play("mannequin_climb");

	m_entities["mannequin"]->getComponent<TransformComponent>().position = glm::vec3(4.0f, 2.0f, 0.0);
	m_entities["mannequin1"]->getComponent<TransformComponent>().position = glm::vec3(1.0f, 0.0f, 0.0f);
	m_entities["mannequin2"]->getComponent<TransformComponent>().position = glm::vec3(-1.0f, 1.0f, 0.0f);
	m_entities["mannequin3"]->getComponent<TransformComponent>().position = glm::vec3(-1.0f, -1.0f, 0.0f);
}

void Scene::tick(float deltaTime)
{
	// ���¼�ʱ��������
	m_timerManager->tick(deltaTime);

	// ����������泤���
	glm::ivec2 viewportSize = m_renderer->getViewportSize();
	if (viewportSize.x != 0 && viewportSize.y != 0)
	{
		m_camera->setAspect(static_cast<float>(viewportSize.x) / viewportSize.y);
	}

	// �ϴ���������
	m_registry.view<SkeletalMeshComponent, AnimatorComponent>().each([this, deltaTime](auto entity, SkeletalMeshComponent& skeletalMeshComp, AnimatorComponent& animatorComp) {
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
	// ���������̬
	m_camera->tick(deltaTime);

	// ����TransformComponent
	m_entities["dragon"]->getComponent<TransformComponent>().rotation = glm::vec3(0.0f, 0.0f, m_timerManager->time() * 90.0f);
	m_entities["dragon"]->getComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, std::sin(m_timerManager->time()) * 1.0f + 1.0f);
	m_rootEntity->update();

	// ����StaticMeshComponent
	m_registry.view<TransformComponent, StaticMeshComponent>().each([this](auto entity, TransformComponent& transformComp, StaticMeshComponent& staticMeshComp) {
		staticMeshComp.batchResource->vpco.m = transformComp.worldMatrix;
		staticMeshComp.batchResource->vpco.mvp = m_camera->getViewPerspectiveMatrix() * transformComp.worldMatrix;
		staticMeshComp.batchResource->fpco.cameraPosition = m_camera->getPosition();
		staticMeshComp.batchResource->fpco.lightDirection = glm::vec3(-1.0f, 1.0f, -1.0f);
	});

	// ����SkeletalMeshComponent
	m_registry.view<TransformComponent, SkeletalMeshComponent>().each([this](auto entity, TransformComponent& transformComp, SkeletalMeshComponent& skeletalMeshComp) {
		skeletalMeshComp.batchResource->vpco.m = transformComp.worldMatrix;
		skeletalMeshComp.batchResource->vpco.mvp = m_camera->getViewPerspectiveMatrix() * transformComp.worldMatrix;
		skeletalMeshComp.batchResource->fpco.cameraPosition = m_camera->getPosition();
		skeletalMeshComp.batchResource->fpco.lightDirection = glm::vec3(-1.0f, 1.0f, -1.0f);
	});
}

void Scene::tickAnimation(float deltaTime)
{
	// ���¶���
	m_registry.view<AnimatorComponent>().each([this, deltaTime](auto entity, AnimatorComponent& animatorComp) {
		//Transform transform;
		//transform.rotation = glm::vec3(0.0f, 0.0f, m_timerManager->time() * 90.0f);
		//transform.position = glm::vec3(std::sin(m_timerManager->time()) * 100.0f, 0.0f, 0.0f);
		//glm::mat4 mat = transform.matrix();

		//animatorComp.skeleton->getBone("calf_l").animatedTransform.rotation = glm::vec3(m_timerManager->time() * 2.0f, 0.0f, 0.0f);
		//animatorComp.skeleton->getBone("calf_r").animatedTransform.rotation = glm::vec3(m_timerManager->time() * 2.0f, 0.0f, 0.0f);
		//animatorComp.skeleton->getBone("hand_l").animatedTransform.rotation = glm::vec3(m_timerManager->time() * 2.0f, 0.0f, 0.0f);
		//animatorComp.skeleton->getBone("middle_02_l").animatedTransform.rotation = glm::vec3(0.0f, m_timerManager->time() * 2.0f, 0.0f);
		animatorComp.tick(deltaTime);
	});
}

void Scene::tickEvent(float deltaTime)
{

}