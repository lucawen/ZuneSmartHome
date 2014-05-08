using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using ZuneSmartHome.Resources;
using Windows.Networking.Sockets;
using Windows.Networking.Proximity;
using System.Diagnostics;
using Windows.Storage.Streams;
using System.Threading.Tasks;
using BluetoothConnectionManager;
using System.Windows.Media;

namespace ZuneSmartHome
{
    public partial class Lampadas : PhoneApplicationPage
    {
        private ConnectionManager connectionManager;

        private StateManager stateManager;

        public Lampadas()
        {
            InitializeComponent();
            connectionManager = new ConnectionManager();
            connectionManager.MessageReceived += connectionManager_MessageReceived;
            stateManager = new StateManager();
        }
        async void connectionManager_MessageReceived(string message)
        {
            Debug.WriteLine("Mensagem recebida:" + message);
            string[] messageArray = message.Split(':');
            switch (messageArray[0])
            {
                case "QUARTO":
                    stateManager.Quarto = messageArray[1] == "ON" ? true : false;
                    Dispatcher.BeginInvoke(delegate()
                    {
                        QuartoImg.Opacity = stateManager.Quarto ? 1 : 0.3;
                    });
                    break;
                case "SALA":
                    stateManager.Sala = messageArray[1] == "ON" ? true : false;
                    Dispatcher.BeginInvoke(delegate()
                    {
                        SalaImg.Opacity = stateManager.Sala ? 1 : 0.3;
                    });
                    break;
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            connectionManager.Initialize();
            stateManager.Initialize();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            connectionManager.Terminate();
        }

        private void buttonConnect_Click_1(object sender, RoutedEventArgs e)
        {
            AppToDevice();
        }

        private async void AppToDevice()
        {
            buttonConnect.Content = "Conectando...";
            PeerFinder.AlternateIdentities["Bluetooth:Paired"] = "";
            var pairedDevices = await PeerFinder.FindAllPeersAsync();

            if (pairedDevices.Count == 0)
            {
                Debug.WriteLine("Nenhum dispositivo pareado encontrado");
            }
            else
            {
                foreach (var pairedDevice in pairedDevices)
                {
                    if (pairedDevice.DisplayName == DeviceName.Text)
                    {
                        connectionManager.Connect(pairedDevice.HostName);
                        buttonConnect.Content = "Conectado";
                        DeviceName.IsReadOnly = true;
                        buttonConnect.IsEnabled = false;
                        continue;
                    }
                }
            }
        }

        private async void QuartoButton_Click_1(object sender, RoutedEventArgs e)
        {
            string command = stateManager.Quarto ? "TURN_QUARTO" : "TURN_QUARTO";
            await connectionManager.SendCommand(command);
        }

        private async void SalaButton_Click_1(object sender, RoutedEventArgs e)
        {
            string command = stateManager.Sala ? "TURN_SALA" : "TURN_SALA";
            await connectionManager.SendCommand(command);
        }

        private void DeviceName_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
    }
}