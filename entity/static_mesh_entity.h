#pragma once

#include "base_entity.h"
#include "component/static_mesh_component.h"

class StaticMeshEntity : public BaseEntity
{
public:
	StaticMeshEntity();
	virtual ~StaticMeshEntity();

	virtual void tick(float deltaTime) override;

private:
	std::shared_ptr<StaticMeshComponent> m_staticMeshComponent;
};