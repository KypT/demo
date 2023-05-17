#include "Clouds.h"
#include <Mesh.h>
#include <vector>
#include "headlers\Camera.h"

using namespace std;
using namespace demo;

void Clouds::update()
{
	long t = GetTickCount();
	_shuffle.x += (t - _t) * 5e-5;
	_shuffle.y += (t - _t) * 5e-5;
	_shuffle.z += (t - _t) * 5e-5;
	_t = t;
}

void Clouds::setShaders(ShaderMaster sm)
{
	_vertexShader = sm.getVertexShader("Cloud");
	_pixelShader = sm.getPixelShader("Cloud");
}

void Clouds::render(ID3D11DeviceContext* deviceContext, const ObservingCamera &camera)
{
	deviceContext->VSSetShader(_vertexShader, NULL, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &_dataBuffer);
	deviceContext->PSSetShader(_pixelShader, NULL, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &_dataBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ConstantBuffer cb;
	cb.world = XMMatrixTranspose(XMMatrixTranslation(0.0f, 4.0f, 0.0f) * XMMatrixScaling(5, 5, 5));
	cb.view = XMMatrixTranspose(camera.getViewMatrix());
	cb.projection = XMMatrixTranspose(camera.getProjectionMatrix());
	cb.color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cb.shuffle = _shuffle;

	deviceContext->UpdateSubresource(_dataBuffer, 0, NULL, &cb, 0, 0);
	deviceContext->DrawIndexed(_indicesCount, 0, 0);
}

Clouds::Clouds(ID3D11Device* device)
{
	vector<Vertex> vertices;
	vector<WORD>   indices;

	//loadMeshFromFile("models/Terrain.ply", vertices, indices);

	srand(GetTickCount());
	_verticesCount = vertices.size();
	_indicesCount = indices.size();
	_shuffle = XMFLOAT3(rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX);
	_t = GetTickCount();

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&InitData, sizeof(InitData));

	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * _verticesCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InitData.pSysMem = vertices.data();
	device->CreateBuffer(&bd, &InitData, &_vertexBuffer);

	bd.ByteWidth = sizeof(WORD) * _indicesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	InitData.pSysMem = indices.data();
	device->CreateBuffer(&bd, &InitData, &_indexBuffer);

	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	device->CreateBuffer(&bd, NULL, &_dataBuffer);
}

Clouds::Clouds() {}
Clouds::~Clouds(){}
