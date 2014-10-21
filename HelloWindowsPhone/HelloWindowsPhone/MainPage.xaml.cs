using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using PhoneDirect3DXamlAppComponent;
using Microsoft.Phone.Shell;

namespace PhoneDirect3DXamlAppInterop
{
    public partial class MainPage : PhoneApplicationPage
    {
        private Direct3DBackground m_d3dBackground = new Direct3DBackground();

        // Constructor
        public MainPage()
        {
            InitializeComponent();
        }
        private void UpdateTile()
        {
            Dispatcher.BeginInvoke(
               delegate()
               {
                   Random random = new Random();
                   // get application tile
                   ShellTile tile;
                   tile = ShellTile.ActiveTiles.First();
                   if (tile != null)
                   {
                       // create a new data for tile
                       StandardTileData data = new StandardTileData();
                       // tile foreground data
                       data.Title = "Title text here";
                       data.BackgroundImage = new Uri("/Images/Blue.jpg", UriKind.Relative);
                       data.Count = random.Next(99);
                       // to make tile flip add data to background also
                       data.BackTitle = "THIS IS PRIMARY TILE";
                       data.BackBackgroundImage = new Uri("/Images/Green.jpg", UriKind.Relative);
                       data.BackContent = "SO I DRAW IT'S TEXT BIG";
                       // update tile
                       tile.Update(data);
                   }
                   tile = ShellTile.ActiveTiles.FirstOrDefault(x => x.NavigationUri.ToString().Contains("Title=SecondaryTile"));
                   if (tile != null)
                   {
                       // create a new data for tile
                       StandardTileData data = new StandardTileData();
                       // tile foreground data
                       data.Title = "Title text here";
                       data.BackgroundImage = new Uri("/Images/Blue.jpg", UriKind.Relative);
                       data.Count = random.Next(99);
                       // to make tile flip add data to background also
                       data.BackTitle = "this is secondary tile";
                       data.BackBackgroundImage = new Uri("/Images/Green.jpg", UriKind.Relative);
                       data.BackContent = "better draw it's text small";
                       // update tile
                       tile.Update(data);
                   }

               }
            );
        }
        public void ShowMessage(string message)
        {
            Dispatcher.BeginInvoke(
                delegate() 
                {
                    MessageBox.Show(message); 
                }
            );
        }
        public void ShowToast(string title, string message)
        {
            ShellToast toast = new ShellToast();
            toast.Title = title;
            toast.Content = message;
            toast.Show();
        }
        private void DrawingSurfaceBackground_Loaded(object sender, RoutedEventArgs e)
        {
            // Set window bounds in dips
            m_d3dBackground.WindowBounds = new Windows.Foundation.Size(
                (float)Application.Current.Host.Content.ActualWidth,
                (float)Application.Current.Host.Content.ActualHeight
                );

            // Set native resolution in pixels
            m_d3dBackground.NativeResolution = new Windows.Foundation.Size(
                (float)Math.Floor(Application.Current.Host.Content.ActualWidth * Application.Current.Host.Content.ScaleFactor / 100.0f + 0.5f),
                (float)Math.Floor(Application.Current.Host.Content.ActualHeight * Application.Current.Host.Content.ScaleFactor / 100.0f + 0.5f)
                );

            // Set render resolution to the full native resolution
            m_d3dBackground.RenderResolution = m_d3dBackground.NativeResolution;

            // Hook-up native component to DrawingSurfaceBackgroundGrid
            DrawingSurfaceBackground.SetBackgroundContentProvider(m_d3dBackground.CreateContentProvider());
            DrawingSurfaceBackground.SetBackgroundManipulationHandler(m_d3dBackground);
            PhoneDirect3DXamlAppComponent.Direct3DBackground.ShowMessageInvoker += ShowMessage;
            PhoneDirect3DXamlAppComponent.Direct3DBackground.UpdateTileInvoker += UpdateTile;
        }
        

    }
}