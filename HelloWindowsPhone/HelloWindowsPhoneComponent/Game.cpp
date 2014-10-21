#include "pch.h"
#include "Game.h"
#include <fstream>

void Game::Initialize(_In_ ID3D11Device1* _device)
{
	device = _device;
	{
		D3D11_RASTERIZER_DESC rd;
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.FrontCounterClockwise = FALSE;
		rd.DepthClipEnable = TRUE;
		rd.ScissorEnable = FALSE;
		rd.AntialiasedLineEnable = FALSE;
		rd.MultisampleEnable = FALSE;
		rd.DepthBias = 0;
		rd.DepthBiasClamp = 0.0f;
		rd.SlopeScaledDepthBias = 0.0f;

		device->CreateRasterizerState(&rd, &defaultRasterizerState);
		
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.AntialiasedLineEnable = TRUE;

		device->CreateRasterizerState(&rd, &wireRasterizerState);
	}
	{
		D3D11_BLEND_DESC bd;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		bd.IndependentBlendEnable = FALSE;
		bd.AlphaToCoverageEnable = FALSE;

		device->CreateBlendState(&bd, &blendState);
	}
	{
		D3D11_SAMPLER_DESC sd;
		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		sd.MaxAnisotropy = 16;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.BorderColor[0] = 0.0f;
		sd.BorderColor[1] = 0.0f;
		sd.BorderColor[2] = 0.0f;
		sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0.0f;
		sd.MaxLOD = FLT_MAX;
		sd.MipLODBias = 0.0f;

		device->CreateSamplerState(&sd, &samplerState);    // create the anisotropic sampler
	}
	InitGraphics();
}
void Game::UpdateDirectX(_In_ ID3D11Device1* _device, _In_ ID3D11DeviceContext1* _context, _In_ ID3D11RenderTargetView* render_target)
{
	{// create texture as z buffer
		context = _context;
		renderTarget = render_target;

		if (device.Get() != _device)
		{
			device = _device;
			renderTargetSize.Width  = -1;
			renderTargetSize.Height = -1;
		}

		ComPtr<ID3D11Resource> renderTargetViewResource;
		render_target->GetResource(&renderTargetViewResource);

		ComPtr<ID3D11Texture2D> backBuffer;
		renderTargetViewResource.As(&backBuffer);

		// Cache the rendertarget dimensions in our helper class for convenient use.
		D3D11_TEXTURE2D_DESC backBufferDesc;
		backBuffer->GetDesc(&backBufferDesc);

		if (renderTargetSize.Width  != static_cast<float>(backBufferDesc.Width) ||
			renderTargetSize.Height != static_cast<float>(backBufferDesc.Height))
		{
			renderTargetSize.Width  = static_cast<float>(backBufferDesc.Width);
			renderTargetSize.Height = static_cast<float>(backBufferDesc.Height);
			// Create a depth stencil view.
			CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT,
				static_cast<UINT>(renderTargetSize.Width),
				static_cast<UINT>(renderTargetSize.Height),
				1,
				1,
				D3D11_BIND_DEPTH_STENCIL);

			ComPtr<ID3D11Texture2D> depthStencil;
			device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
			device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &zBuffer);
		}

		CD3D11_VIEWPORT viewport(
			0.0f,
			0.0f,
			renderTargetSize.Width,
			renderTargetSize.Height
			);

		context->RSSetViewports(1, &viewport);
	}
	{
		wood_cube.SetDevice(device.Get());
		wood_cube.SetContext(context.Get());
		wood_cube.BuildDirectXData();
		brick_cube.SetDevice(device.Get());
		brick_cube.SetContext(context.Get());
		brick_cube.BuildDirectXData();
		solid_cube.SetDevice(device.Get());
		solid_cube.SetContext(context.Get());
		solid_cube.BuildDirectXData();

		for (int i = 0; i < 20; i++)
		{
			ray_cube[i].SetDevice(device.Get());
			ray_cube[i].SetContext(context.Get());
			ray_cube[i].BuildDirectXData();
		}
		wood_cube.SetTexture(L"Wood.dds");
		brick_cube.SetTexture(L"bricks.dds");
		solid_cube.SetTexture(L"Wood.dds");

		for (int i = 0; i < 20; i++)
		{
			ray_cube[i].SetTexture(L"Wood.dds");
		}
	}
	{
		matProjection = XMMatrixPerspectiveFovLH(
			PI*0.25,
			renderTargetSize.Width / renderTargetSize.Height,
			1,
			100);
	}
}
void Game::InitGraphics()
{
	color[0] = 0.0; color[1] = 0.0; color[2] = 0.0; color[3] = 1.0;
	colorAccel[0] = random(-MAX_ACCEL, MAX_ACCEL);
	colorAccel[1] = random(-MAX_ACCEL, MAX_ACCEL);
	colorAccel[2] = random(-MAX_ACCEL, MAX_ACCEL);

	wood_cube.SetTranslation(0.0, 0.0, 0.0);
	brick_cube.SetTranslation(-3.0, -3.0, 0);
	solid_cube.SetTranslation(3.0, -3.0, 0);

	for (int i = 0; i < 20; i++)
	{
		ray_cube[i].SetTranslation(0.0, 0.0, 0.0);
		ray_cube[i].SetScale(0.01);
	}

	enableWireFrame = false;
	enableDepth = true;
	time = 0.0f;
}
void Game::Update()
{
	color[0] += colorAccel[0];
	color[1] += colorAccel[1];
	color[2] += colorAccel[2];
	if (color[0] <= 0.0f || color[0] >= 1.0f || abs(colorAccel[0]) < MIN_ACCEL)
	{
		colorAccel[0] = random(-MAX_ACCEL, MAX_ACCEL);
	}
	if (color[1] <= 0.0f || color[1] >= 1.0f || abs(colorAccel[1]) < MIN_ACCEL)
	{
		colorAccel[1] = random(-MAX_ACCEL, MAX_ACCEL);
	}
	if (color[2] <= 0.0f || color[2] >= 1.0f || abs(colorAccel[2]) < MIN_ACCEL)
	{
		colorAccel[2] = random(-MAX_ACCEL, MAX_ACCEL);
	}

	
	XMVECTOR cam_position = XMVectorSet(1.5f, 3.5f, 15.5f, 0);
	XMVECTOR look_at = XMVectorSet(0, 0, 0, 0);
	XMVECTOR cam_up = XMVectorSet(0, 1, 0, 0);
	matView = XMMatrixLookAtLH(cam_position, look_at, cam_up);
	
	
	time += 0.05f;
	wood_cube.SetRotationY(time);
	solid_cube.SetRotationY(time*1.5);
	brick_cube.SetRotationY(time*2);
	wood_cube.Update();
	solid_cube.Update();
	brick_cube.Update();
}
void Game::Render()
{
	context->OMSetRenderTargets(1, renderTarget.GetAddressOf(), zBuffer.Get());
	context->ClearRenderTargetView(renderTarget.Get(), color);
	context->ClearDepthStencilView(zBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	if (enableWireFrame)
	{
		context->RSSetState(wireRasterizerState.Get());
	}
	else
	{
		context->RSSetState(defaultRasterizerState.Get());
	}
	if (enableDepth)
		context->OMSetDepthStencilState(defaultDepthState.Get(), 0);
	else
		context->OMSetDepthStencilState(offDepthState.Get(), 0);
	context->OMSetBlendState(blendState.Get(), 0, 0xffffffff);
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	{
		XMMATRIX camera_transform = matView * matProjection;
		wood_cube.Draw(camera_transform);
		brick_cube.Draw(camera_transform);
		solid_cube.Draw(camera_transform);

		for (int i = 0; i < 20; i++)
		{
			ray_cube[i].Draw(camera_transform);
		}
	}
}
void Game::SwitchWireFrame()
{
	enableWireFrame = !enableWireFrame;
}
void Game::SwitchDepth()
{
	enableDepth = !enableDepth;
}
int Game::ScreenPositionToObject(float x, float y)
{
	XMVECTOR transformed_ray_begin = XMVectorSet(x, -y, 0.0f, 1.0f);
	XMVECTOR transformed_ray_end = XMVectorSet(x, -y, 1.0f, 1.0f);
	XMVECTOR ray_begin;
	XMVECTOR ray_end;
	XMMATRIX camera_transform = matView * matProjection;
	XMVECTOR det = XMMatrixDeterminant(camera_transform);
	XMMATRIX inverse_transform = XMMatrixInverse(&det, camera_transform);

	ray_begin = XMVector4Transform(transformed_ray_begin, inverse_transform);
	if (ray_begin.m128_f32[3] != 0) ray_begin = XMVectorScale(ray_begin, 1.0 / ray_begin.m128_f32[3]);
	ray_end = XMVector4Transform(transformed_ray_end, inverse_transform);
	if (ray_end.m128_f32[3] != 0) ray_end = XMVectorScale(ray_end, 1.0 / ray_end.m128_f32[3]);

	XMVECTOR ray = XMVectorSubtract(ray_end, ray_begin);
	// visually show the ray, draw 20 small cube along it
	for (int i = 0; i < 20; i++)
	{
		XMVECTOR position = XMVectorLerp(ray_begin, ray_end, (float)i / 20);
		ray_cube[i].SetTranslation(position.m128_f32[0], position.m128_f32[1], position.m128_f32[2]);
	}
	if (wood_cube.RayIntersects(ray_begin, ray_end))
	{
		wood_cube.SetBlinking(true);
		brick_cube.SetBlinking(false);
		solid_cube.SetBlinking(false);
		return 1;
	}
	if (brick_cube.RayIntersects(ray_begin, ray_end))
	{
		wood_cube.SetBlinking(false);
		brick_cube.SetBlinking(true);
		solid_cube.SetBlinking(false);
		return 2;
	}
	if (solid_cube.RayIntersects(ray_begin, ray_end))
	{
		wood_cube.SetBlinking(false);
		brick_cube.SetBlinking(false);
		solid_cube.SetBlinking(true);
		return 3;
	}
	return 0;
}