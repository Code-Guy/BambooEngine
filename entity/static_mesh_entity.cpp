#include "static_mesh_entity.h"

StaticMeshEntity::StaticMeshEntity()
{
	m_staticMeshComponent = new StaticMeshComponent;
	registerComponent("static_mesh", m_staticMeshComponent);
}

StaticMeshEntity::~StaticMeshEntity()
{

}

void StaticMeshEntity::tick(float deltaTime)
{
	BaseEntity::tick(deltaTime);

}
