using Windows.UI.Composition;
using Windows.UI.Composition.Desktop;
using Windows.Win32.Foundation;
using Windows.Win32.System.WinRT.Composition;
using WinRT;

namespace ManagedTestApp
{
    static class CompositionInterop
    {
        public static DesktopWindowTarget CreateTargetForWindow(this Compositor compositor, HWND window, bool topMost)
        {
            var interop = compositor.As<ICompositorDesktopInterop>();
            interop.CreateDesktopWindowTarget(window, topMost, out var target);
            return target;
        }
    }
}
