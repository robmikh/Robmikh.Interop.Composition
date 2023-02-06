using System.Runtime.InteropServices;
using System.Windows.Interop;
using Windows.System;
using Windows.Win32.Foundation;
using Windows.Win32.System.WinRT;
using static Windows.Win32.PInvoke;

namespace ManagedTestApp.Interop
{
    static class DispatcherQueueControllerInterop
    {
        public static DispatcherQueueController CreateDispatcherQueueControllerForCurrentThread()
        {
            var options = new DispatcherQueueOptions
            {
                dwSize = (uint)Marshal.SizeOf<DispatcherQueueOptions>(),
                threadType = DISPATCHERQUEUE_THREAD_TYPE.DQTYPE_THREAD_CURRENT,
                apartmentType = DISPATCHERQUEUE_THREAD_APARTMENTTYPE.DQTAT_COM_NONE
            };
            CreateDispatcherQueueController(options, out DispatcherQueueController controller);
            return controller;
        }
    }
}
