#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <vector>
#include <dinput.h>
#include <cstdlib>
#include <string>

#include <d3dcompiler.h>
#include <comdef.h>

#include <Mesh.h>
#include <Camera.h>
#include "ShaderMaster.h"
#include "Monkey.h"
#include "Terrain.h"
#include "Clouds.h"

#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "GraphicsLibriary.lib")

using namespace std;
using namespace demo;

bool flag;

#pragma region Macro

#define HEIGHT 720
#define WIDTH  1280
#define SLICES 50
#define STACKS 50
#define INDICES_COUNT (SLICES * STACKS * 6)
#define VERTICES_COUNT (SLICES * STACKS + 2)
#define STARS_COUNT 5000
#define STARS_R     1000

#define ORBIT_OFFSET 70
#define ORBIT_SCALE 10
#define SELF_ROTATION_SPEED_SCALE 0.3
#define ORBIT_ROTATION_SPEED_SCALE 0.1
#define RADIUS_SCALE 1

#define angle_speed(T) ( 2 * XM_2PI / T )
#define RANDOM_ORBIT_ANGLE  (XM_2PI * rand() / RAND_MAX)

#pragma endregion

#pragma region Globals

HINSTANCE               hInst = NULL;
HWND                    hWnd = NULL;
IDirectInput8*          directInput;
IDirectInputDevice8*    keyboard;
ID3D11Device*           device = NULL;
ID3D11DeviceContext*    deviceContext = NULL;
IDXGISwapChain*         swapChain = NULL;
ID3D11RenderTargetView* targetView = NULL;
ID3D11Texture2D*        depthStencil = NULL;
ID3D11DepthStencilView* depthStencilView = NULL;

Monkey monkey;
Terrain terrain;
Clouds clouds;
ShaderMaster shaderMaster;
ObservingCamera camera;

#pragma endregion

#pragma region Forward Declarations

LRESULT WINAPI manageMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT initializeWindow(HINSTANCE hInstance, int nCmdShow);
void initializeWorld();
HRESULT initializeDirectX11();
HRESULT createDeviceAndSwapChain();
HRESULT setPipelineOutput();
void handleKeyboardInput();
HRESULT loadBuffers();
HRESULT loadMyBuffers();
bool checkMessages();
void gameLoop();
void nextUniverseState();
void renderScene();
void startRecord();
void cleanup();

void loadShaders();

#pragma endregion

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (FAILED(initializeWindow(hInstance, nCmdShow)))
	{
		MessageBox(hWnd, L"Failed to initialize window.", L"Critical Error", 0);
		return 0;
	}

	if (FAILED(initializeDirectX11()))
	{
		MessageBox(hWnd, L"Failed to initialize DirectX.", L"Critical Error", 0);
		return 0;
	}

	initializeWorld();

	gameLoop();
	return 0;
}

HRESULT initializeWindow(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	WNDCLASS  wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = manageMessages;
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) 6;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"GameWindow";
	RegisterClass(&wc);

	hWnd = CreateWindow(L"GameWindow", L"adAstra", WS_OVERLAPPEDWINDOW, 100, 100, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd) return E_FAIL;
	ShowWindow(hWnd, nCmdShow);
	return S_OK;
}

HRESULT initializeDirectX11()
{
	createDeviceAndSwapChain();
	setPipelineOutput();

	shaderMaster = ShaderMaster(hWnd);
	shaderMaster.loadShadersAndInputLayout(device, deviceContext);

	monkey = Monkey(device);
	monkey.setShaders(shaderMaster);

	terrain = Terrain(device);
	terrain.setShaders(shaderMaster);

	clouds = Clouds(device);
	clouds.setShaders(shaderMaster);

	srand(GetTickCount());

	D3D11_RASTERIZER_DESC wfdesc;
	ID3D11RasterizerState* WireFrame;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_BACK;
	device->CreateRasterizerState(&wfdesc, &WireFrame);
	deviceContext->RSSetState(WireFrame);

	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &directInput, NULL);
	directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);

	keyboard->SetDataFormat(&c_dfDIKeyboard);
	keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	keyboard->Acquire();

	return S_OK;
}

void initializeWorld()
{
}

HRESULT createDeviceAndSwapChain()
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	return D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
		&sd, &swapChain, &device, NULL, &deviceContext);
}

HRESULT setPipelineOutput()
{
	ID3D11Texture2D* backBuffer = NULL;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBuffer);

	device->CreateRenderTargetView(backBuffer, NULL, &targetView);
	backBuffer->Release();

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = WIDTH;
	descDepth.Height = HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, NULL, &depthStencil);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);

	deviceContext->OMSetRenderTargets(1, &targetView, depthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT) WIDTH;
	vp.Height = (FLOAT) HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);

	return S_OK;
}

void gameLoop()
{
	while (checkMessages())
	{
		keyboard->Acquire();
		handleKeyboardInput();

		monkey.update();
		camera.update();
		clouds.update();

		renderScene();
	}
	cleanup();
}

void handleKeyboardInput()
{
	char buffer[256];
	keyboard->GetDeviceState(sizeof(buffer), buffer);

	if (buffer[DIK_ESCAPE])
		PostQuitMessage(0);

	if (buffer[DIK_LCONTROL] && buffer[DIK_R])
		flag = true;
}

void makeScreenshot()
{
	ID3D11Texture2D* buffer, *image = NULL;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &buffer);

	if (image == NULL)
	{
		D3D11_TEXTURE2D_DESC td;
		buffer->GetDesc(&td);
		device->CreateTexture2D(&td, NULL, &image);
	}

	deviceContext->CopyResource(image, buffer);
	HRESULT hr = D3DX11SaveTextureToFile(deviceContext, image, D3DX11_IFF_JPG, L"ad-Astra.jpg");
}

void renderScene()
{
	float ClearColor[4] = { 0.3f, 0.3f, 1.0f, 1.0f };
	deviceContext->ClearRenderTargetView(targetView, ClearColor);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	terrain.render(deviceContext, camera);
	clouds.render(deviceContext, camera);
	monkey.render(deviceContext, camera); 
	
	for (int i = 0; i < 30; i++)
	for (int j = 0; j < 30; j++)
	if (i != 0 && j != 0)
		monkey.drawAnother(deviceContext, camera, i, j);

	swapChain->Present(0, 0);
}

void cleanup()
{
	if (deviceContext) deviceContext->ClearState();

	monkey.release();
	terrain.release();
	clouds.release();
	shaderMaster.release();

	if (keyboard) keyboard->Release();
	if (directInput) directInput->Release();
	if (depthStencil) depthStencil->Release();
	if (depthStencilView) depthStencilView->Release();
	if (targetView) targetView->Release();
	if (swapChain) swapChain->Release();
	if (deviceContext) deviceContext->Release();
	if (device) device->Release();
}

bool checkMessages()
{
	MSG msg;
	if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

LRESULT WINAPI manageMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}