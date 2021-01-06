#pragma once

#include "entity.h"
#include "component/static_mesh_component.h"

class StaticMeshEntity : public Entity
{
public:
	StaticMeshEntity();
	virtual ~StaticMeshEntity();

	virtual void tick(float deltaTime) override;

	std::shared_ptr<StaticMeshComponent> m_staticMeshComponent;

private:
};