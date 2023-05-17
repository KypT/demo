#pragma once
#include "Renderable.h"
class Clouds :
	public Renderable
{
private:
	XMFLOAT3 _shuffle;
	long _t;

public:

	virtual void update();
	virtual void setShaders(ShaderMaster sm);
	virtual void render(ID3D11DeviceContext* deviceContext, const ObservingCamera &camera);

	Clouds(ID3D11Device* device);
	Clouds();
	~Clouds();
};

