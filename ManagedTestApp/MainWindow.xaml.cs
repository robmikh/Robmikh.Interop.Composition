using ManagedTestApp.Interop;
using Robmikh.Interop.Composition.Effects;
using System;
using System.CodeDom;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Navigation;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Composition;
using Windows.Win32.Foundation;
using Windows.Win32.Graphics.Direct3D11;
using Windows.Win32.System.WinRT;
using Windows.Win32.System.WinRT.Composition;
using Windows.Win32.Graphics.Dxgi.Common;
using WinRT;
using Windows.Graphics.DirectX;

namespace ManagedTestApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Compositor _compositor;
        private CompositionTarget _target;
        private ContainerVisual _root;
        private SpriteVisual _colorEffectVisual;
        private CompositionEffectFactory _colorEffectFactory;
        private SpriteVisual _acrylicVisual;
        private CompositionEffectFactory _acrylicEffectFactory;
        private CompositionDrawingSurface _noiseSurface;

        private CompositionGraphicsDevice _compGraphics;
        private ID3D11Device _d3dDevice;
        private ID3D11DeviceContext _d3dContext;

        public MainWindow()
        {
            InitializeComponent();

            var helper = new WindowInteropHelper(this);
            helper.EnsureHandle();
            var handle = helper.Handle;
            var hwnd = new HWND(handle);

            _compositor = new Compositor();
            _target = _compositor.CreateTargetForWindow(hwnd, false);
            _root = _compositor.CreateContainerVisual();
            _root.RelativeSizeAdjustment = Vector2.One;
            _target.Root = _root;

            _d3dDevice = Direct3D11Helper.CreateD3DDevice();
            _d3dDevice.GetImmediateContext(out _d3dContext);
            var compositorInterop = _compositor.As<ICompositorInterop>();
            compositorInterop.CreateGraphicsDevice(_d3dDevice, out _compGraphics);
            _noiseSurface = _compGraphics.CreateDrawingSurface(new Windows.Foundation.Size(1, 1), DirectXPixelFormat.B8G8R8A8UIntNormalized, DirectXAlphaMode.Premultiplied);

            var effect = new ColorSourceEffect();
            effect.Color = new Color() { A = 255, R = 0, G = 0, B = 255 };
            _colorEffectFactory = _compositor.CreateEffectFactory(effect);
            var colorEffectBrush = _colorEffectFactory.CreateBrush();

            _colorEffectVisual = _compositor.CreateSpriteVisual();
            _colorEffectVisual.Size = new Vector2(200, 200);
            _colorEffectVisual.Brush = colorEffectBrush;
            _root.Children.InsertAtTop(_colorEffectVisual);

            var acrylicEffect = new BlendEffect
            {
                Mode = BlendEffectMode.Overlay,
                Background = new CompositeEffect
                {
                    Mode = CompositeMode.SourceOver,
                    Sources =
                            {
                            new BlendEffect
                            {
                                Mode = BlendEffectMode.Exclusion,
                                Background = new SaturationEffect
                                {
                                    Saturation = 2,
                                    Source = new GaussianBlurEffect
                                    {
                                        Source = new CompositionEffectSourceParameter("Backdrop"),
                                        BorderMode = EffectBorderMode.Hard,
                                        BlurAmount = 30
                                    },
                                },
                                Foreground = new ColorSourceEffect()
                                {
                                    Color = Color.FromArgb(26, 255, 255, 255)
                                }
                            },
                            new ColorSourceEffect
                            {
                                Color = Color.FromArgb(153, 255, 255, 255)
                            }
                        }
                },
                Foreground = new OpacityEffect
                {
                    Opacity = 0.03f,
                    Source = new BorderEffect()
                    {
                        ExtendX = EdgeBehavior.Wrap,
                        ExtendY = EdgeBehavior.Wrap,
                        Source = new CompositionEffectSourceParameter("Noise")
                    },
                },
            };
            _acrylicEffectFactory = _compositor.CreateEffectFactory(acrylicEffect);
            var acrylicEffectBrush = _acrylicEffectFactory.CreateBrush();
            acrylicEffectBrush.SetSourceParameter("Backdrop", _compositor.CreateBackdropBrush());
            acrylicEffectBrush.SetSourceParameter("Noise", _compositor.CreateSurfaceBrush(_noiseSurface));
            LoadAssetSurface(_noiseSurface, "NoiseAsset_256X256.png");

            _acrylicVisual = _compositor.CreateSpriteVisual();
            _acrylicVisual.AnchorPoint = new Vector2(0.5f, 0.5f);
            _acrylicVisual.Size = new Vector2(-400, -200);
            _acrylicVisual.RelativeSizeAdjustment = Vector2.One;
            _acrylicVisual.RelativeOffsetAdjustment = new Vector3(0.5f, 0.5f, 0);
            _acrylicVisual.Brush = acrylicEffectBrush;
            _root.Children.InsertAtTop(_acrylicVisual);

            var backgroundSurface = _compGraphics.CreateDrawingSurface(new Windows.Foundation.Size(1, 1), DirectXPixelFormat.B8G8R8A8UIntNormalized, DirectXAlphaMode.Premultiplied);
            var background = _compositor.CreateSpriteVisual();
            background.RelativeSizeAdjustment = Vector2.One;
            var backgroundBrush = _compositor.CreateSurfaceBrush(backgroundSurface);
            backgroundBrush.Stretch = CompositionStretch.Uniform;
            background.Brush = backgroundBrush;
            _root.Children.InsertAtBottom(background);
            LoadAssetSurface(backgroundSurface, "default-before.jpg");
        }

        private async void LoadAssetSurface(CompositionDrawingSurface surface, string fileName)
        {
            var texture = await LoadTextureFromAssetFileAsync(fileName);

            var desc = new D3D11_TEXTURE2D_DESC();
            unsafe
            {
                texture.GetDesc(&desc);
            }

            surface.Resize(new Windows.Graphics.SizeInt32() { Width = (int)desc.Width, Height = (int)desc.Height });

            using (var drawingSession = new SurfaceDrawingSession<ID3D11Texture2D>(surface))
            {
                var surfaceTexture = drawingSession.UpdateObject;
                var offset = drawingSession.Offset;

                unsafe
                {
                    _d3dContext.CopySubresourceRegion(surfaceTexture, 0, (uint)offset.X, (uint)offset.Y, 0, texture, 0, null);
                }
            }
        }

        private async Task<ID3D11Texture2D> LoadTextureFromAssetFileAsync(string fileName)
        {
            var exePath = System.Reflection.Assembly.GetEntryAssembly().Location;
            var folderPath = Path.Combine(Path.GetDirectoryName(exePath), "Assets");
            var folder = await StorageFolder.GetFolderFromPathAsync(folderPath);
            var file = await folder.GetFileAsync(fileName);
            var bitmap = await LoadSoftwareBitmapFromFileAsync(file);
            return CreateTextureFromSoftwareBitmap(bitmap);
        }

        private async Task<SoftwareBitmap> LoadSoftwareBitmapFromFileAsync(StorageFile file)
        {
            using (var stream = await file.OpenReadAsync())
            {
                var decoder = await BitmapDecoder.CreateAsync(stream);
                var frame = await decoder.GetFrameAsync(0);
                var bitmap = await frame.GetSoftwareBitmapAsync();
                return bitmap;
            }
        }

        private ID3D11Texture2D CreateTextureFromSoftwareBitmap(SoftwareBitmap bitmap)
        {
            var pixelFormat = bitmap.BitmapPixelFormat;
            var alphaMode = bitmap.BitmapAlphaMode;

            SoftwareBitmap convertedBitmap;
            if (pixelFormat == BitmapPixelFormat.Bgra8 && alphaMode == BitmapAlphaMode.Premultiplied)
            {
                convertedBitmap = bitmap;
            }
            else
            {
                convertedBitmap = SoftwareBitmap.Convert(bitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
            }

            var width = bitmap.PixelWidth;
            var height = bitmap.PixelHeight;

            using (var buffer = convertedBitmap.LockBuffer(BitmapBufferAccessMode.Read))
            using (var reference = buffer.CreateReference())
            {
                var access = reference.As<IMemoryBufferByteAccess>();
                unsafe
                {
                    byte* bytes = null;
                    uint capacity = 0;
                    access.GetBuffer(&bytes, &capacity);

                    var desc = new D3D11_TEXTURE2D_DESC();
                    desc.Width = (uint)width;
                    desc.Height = (uint)height;
                    desc.MipLevels = 1;
                    desc.ArraySize = 1;
                    desc.Format = DXGI_FORMAT.DXGI_FORMAT_B8G8R8A8_UNORM;
                    desc.BindFlags = D3D11_BIND_FLAG.D3D11_BIND_SHADER_RESOURCE;
                    desc.SampleDesc.Count = 1;

                    var initData = new D3D11_SUBRESOURCE_DATA();
                    initData.pSysMem = bytes;
                    initData.SysMemPitch = (uint)width * 4;

                    _d3dDevice.CreateTexture2D(&desc, &initData, out var texture);
                    return texture;
                }
            }
        }
    }
}
