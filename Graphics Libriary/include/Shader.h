#include <d3dx11.h>
#include <string>
#include <vector>

#pragma once

using namespace std;

class Shader
{
	public:
	ID3D11VertexShader*               vertexShader;
	ID3D11PixelShader*                pixelShader;
	ID3D11InputLayout*				  inputLayout;
	vector <D3D11_INPUT_ELEMENT_DESC> layout;
	wstring							  filePath;
	wstring							  name;
	
public:
	Shader(wstring fileName);
	Shader();
	~Shader();

	HRESULT Compile(ID3D11Device* device);
	void Apply(ID3D11DeviceContext* deviceContext);
	void SetInputLayout(vector <D3D11_INPUT_ELEMENT_DESC> inputLayout);
	wstring GetName();
	void Release();
};