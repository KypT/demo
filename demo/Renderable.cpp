#include "Renderable.h"

D3D11_PRIMITIVE_TOPOLOGY Renderable::getTopology()
{
	return _topology;
}

ID3D11VertexShader* Renderable::getVertexShader()
{
	return _vertexShader;
}

ID3D11PixelShader* Renderable::getPixelShader()
{
	return _pixelShader;
}

ID3D11Buffer* Renderable::getVertexBuffer()
{
	return _vertexBuffer;
}

ID3D11Buffer* Renderable::getIndexBuffer()
{
	return _indexBuffer;
}

ID3D11Buffer* Renderable::getDataBuffer()
{
	return _dataBuffer;
}

ConstantBuffer* Renderable::getShaderData()
{
	return &_constantBuffer;
}

int Renderable::verticesCount()
{
	return _verticesCount;
}

int Renderable::indicesCount()
{
	return _indicesCount;
}

Renderable::Renderable()
{
	_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_vertexShader = nullptr;
	_pixelShader = nullptr;
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
	_dataBuffer = nullptr;
	_verticesCount = 0;
	_indicesCount = 0;
	_t = GetTickCount();
}

Renderable::~Renderable(){}

void Renderable::release()
{
	if (_vertexShader) _vertexShader->Release();
	if (_pixelShader) _pixelShader->Release();
	if (_vertexBuffer) _vertexBuffer->Release();
	if (_indexBuffer) _indexBuffer->Release();
	if (_dataBuffer) _dataBuffer->Release();
}

void render(ID3D11DeviceContext* deviceContext)
{

}
