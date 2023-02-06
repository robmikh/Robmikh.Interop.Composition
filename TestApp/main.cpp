#include "pch.h"
#include "MainWindow.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Numerics;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Streams;
    using namespace Windows::UI;
    using namespace Windows::UI::Composition;
    using namespace Robmikh::Interop::Composition::Effects;
}

namespace util
{
    using namespace robmikh::common::uwp;
    using namespace robmikh::common::desktop;
}

winrt::IAsyncAction LoadTestImagesAsync(
    winrt::Visual visual,
    winrt::CompositionDrawingSurface surface1,
    std::wstring surface1FileName,
    winrt::CompositionDrawingSurface surface2,
    std::wstring surface2FileName,
    winrt::com_ptr<ID3D11Device> d3dDevice);
std::wstring GetModulePath(HMODULE module);
std::future<winrt::com_ptr<ID3D11Texture2D>> LoadTextureFromFileAsync(
    winrt::StorageFile const& file,
    winrt::com_ptr<ID3D11Device> d3dDevice);
void CopyTexutreIntoCompositionSurface(
    winrt::CompositionDrawingSurface const& surface,
    winrt::com_ptr<ID3D11Texture2D> const& sourceTexture,
    winrt::com_ptr<ID3D11DeviceContext> const& d3dContext);

