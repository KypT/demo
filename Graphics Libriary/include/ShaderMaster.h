#include <d3d11.h>
#include <string>
#include <map>

using namespace std;

#pragma once

class ShaderMaster
{
private:
	map<string, ID3D11PixelShader*>  _pixelShaders;
	map<string, ID3D11VertexShader*> _VertexShaders;
	HWND _hwnd;

	HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob);

public: 
	HRESULT loadShadersAndInputLayout(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	ID3D11PixelShader*  getPixelShader(string shaderName);  
	ID3D11VertexShader* getVertexShader(string shaderName);
	void release();
	ShaderMaster(HWND hWnd);
	ShaderMaster();
	~ShaderMaster();
};

