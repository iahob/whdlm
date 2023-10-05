
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Game.Utils;
using WebSocketSharp;
using WebSocketSharp.Server;

namespace Game.Socket.Service
{
    public class Laputa : WebSocketBehavior
    {
        protected override void OnMessage(MessageEventArgs e)
        {
            var msg = e.Data == "BALUS"
                ? "I've been balused already..."
                : "I'm not available now.";

            Send(msg);
        }
    }
}