int __stdcall WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    // Initialize COM
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // Create the DispatcherQueue that the compositor needs to run
    auto controller = util::CreateDispatcherQueueControllerForCurrentThread();
    auto dispatcherQueue = controller.DispatcherQueue();

    // Init D3D
    auto d3dDevice = util::CreateD3DDevice();

    // Create our window and visual tree
    auto window = MainWindow(L"TestApp", 800, 600);
    auto compositor = winrt::Compositor();
    auto compGraphics = util::CreateCompositionGraphicsDevice(compositor, d3dDevice.get());
    auto target = window.CreateWindowTarget(compositor);
    auto root = compositor.CreateSpriteVisual();
    root.RelativeSizeAdjustment({ 1.0f, 1.0f });
    root.Brush(compositor.CreateColorBrush(winrt::Colors::White()));
    target.Root(root);

    // Effects

    // Color
    auto colorEffect = winrt::ColorSourceEffect();
    colorEffect.Color(winrt::Colors::LightSeaGreen());
    auto colorEffectFactory = compositor.CreateEffectFactory(colorEffect);
    auto colorEffectBrush = colorEffectFactory.CreateBrush();

    auto colorEffectVisual = compositor.CreateSpriteVisual();
    colorEffectVisual.Size({ 200, 200 });
    colorEffectVisual.Brush(colorEffectBrush);
    root.Children().InsertAtTop(colorEffectVisual);

    // Gaussian Blur
    auto blurEffect = winrt::GaussianBlurEffect();
    blurEffect.BlurAmount(30.0f);
    blurEffect.BorderMode(winrt::EffectBorderMode::Hard);
    blurEffect.Source(winrt::CompositionEffectSourceParameter(L"Backdrop"));
    auto blurEffectFactory = compositor.CreateEffectFactory(blurEffect);
    auto blurEffectBrush = blurEffectFactory.CreateBrush();
    blurEffectBrush.SetSourceParameter(L"Backdrop", compositor.CreateBackdropBrush());

    auto blurEffectVisual = compositor.CreateSpriteVisual();
    blurEffectVisual.Offset({ 100, 100, 0 });
    blurEffectVisual.Size({ 200, 200 });
    blurEffectVisual.Brush(blurEffectBrush);
    root.Children().InsertAtTop(blurEffectVisual);

    // Saturation
    auto saturationEffect = winrt::SaturationEffect();
    saturationEffect.Saturation(0.3f);
    saturationEffect.Source(colorEffect);
    auto saturationEffectFactory = compositor.CreateEffectFactory(saturationEffect);
    auto saturationEffectBrush = saturationEffectFactory.CreateBrush();

    auto saturationEffectVisual = compositor.CreateSpriteVisual();
    saturationEffectVisual.Offset({ 200, 0, 0 });
    saturationEffectVisual.Size({ 200, 200 });
    saturationEffectVisual.Brush(saturationEffectBrush);
    root.Children().InsertAtBottom(saturationEffectVisual);

    // Blend
    auto blendEffect = winrt::BlendEffect();
    blendEffect.Mode(winrt::BlendEffectMode::Exclusion);
    blendEffect.Background(winrt::CompositionEffectSourceParameter(L"Background"));
    blendEffect.Foreground(winrt::CompositionEffectSourceParameter(L"Foreground"));
    auto blendEffectFactory = compositor.CreateEffectFactory(blendEffect);
    auto blendEffectBrush = blendEffectFactory.CreateBrush();
    auto backgroundSurface = compGraphics.CreateDrawingSurface2({ 1, 1 }, winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized, winrt::DirectXAlphaMode::Premultiplied);
    auto foregroundSurface = compGraphics.CreateDrawingSurface2({ 1, 1 }, winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized, winrt::DirectXAlphaMode::Premultiplied);
    blendEffectBrush.SetSourceParameter(L"Background", compositor.CreateSurfaceBrush(backgroundSurface));
    blendEffectBrush.SetSourceParameter(L"Foreground", compositor.CreateSurfaceBrush(foregroundSurface));

    auto blendEffectVisual = compositor.CreateSpriteVisual();
    blendEffectVisual.Offset({ 0, 400, 0 });
    blendEffectVisual.Size({ 200, 200 });
    blendEffectVisual.Brush(blendEffectBrush);
    root.Children().InsertAtBottom(blendEffectVisual);

    // Load the test images
    dispatcherQueue.TryEnqueue([blendEffectVisual, backgroundSurface, foregroundSurface, d3dDevice]() -> winrt::fire_and_forget
        {
            co_await LoadTestImagesAsync(blendEffectVisual, backgroundSurface, L"default-before.jpg", foregroundSurface, L"4-arthimetic-composite2.jpg", d3dDevice);
        });

    // Composite
    auto compositeEffect = winrt::CompositeEffect();
    compositeEffect.Mode(winrt::CompositeMode::SourceOver);
    auto compositeSources = compositeEffect.Sources();
    compositeSources.Append(winrt::CompositionEffectSourceParameter(L"Input1"));
    compositeSources.Append(winrt::CompositionEffectSourceParameter(L"Input2"));
    auto compositeEffectFactory = compositor.CreateEffectFactory(compositeEffect);
    auto compositeEffectBrush = compositeEffectFactory.CreateBrush();
    auto input1Surface = compGraphics.CreateDrawingSurface2({ 1, 1 }, winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized, winrt::DirectXAlphaMode::Premultiplied);
    auto input2Surface = compGraphics.CreateDrawingSurface2({ 1, 1 }, winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized, winrt::DirectXAlphaMode::Premultiplied);
    compositeEffectBrush.SetSourceParameter(L"Input1", compositor.CreateSurfaceBrush(input1Surface));
    compositeEffectBrush.SetSourceParameter(L"Input2", compositor.CreateSurfaceBrush(input2Surface));

    auto compositeEffectVisual = compositor.CreateSpriteVisual();
    compositeEffectVisual.Offset({ 600, 400, 0 });
    compositeEffectVisual.Size({ 200, 200 });
    compositeEffectVisual.Brush(compositeEffectBrush);
    root.Children().InsertAtBottom(compositeEffectVisual);

    // Load the test images
    dispatcherQueue.TryEnqueue([compositeEffectVisual, input1Surface, input2Surface, d3dDevice]() -> winrt::fire_and_forget
        {
            co_await LoadTestImagesAsync(compositeEffectVisual, input1Surface, L"default-before.jpg", input2Surface, L"3-composite(2of2).png", d3dDevice);
        });

    // Opacity
    auto opacityEffect = winrt::OpacityEffect();
    opacityEffect.Opacity(0.5f);
    opacityEffect.Source(colorEffect);
    auto opacityEffectFactory = compositor.CreateEffectFactory(opacityEffect);
    auto opacityEffectBrush = opacityEffectFactory.CreateBrush();

    auto opacityEffectVisual = compositor.CreateSpriteVisual();
    opacityEffectVisual.Offset({ 400, 0, 0 });
    opacityEffectVisual.Size({ 200, 200 });
    opacityEffectVisual.Brush(opacityEffectBrush);
    root.Children().InsertAtBottom(opacityEffectVisual);

    // Border
    auto borderEffect = winrt::BorderEffect();
    borderEffect.ExtendX(winrt::EdgeBehavior::Wrap);
    borderEffect.ExtendY(winrt::EdgeBehavior::Clamp);
    borderEffect.Source(winrt::CompositionEffectSourceParameter(L"Source"));
    auto borderEffectFactory = compositor.CreateEffectFactory(borderEffect);
    auto borderEffectBrush = borderEffectFactory.CreateBrush();
    auto borderEffectSourceBrush = compositor.CreateSurfaceBrush(backgroundSurface);
    borderEffectSourceBrush.HorizontalAlignmentRatio(0.0f);
    borderEffectSourceBrush.VerticalAlignmentRatio(0.0f);
    borderEffectSourceBrush.Stretch(winrt::CompositionStretch::None);
    borderEffectBrush.SetSourceParameter(L"Source", borderEffectSourceBrush);

    auto borderEffectVisual = compositor.CreateSpriteVisual();
    borderEffectVisual.Offset({ 1200, 0, 0 });
    borderEffectVisual.Size({ 800, 800 });
    borderEffectVisual.Brush(borderEffectBrush);
    root.Children().InsertAtBottom(borderEffectVisual);

    // Message pump
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return util::ShutdownDispatcherQueueControllerAndWait(controller, static_cast<int>(msg.wParam));
}

