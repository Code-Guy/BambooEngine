#pragma once

#include "base_component.h"
#include "material.h"
#include "mesh.h"
#include "rendering/batch_resource.h"

struct Section
{
	std::string name;
	std::shared_ptr<Material> material;
	uint32_t indexCount;
};

class MeshComponent : public BaseComponent
{
public:
	virtual void tick(float deltaTime) override;

	std::vector<Section> m_sections;
};