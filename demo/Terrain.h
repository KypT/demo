#pragma once
#include "Renderable.h"

class Terrain :	public Renderable
{
public:

	virtual void update();
	virtual void setShaders(ShaderMaster sm);
	virtual void render(ID3D11DeviceContext* deviceContext, const ObservingCamera &camera);

	Terrain(ID3D11Device* device);
	Terrain();
	~Terrain();
};

