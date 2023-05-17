#include <Shader.h>
#include <d3dcompiler.h>

HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob* errorBlob;

	D3DX11CompileFromFile(filename, NULL, NULL, entryPoint, shaderModel,
		dwShaderFlags, 0, NULL, blob, &errorBlob, NULL);

	return S_OK;
}

Shader::Shader(wstring fileName)
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	inputLayout = nullptr;
	layout = vector <D3D11_INPUT_ELEMENT_DESC>();
	filePath = fileName;
	name = fileName.substr(fileName.find_first_of('/') + 1, fileName.find_last_of('_') - fileName.find_first_of('/') - 1);
}

Shader::Shader()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	inputLayout = nullptr;
	layout = vector <D3D11_INPUT_ELEMENT_DESC>();
}
Shader::~Shader() {};

HRESULT Shader::Compile(ID3D11Device* device)
{
	ID3DBlob* vertexBlob = nullptr;
	ID3DBlob* pixelBlob = nullptr;

	CompileShaderFromFile((WCHAR *)filePath.data(), "VS", "vs_5_0", &vertexBlob);
	CompileShaderFromFile((WCHAR*) filePath.data(), "PS", "ps_5_0", &pixelBlob);
	device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &vertexShader);
	device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, &pixelShader);

	HRESULT h;

	if (!layout.empty())
		h = device->CreateInputLayout(layout.data(), layout.size(), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &inputLayout);
	else
		return E_FAIL;

	return S_OK;

	vertexBlob->Release();
	pixelBlob->Release();
}

void Shader::Apply(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);
}

void Shader::SetInputLayout(vector<D3D11_INPUT_ELEMENT_DESC> inputLayout)
{
	layout = inputLayout;
}

wstring Shader::GetName()
{
	return name;
}

void Shader::Release()
{
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();
	if (inputLayout) inputLayout->Release();
}