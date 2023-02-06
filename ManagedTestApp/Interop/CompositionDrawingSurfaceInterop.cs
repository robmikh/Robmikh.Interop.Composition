using System;
using System.Drawing;
using Windows.UI.Composition;
using Windows.Win32.System.WinRT.Composition;
using WinRT;

namespace ManagedTestApp.Interop
{
    static class CompositionDrawingSurfaceInterop
    {
        public static (T, Point) BeginDraw<T>(this CompositionDrawingSurface surface)
        {
            var interop = surface.As<ICompositionDrawingSurfaceInterop>();

            T updateObject;
            var offset = new Point();
            unsafe
            {
                var guid = typeof(T).GUID;
                interop.BeginDraw(null, &guid, out var updateObjectUnknown, &offset);
                updateObject = updateObjectUnknown.As<T>();
            }

            return (updateObject, offset);
        }

        public static void EndDraw(this CompositionDrawingSurface surface)
        {
            var interop = surface.As<ICompositionDrawingSurfaceInterop>();
            interop.EndDraw();
        }
    }

    class SurfaceDrawingSession<T> : IDisposable
    {
        public CompositionDrawingSurface Surface { get; }
        public T UpdateObject { get; }
        public Point Offset { get; }

        public SurfaceDrawingSession(CompositionDrawingSurface surface)
        {
            Surface = surface;
            var (updateObject, offset) = surface.BeginDraw<T>();
            UpdateObject = updateObject;
            Offset = offset;
        }

        public void Dispose()
        {
            Surface.EndDraw();
        }
    }

}
