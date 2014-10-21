#pragma once
#include "pch.h"
#include "GameHelper.h"
#include "Cube.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace DirectX;


static const float MAX_ACCEL = 0.005f;
static const float MIN_ACCEL = 0.0007f;
class Game
{
private:
	Windows::Foundation::Size			renderTargetSize;
	ComPtr<ID3D11Device1>				device;
	ComPtr<ID3D11DeviceContext1>		context;
	ComPtr<ID3D11RenderTargetView>		renderTarget;
	ComPtr<ID3D11DepthStencilView>		zBuffer;
	ComPtr<ID3D11RasterizerState>		defaultRasterizerState;
	ComPtr<ID3D11RasterizerState>		wireRasterizerState;
	ComPtr<ID3D11BlendState>			blendState;
	ComPtr<ID3D11DepthStencilState>		defaultDepthState;
	ComPtr<ID3D11DepthStencilState>		offDepthState;
	ComPtr<ID3D11SamplerState>			samplerState;
private:
	Cube wood_cube;
	Cube brick_cube;
	Cube solid_cube;
	Cube ray_cube[20];
	float color[4];
	float colorAccel[4];
	bool enableWireFrame;
	bool enableDepth;
	float time;
	XMMATRIX matView;
	XMMATRIX matProjection;
public:// game specific
	void Initialize(_In_ ID3D11Device1* device);
	void UpdateDirectX(_In_ ID3D11Device1* _device, _In_ ID3D11DeviceContext1* _context, _In_ ID3D11RenderTargetView* render_target);
	void Update();
	void Render();
	void SwitchWireFrame();
	void SwitchDepth();
	int ScreenPositionToObject(float x, float y);
private:
	void InitGraphics();
};