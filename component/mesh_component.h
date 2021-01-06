#pragma once

#include "component.h"
#include "material.h"
#include "mesh.h"
#include "rendering/batch_resource.h"

struct Section
{
	std::string name;
	std::shared_ptr<Material> material;
	uint32_t indexCount;
};

class MeshComponent : public Component
{
public:
	std::vector<Section>& getSections() { return m_sections; }
	std::string getName() { return m_name; }
	void setName(const std::string& name) { m_name = name; }

protected:
	std::string m_name;
	std::vector<Section> m_sections;

	std::shared_ptr<BatchResource> m_batchResource;
};