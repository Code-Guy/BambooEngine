#pragma once

#include "mesh_component.h"

class StaticMeshComponent : public MeshComponent
{
public:
	std::shared_ptr<StaticMesh> m_mesh;
};