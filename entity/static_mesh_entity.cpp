#include "static_mesh_entity.h"

StaticMeshEntity::StaticMeshEntity()
{
	m_staticMeshComponent = std::make_shared<StaticMeshComponent>();
	registerComponent("static_mesh", m_staticMeshComponent.get());
}

StaticMeshEntity::~StaticMeshEntity()
{

}

void StaticMeshEntity::tick(float deltaTime)
{
	Entity::tick(deltaTime);

}
