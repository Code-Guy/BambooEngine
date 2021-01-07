#include "entity.h"
#include "component/component.h"

Entity::Entity(Scene* scene, entt::entity handle) :
	m_scene(scene), m_handle(handle)
{
	m_parent = nullptr;
}

Entity::~Entity()
{

}

void Entity::attach(std::shared_ptr<Entity> parent)
{
	detach();

	m_parent = parent.get();
	m_parent->m_children.insert(this);
}

void Entity::detach()
{
	if (m_parent)
	{
		m_parent->m_children.erase(this);
		m_parent = nullptr;
	}
}

void Entity::destroy()
{
	detach();

	auto& tag = getComponent<TagComponent>();
	m_scene->removeEntity(tag.name);
	m_scene->getRegistry().remove(m_handle);
}

void Entity::tick()
{
	auto& tag = getComponent<TagComponent>();
	auto& transform = getComponent<TransformComponent>();
	transform.localMatrix = transform.calcModelMatrix();

	if (m_parent)
	{
		auto& parentTransform = m_parent->getComponent<TransformComponent>();
		transform.worldMatrix = parentTransform.worldMatrix * transform.localMatrix;
	}
	else
	{
		transform.worldMatrix = transform.localMatrix;
	}

	for (auto& iter : m_children)
	{
		iter->tick();
	}
}
