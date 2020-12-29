#pragma once

#include "base_component.h"
#include "material.h"
#include "mesh.h"

class MeshComponent : public BaseComponent
{
public:
	virtual void tick(float deltaTime) override;

	Material* m_material;
};