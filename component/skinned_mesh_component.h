#pragma once

#include "mesh_component.h"

class SkinnedMeshComponent : public MeshComponent
{
public:
	std::shared_ptr<SkinnedMesh> m_mesh;
};