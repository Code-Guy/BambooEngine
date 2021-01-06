#pragma once

#include "mesh_component.h"

class SkinnedMeshComponent : public MeshComponent
{
public:
	std::shared_ptr<SkinnedMesh>& getMesh() { return m_mesh; }
	void setMesh(std::shared_ptr<SkinnedMesh> mesh) { m_mesh = mesh; }

private:
	std::shared_ptr<SkinnedMesh> m_mesh;
};