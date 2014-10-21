#include "pch.h"
#include "HelloWindowsPhoneComponent.h"
#include "Direct3DContentProvider.h"

using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Microsoft::WRL;
using namespace Windows::Phone::Graphics::Interop;
using namespace Windows::Phone::Input::Interop;


#include "Game.h"
namespace PhoneDirect3DXamlAppComponent
{

Direct3DBackground::Direct3DBackground()
{
}

IDrawingSurfaceBackgroundContentProvider^ Direct3DBackground::CreateContentProvider()
{
	ComPtr<Direct3DContentProvider> provider = Make<Direct3DContentProvider>(this);
	return reinterpret_cast<IDrawingSurfaceBackgroundContentProvider^>(provider.Detach());
}

// IDrawingSurfaceManipulationHandler
void Direct3DBackground::SetManipulationHost(DrawingSurfaceManipulationHost^ manipulationHost)
{
	manipulationHost->PointerPressed +=
		ref new TypedEventHandler<DrawingSurfaceManipulationHost^, PointerEventArgs^>(this, &Direct3DBackground::OnPointerPressed);

	manipulationHost->PointerMoved +=
		ref new TypedEventHandler<DrawingSurfaceManipulationHost^, PointerEventArgs^>(this, &Direct3DBackground::OnPointerMoved);

	manipulationHost->PointerReleased +=
		ref new TypedEventHandler<DrawingSurfaceManipulationHost^, PointerEventArgs^>(this, &Direct3DBackground::OnPointerReleased);
}

// Event Handlers
void Direct3DBackground::OnPointerPressed(DrawingSurfaceManipulationHost^ sender, PointerEventArgs^ args)
{
	float x = args->CurrentPoint->Position.X;
	float y = args->CurrentPoint->Position.Y;
	float ndc_x = (x / RenderResolution.Width)*2.0f - 1.0f;
	float ndc_y = (y / RenderResolution.Height)*2.0f - 1.0f;
	int ret = game.ScreenPositionToObject(ndc_x, ndc_y);
	if(ret == 1)
	{
		ShowMessageInvoker("You click on box 1, we will update live tile !!!!!!!! check it out !!!!!");
		UpdateTileInvoker();
	}
	else if(ret == 2)
	{
		ShowMessageInvoker("You click on box 2, we will show a toast !!!!!!!! check it out !!!!!");
		ShowToastInvoker("This is toast title", "and this is, you know, toast message");

	}
	else if(ret == 3)
	{
		ShowMessageInvoker("You click on box 3, we will do nothing !!!!!");
	}
}

void Direct3DBackground::OnPointerMoved(DrawingSurfaceManipulationHost^ sender, PointerEventArgs^ args)
{
	
	// Insert your code here.
}

void Direct3DBackground::OnPointerReleased(DrawingSurfaceManipulationHost^ sender, PointerEventArgs^ args)
{
	// Insert your code here.
}

// Interface With Direct3DContentProvider
HRESULT Direct3DBackground::Connect(_In_ IDrawingSurfaceRuntimeHostNative* host, _In_ ID3D11Device1* device)
{
	game.Initialize(device);
	return S_OK;
}

void Direct3DBackground::Disconnect()
{

}

HRESULT Direct3DBackground::PrepareResources(_In_ const LARGE_INTEGER* presentTargetTime, _Inout_ DrawingSurfaceSizeF* desiredRenderTargetSize)
{
	desiredRenderTargetSize->width = RenderResolution.Width;
	desiredRenderTargetSize->height = RenderResolution.Height;

	return S_OK;
}

HRESULT Direct3DBackground::Draw(_In_ ID3D11Device1* device, _In_ ID3D11DeviceContext1* context, _In_ ID3D11RenderTargetView* renderTargetView)
{
	game.UpdateDirectX(device, context, renderTargetView);
	RequestAdditionalFrame();
	game.Update();
	game.Render();
	return S_OK;
}

}