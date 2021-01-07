#include "entity.h"
#include "component/component.h"

Entity::Entity(std::shared_ptr<entt::registry> registry, entt::entity handle) :
	m_registry(registry), m_handle(handle)
{
	m_parent = nullptr;
}

Entity::Entity()
{

}

Entity::~Entity()
{

}

void Entity::attach(Entity* parent)
{
	m_parent = parent;
	parent->m_children.insert(this);
}

void Entity::detach()
{
	if (m_parent)
	{
		m_parent->m_children.erase(this);
		m_parent = nullptr;
	}
}

void Entity::tick()
{
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
