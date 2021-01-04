#pragma once

#include "base_component.h"
#include "material.h"
#include "mesh.h"

class MeshComponent : public BaseComponent
{
public:
	virtual void tick(float deltaTime) override;

	std::vector<std::shared_ptr<Material>> m_materials;
	std::vector<uint32_t> m_sections;
};