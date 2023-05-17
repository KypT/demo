#include "ShaderMaster.h"
#include <d3dcompiler.h>
#include <d3dx11.h>
#include <comdef.h>
#include <string>

HRESULT ShaderMaster::loadShadersAndInputLayout(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ID3D11InputLayout*  inputLayout = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader*  pixelShader = nullptr;
	ID3DBlob* VSBlob = NULL;
	ID3DBlob* PSBlob = NULL;
	
	CompileShaderFromFile(L"shaders/SolidShader.fx", "VS", "vs_5_0", &VSBlob);
	CompileShaderFromFile(L"shaders/SolidShader.fx", "PS", "ps_5_0", &PSBlob);
	device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &pixelShader);

	_VertexShaders["Solid"] = vertexShader;
	_pixelShaders ["Solid"] = pixelShader;

	CompileShaderFromFile(L"shaders/SimpleLightShader.fx", "VS", "vs_5_0", &VSBlob);
	CompileShaderFromFile(L"shaders/SimpleLightShader.fx", "PS", "ps_5_0", &PSBlob);
	device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &pixelShader);

	_VertexShaders["SimpleLight"] = vertexShader;
	_pixelShaders ["SimpleLight"] = pixelShader;

	CompileShaderFromFile(L"shaders/NoiseShader.fx", "VS", "vs_5_0", &VSBlob);
	CompileShaderFromFile(L"shaders/NoiseShader.fx", "PS", "ps_5_0", &PSBlob);
	device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &pixelShader);

	_VertexShaders["Noise"] = vertexShader;
	_pixelShaders["Noise"] = pixelShader;

	CompileShaderFromFile(L"shaders/PointLightShader.fx", "VS", "vs_5_0", &VSBlob);
	CompileShaderFromFile(L"shaders/PointLightShader.fx", "PS", "ps_5_0", &PSBlob);
	device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &pixelShader);

	_VertexShaders["PointLight"] = vertexShader;
	_pixelShaders["PointLight"] = pixelShader;

	CompileShaderFromFile(L"shaders/TerrainShader.fx", "VS", "vs_5_0", &VSBlob);
	CompileShaderFromFile(L"shaders/TerrainShader.fx", "PS", "ps_5_0", &PSBlob);
	device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &pixelShader);

	_VertexShaders["Terrain"] = vertexShader;
	_pixelShaders["Terrain"] = pixelShader;

	CompileShaderFromFile(L"shaders/CloudShader.fx", "VS", "vs_5_0", &VSBlob);
	CompileShaderFromFile(L"shaders/CloudShader.fx", "PS", "ps_5_0", &PSBlob);
	device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &pixelShader);

	_VertexShaders["Cloud"] = vertexShader;
	_pixelShaders["Cloud"] = pixelShader;

	CompileShaderFromFile(L"shaders/TextureShader.fx", "VS", "vs_5_0", &VSBlob);
	CompileShaderFromFile(L"shaders/TextureShader.fx", "PS", "ps_5_0", &PSBlob);
	device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &pixelShader);

	_VertexShaders["Texture"] = vertexShader;
	_pixelShaders["Texture"] = pixelShader;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8 * sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(layout, ARRAYSIZE(layout), VSBlob->GetBufferPointer(),
		                      VSBlob->GetBufferSize(), &inputLayout);
	deviceContext->IASetInputLayout(inputLayout);

	VSBlob->Release();
	PSBlob->Release();

	return S_OK;
}

ID3D11PixelShader*  ShaderMaster::getPixelShader(string shaderName)
{
	return _pixelShaders[shaderName];
}

ID3D11VertexShader* ShaderMaster::getVertexShader(string shaderName)
{
	return _VertexShaders[shaderName];
}

HRESULT ShaderMaster::CompileShaderFromFile(WCHAR* filename, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob* errorBlob;

	HRESULT hr = D3DX11CompileFromFile(filename, NULL, NULL, entryPoint, shaderModel,
		dwShaderFlags, 0, NULL, blob, &errorBlob, NULL);
	if (FAILED(hr))
	{
		_com_error error(hr);
		MessageBox(_hwnd, error.ErrorMessage(), L"Shader error", 0);
		return hr;
	}
	if (errorBlob) errorBlob->Release();

	return S_OK;
}

ShaderMaster::ShaderMaster(HWND hwnd)
{
	_pixelShaders  = map<string, ID3D11PixelShader*> ();
	_VertexShaders = map<string, ID3D11VertexShader*>();
	_hwnd = hwnd;
}

ShaderMaster::ShaderMaster()
{
	_pixelShaders = map<string, ID3D11PixelShader*>();
	_VertexShaders = map<string, ID3D11VertexShader*>();
}

ShaderMaster::~ShaderMaster()
{

}

void ShaderMaster::release()
{
	for (map<string, ID3D11VertexShader*>::reverse_iterator it = _VertexShaders.rbegin(); it != _VertexShaders.rend(); it++)
		it->second->Release();
	for (map<string, ID3D11PixelShader*>::reverse_iterator it = _pixelShaders.rbegin(); it != _pixelShaders.rend(); it++)
		it->second->Release();
}
