#pragma once

#include "mesh_component.h"

class StaticMeshComponent : public MeshComponent
{
public:
	std::shared_ptr<StaticMesh>& getMesh() { return m_mesh; }
	void setMesh(std::shared_ptr<StaticMesh> mesh) { m_mesh = mesh; }

	virtual void pre() override;
	virtual void tick(float deltaTime) override;
	virtual void post() override;

private:
	std::shared_ptr<StaticMesh> m_mesh;
};