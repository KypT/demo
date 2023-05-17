#include "Terrain.h"
#include <Mesh.h>
#include <vector>
#include <xnamath.h>
#include "headlers\Camera.h"

using namespace std;
using namespace demo;

void Terrain::update() {};

void Terrain::setShaders(ShaderMaster sm)
{
	_vertexShader = sm.getVertexShader("Terrain");
	_pixelShader = sm.getPixelShader("Terrain");
}

void Terrain::render(ID3D11DeviceContext* deviceContext, const ObservingCamera &camera)
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
	cb.world = XMMatrixTranspose(XMMatrixRotationRollPitchYaw(XM_PI, 0, 0) * XMMatrixTranslation(0.0f, -0.75f, 0.0f) * XMMatrixScaling(5, 5, 5));
	cb.view = XMMatrixTranspose(camera.getViewMatrix());
	cb.projection = XMMatrixTranspose(camera.getProjectionMatrix());
	cb.color = XMFLOAT4(0.0f, 1, 0, 1.0f);
	cb.lightPos = camera.getPosition();

	deviceContext->UpdateSubresource(_dataBuffer, 0, NULL, &cb, 0, 0);
	deviceContext->DrawIndexed(_indicesCount, 0, 0);
}

Terrain::Terrain(ID3D11Device* device)
{
	vector<Vertex> vertices;
	vector<WORD>   indices;

//	loadMeshFromFile("models/terrain.ply", vertices, indices);

	_verticesCount = vertices.size();
	_indicesCount = indices.size();

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

Terrain::Terrain() {}
Terrain::~Terrain(){}
