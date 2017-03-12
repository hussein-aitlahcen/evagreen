using System.Net;
using System.Threading.Tasks;
using System.Net.Sockets;

namespace EvaGreen.Server
{
    public class Program
    {

        public const int PORT = 1337;

        public static void Main(string[] args)
        {
            Run().Wait();
        }

        private async static Task Run()
        {
            var server = new TcpListener(IPAddress.Any, PORT);
            server.Start();
            while (true)
            {
                var socket = await server.AcceptTcpClientAsync();
                var client = new NetClient(socket);
                await Task.Factory.StartNew(client.Read);
            }
        }
    }
}
