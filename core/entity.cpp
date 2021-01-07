#include "entity.h"

Entity::Entity(entt::registry& registry, entt::entity handle) : 
	m_registry(registry), m_handle(handle)
{
	
}

Entity::~Entity()
{

}