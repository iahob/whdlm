using System.Security.Cryptography.X509Certificates;
using WebSocketSharp.Server;

namespace Game.Socket.Service
{
    public class SocketService
    {
        public WebSocketServer Server { get; set; }

        public SocketService()
        {
            if (Server == null)
            {
                Server = new WebSocketServer(8899, true);
                Server.SslConfiguration.ServerCertificate =
                    new X509Certificate2(@"E:\\svn\\QJ\\普通\\Common\\Game.Socket\\Service\\www.esssc.cc.pfx", "407043");
                Server.AddWebSocketService<Laputa>("/Laputa");
            }
        }

        public void Start()
        {
            Server.Start();
        }

        public void Stop()
        {
            Server.Stop();
        }
    }
}