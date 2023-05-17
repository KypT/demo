#include <d3d11.h>
#include <xnamath.h>
#include "ShaderMaster.h"
#include "headlers\Camera.h"

#pragma once

struct ConstantBuffer
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;

	XMFLOAT4 color;
	XMFLOAT3 lightPos;
	XMFLOAT3 shuffle;
};

class Renderable
{
protected:
	D3D11_PRIMITIVE_TOPOLOGY _topology;
	ID3D11VertexShader*      _vertexShader;
	ID3D11PixelShader*       _pixelShader;
	ID3D11Buffer*            _vertexBuffer;
	ID3D11Buffer*			 _indexBuffer;
	ID3D11Buffer*			 _dataBuffer;
	ConstantBuffer           _constantBuffer;
	int _verticesCount;
	int _indicesCount;
	long _t;

public:
	D3D11_PRIMITIVE_TOPOLOGY getTopology();
	ID3D11VertexShader*      getVertexShader();
	ID3D11PixelShader*       getPixelShader();
	virtual ID3D11Buffer*    getVertexBuffer();
	virtual ID3D11Buffer*    getIndexBuffer();
	virtual ID3D11Buffer*    getDataBuffer();
	virtual ConstantBuffer*  getShaderData();

	void release();

	virtual int verticesCount();
	virtual int indicesCount();

	virtual void update() = 0;
	virtual void setShaders(ShaderMaster sm) = 0;
	virtual void render(ID3D11DeviceContext* deviceContext, const ObservingCamera &camera) = 0;

	Renderable();
	virtual ~Renderable();
};

