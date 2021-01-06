#include "static_mesh_entity.h"

StaticMeshEntity::StaticMeshEntity()
{
	
}

StaticMeshEntity::~StaticMeshEntity()
{
	
}

void StaticMeshEntity::tick(float deltaTime)
{
	Entity::tick(deltaTime);

}

void StaticMeshEntity::setStaticMeshComponent(std::shared_ptr<StaticMeshComponent> staticMeshComponent)
{
	m_staticMeshComponent = staticMeshComponent;
	registerComponent("static_mesh", m_staticMeshComponent.get());
}