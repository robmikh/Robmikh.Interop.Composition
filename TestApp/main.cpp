#include "pch.h"
#include "MainWindow.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Numerics;
    using namespace Windows::UI;
    using namespace Windows::UI::Composition;
    using namespace Robmikh::Interop::Composition::Effects;
}

namespace util
{
    using namespace robmikh::common::desktop;
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    // Initialize COM
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // Create the DispatcherQueue that the compositor needs to run
    auto controller = util::CreateDispatcherQueueControllerForCurrentThread();

    // Create our window and visual tree
    auto window = MainWindow(L"TestApp", 800, 600);
    auto compositor = winrt::Compositor();
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

    // Message pump
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return util::ShutdownDispatcherQueueControllerAndWait(controller, static_cast<int>(msg.wParam));
}
