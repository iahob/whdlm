using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Game.Utils;
using WebSocketSharp;

namespace Game.Socket.Service
{
    public class SocketClient
    {
        public WebSocket Client { get; set; }

        public SocketClient()
        {
            if (Client == null)
            {
                Client = new WebSocket("wss://www.esssc.cc:8899/Laputa");
                Client.OnOpen +=
                    Client_OnOpen;
                Client.OnClose +=
                    Client_OnClose;
            }
        }

        private void Client_OnOpen(object sender, EventArgs e)
        {
            TextLogger.Write("Client connected", "wsTest");
        }

        private void Client_OnClose(object sender, CloseEventArgs e)
        {
            TextLogger.Write("Client closed", "wsTest");
        }

        public void Connect()
        {
            Client.Connect();
        }

        public bool IsOpen => Client.ReadyState == WebSocketState.Open;

        public void Send()
        {
            Client.Send("BALUS");
        }
    }
}