winrt::IAsyncAction LoadTestImagesAsync(
    winrt::Visual visual,
    winrt::CompositionDrawingSurface surface1,
    std::wstring surface1FileName,
    winrt::CompositionDrawingSurface surface2,
    std::wstring surface2FileName,
    winrt::com_ptr<ID3D11Device> d3dDevice)
{
    winrt::apartment_context context;

    auto exePath = std::filesystem::path(GetModulePath(nullptr));
    auto folderPath = exePath.parent_path();
    auto folder = co_await winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(folderPath.wstring());

    auto surface1ImageFile = co_await folder.GetFileAsync(surface1FileName);
    auto surface2ImageFile = co_await folder.GetFileAsync(surface2FileName);

    auto surface1Texture = co_await LoadTextureFromFileAsync(surface1ImageFile, d3dDevice);
    auto surface2Texture = co_await LoadTextureFromFileAsync(surface2ImageFile, d3dDevice);
    co_await context;

    winrt::com_ptr<ID3D11DeviceContext> d3dContext;
    d3dDevice->GetImmediateContext(d3dContext.put());
    CopyTexutreIntoCompositionSurface(surface1, surface1Texture, d3dContext);
    CopyTexutreIntoCompositionSurface(surface2, surface2Texture, d3dContext);

    auto size = surface1.Size();
    visual.Size({ size.Width, size.Height });
}

std::wstring GetModulePath(HMODULE module)
{
    std::wstring path(MAX_PATH, L'\0');
    DWORD size = static_cast<DWORD>(path.size());
    auto newSize = GetModuleFileNameW(module, path.data(), size);
    path.resize(newSize);
    return path;
}

std::future<winrt::com_ptr<ID3D11Texture2D>> LoadTextureFromFileAsync(
    winrt::StorageFile const& file,
    winrt::com_ptr<ID3D11Device> d3dDevice)
{
    auto stream = co_await file.OpenReadAsync();
    auto texture = co_await util::LoadTextureFromStreamAsync(stream, d3dDevice);
    co_return texture;
}

void CopyTexutreIntoCompositionSurface(
    winrt::CompositionDrawingSurface const& surface,
    winrt::com_ptr<ID3D11Texture2D> const& sourceTexture,
    winrt::com_ptr<ID3D11DeviceContext> const& d3dContext)
{
    // Since we're going to interop with D3D, we'll need the inteorp COM interface from the surface.
    auto surfaceInterop = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();

    // Make sure our surface is the correct size for our image.
    D3D11_TEXTURE2D_DESC desc = {};
    sourceTexture->GetDesc(&desc);
    winrt::check_hresult(surfaceInterop->Resize({ static_cast<LONG>(desc.Width), static_cast<LONG>(desc.Height) }));

    POINT offset = {};
    winrt::com_ptr<ID3D11Texture2D> surfaceTexture;
    winrt::check_hresult(surfaceInterop->BeginDraw(nullptr, winrt::guid_of<ID3D11Texture2D>(), surfaceTexture.put_void(), &offset));
    auto scopeExit = wil::scope_exit([surfaceInterop]()
        {
            winrt::check_hresult(surfaceInterop->EndDraw());
        });

    d3dContext->CopySubresourceRegion(
        surfaceTexture.get(),
        0,
        offset.x,
        offset.y,
        0,
        sourceTexture.get(),
        0,
        nullptr);
}