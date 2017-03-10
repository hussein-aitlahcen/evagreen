using System;
using System.Linq;
using System.Collections.Generic;
using System.Net;
using System.Threading.Tasks;
using System.Net.Sockets;
using EvaGreen.Common;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using EvaGreen.Server.Type;

namespace EvaGreen.Server
{
    public class Program
    {
        public const byte CMSG_CONFIGREQ = 0x0A;
        public const byte CMSG_UPLOADDATA = 0x0B;

        public const int PORT = 1337;
        public const int INPUT_BUFFER_LENGTH = 2048;

        public static void Main(string[] args)
        {
            Log("Initializing TcpServer");
            Run().Wait();
        }

        private async static Task Run()
        {
            var server = new TcpListener(IPAddress.Any, PORT);
            server.Start();
            Log("TcpServer initialized, starting listenning...");
            while (true)
            {
                var client = await server.AcceptTcpClientAsync();
                await Task.Factory.StartNew(async () => await Read(client));
            }
        }

        /*
            Read message comming from the client
        */
        private async static Task Read(TcpClient client)
        {
            Log("Client connected");

            using (var stream = client.GetStream())
            {
                var headerReceived = false;
                var packet = new List<byte>();
                var header = new NetHeader();
                var headerSize = Marshal.SizeOf<NetHeader>();
                var chunk = new byte[INPUT_BUFFER_LENGTH];
                var bytesRead = 0;
                var offset = 0;
                while ((bytesRead = await stream.ReadAsync(chunk, 0, chunk.Length)) > 0)
                {

                again:

                    // we obviously wait for the header first
                    if (!headerReceived)
                    {
                        var bytesLeft = bytesRead - offset;
                        if (bytesLeft < headerSize)
                            continue;
                        header = chunk
                                    .Skip(offset)
                                    .Take(headerSize)
                                    .ToArray()
                                    .ToStructure<NetHeader>();
                        offset += headerSize;
                        headerReceived = true;
                        Log($"Header: size={header.DataSize}, opcode={header.Opcode}");
                    }

                    // read the rest as packet data
                    while (offset < bytesRead && packet.Count < header.DataSize)
                    {
                        packet.Add(chunk[offset++]);
                    }

                    // if we fully received the payload, process the packet
                    if (packet.Count == header.DataSize)
                    {
                        await Process(stream, header, packet.ToArray());
                        packet.Clear();
                        headerReceived = false;
                        goto again;
                    }

                    offset = 0;
                }
            }

            client.Dispose();

            Log("Client disconnected");
        }

        private static async Task Process(NetworkStream output, NetHeader header, byte[] data)
        {
            Log($"Processing: opcode={header.Opcode}");

            using (var con = new DataConnection())
            {
                switch (header.Opcode)
                {
                    case CMSG_CONFIGREQ:
                        Log("Retrieving RemoteConfiguration");
                        var structure = new NetRemoteConfiguration
                        (
                            con.CreateDefaultAgentConfiguration()
                        ).ToByteArray();
                        await output.WriteAsync(structure, 0, structure.Length);
                        break;

                    case CMSG_UPLOADDATA:
                        con.Insert(CreateDataFromRaw(data));
                        break;
                }
            }
        }

        private static Data CreateDataFromRaw(byte[] data)
        {
            using (var reader = new BinaryReader(new MemoryStream(data)))
            {
                var agentId = reader.ReadInt32();
                var type = (DataType)reader.ReadByte();
                Log($"Data comming from agentId={agentId}, type={type}");
                switch (type)
                {
                    case DataType.Image:
                        var fileNameLength = reader.ReadInt32();
                        var fileName = Encoding.UTF8.GetString(reader.ReadBytes(fileNameLength).ToArray());
                        var payloadLength = reader.ReadInt32();
                        var payload = reader.ReadBytes(payloadLength);
                        var creationDate = reader.ReadInt64().ToDateTime();
                        var uniqueFileName = string.Format("{0}{1}", Guid.NewGuid().ToString(), Path.GetExtension(fileName));
                        var filePath = Path.Combine(DataConnection.DB_IMAGES_PATH, uniqueFileName);
                        File.WriteAllBytes(filePath, payload);
                        Log($"Received image: name={fileName}, id={uniqueFileName}, creation={creationDate.ToLocalTime().ToString()}, size={payloadLength}");
                        return new Data
                        {
                            AgentId = agentId,
                            Type = type,
                            CreationDate = creationDate.ToUnixTime(),
                            IntegrationDate = DateTime.Now.ToUnixTime(),
                            Value = uniqueFileName,
                            Description = fileName
                        };

                    case DataType.Temperature:
                        var value = reader.ReadInt32();
                        Log($"Received temperature={value}");
                        return new Data
                        {
                            AgentId = agentId,
                            Type = type,
                            CreationDate = DateTime.Now.ToUnixTime(),
                            IntegrationDate = DateTime.Now.ToUnixTime(),
                            Value = value.ToString(),
                            Description = "Celcius"
                        };

                    default:
                        Log("Unknow DataType : " + type);
                        throw new NotImplementedException();
                }
            }
        }

        private static void Log(String message)
        {
            Console.WriteLine(message);
        }
    }
}
