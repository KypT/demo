#include "Renderable.h"
#include <Shader.h>

#pragma once

class Monkey : public Renderable
{
private:
	Shader shader;
	XMFLOAT3 _position;
	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* linearSampler;

public:
	Monkey(ID3D11Device* device);
	Monkey();
	void setLocation(XMFLOAT3 location);
	virtual void update();
	virtual void render(ID3D11DeviceContext* deviceContext, const ObservingCamera &camera);
	void drawAnother(ID3D11DeviceContext* deviceContext, const ObservingCamera &camera, int i, int j);
	virtual void setShaders(ShaderMaster sm);
};

