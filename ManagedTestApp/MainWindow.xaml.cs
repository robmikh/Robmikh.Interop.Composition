using Robmikh.Interop.Composition.Effects;
using System;
using System.Collections.Generic;
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
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Windows.UI;
using Windows.UI.Composition;
using Windows.Win32.Foundation;

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

            var effect = new ColorSourceEffect();
            effect.Color = new Color() { A = 255, R = 0, G = 0, B = 255 };
            _colorEffectFactory = _compositor.CreateEffectFactory(effect);
            var colorEffectBrush = _colorEffectFactory.CreateBrush();

            _colorEffectVisual = _compositor.CreateSpriteVisual();
            _colorEffectVisual.Size = new Vector2(200, 200);
            _colorEffectVisual.Brush = colorEffectBrush;
            _root.Children.InsertAtTop(_colorEffectVisual);
        }
    }
}
