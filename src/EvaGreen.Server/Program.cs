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
            Network message header
        */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct NetHeader
        {
            public uint DataSize;
            public byte Opcode;
        }

        /*
            Remote configuration structure
        */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct RemoteConfiguration
        {
            public long UploadInterval;
            public long SnapshotInterval;
            public uint WidthResolution;
            public uint HeightResolution;
        }

        /*
            Marshal back given byte array into a structure
        */
        private static T ByteArrayToStructure<T>(byte[] bytes) where T : struct
        {
            GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            T stuff = Marshal.PtrToStructure<T>(handle.AddrOfPinnedObject());
            handle.Free();
            return stuff;
        }

        /*
            Marshal given struct instance into its byte array representation
        */
        private static byte[] StructureToByteArray<T>(T obj) where T : struct
        {
            var len = Marshal.SizeOf(obj);
            byte[] arr = new byte[len];
            IntPtr ptr = Marshal.AllocHGlobal(len);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, arr, 0, len);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }

        /*
            Transform a unix timestamp into a .net date
         */
        public static DateTime FromUnixTime(long unixTime)
        {
            var epoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
            return epoch.AddSeconds(unixTime);
        }

        /*
            Transform a .net date into a unix timestamp 
         */
        public static long ToUnixTime(DateTime date)
        {
            var epoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
            return Convert.ToInt64((date - epoch).TotalSeconds);
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
                        header = ByteArrayToStructure<NetHeader>(chunk.Skip(offset).Take(headerSize).ToArray());
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
                    if (packet.Count >= header.DataSize - headerSize)
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

            switch (header.Opcode)
            {
                case CMSG_CONFIGREQ:
                    Log("Retrieving RemoteConfiguration");
                    var structure = StructureToByteArray(new RemoteConfiguration
                    {
                        UploadInterval = 3600 * 6,
                        SnapshotInterval = 1800,
                        WidthResolution = 1024,
                        HeightResolution = 1024
                    });
                    await output.WriteAsync(structure, 0, structure.Length);
                    break;

                case CMSG_UPLOADDATA:
                    using (var connection = new DataConnection())
                    {
                        connection.Insert(CreateDataFromRaw(data));
                    }
                    break;
            }
        }

        private static Data CreateDataFromRaw(byte[] data)
        {
            using (var reader = new BinaryReader(new MemoryStream(data)))
            {
                var type = (DataType)reader.ReadByte();
                switch (type)
                {
                    case DataType.Image:
                        var fileNameLength = reader.ReadInt32();
                        var fileName = Encoding.UTF8.GetString(reader.ReadBytes(fileNameLength).ToArray());
                        var payloadLength = reader.ReadInt32();
                        var payload = reader.ReadBytes(payloadLength);
                        var creationDate = FromUnixTime(reader.ReadInt64());
                        var uniqueFileName = Guid.NewGuid().ToString();
                        var filePath = Path.Combine(DataConnection.DB_IMAGES_PATH, uniqueFileName);
                        File.WriteAllBytes(filePath, payload);
                        Log($"Received image: name={fileName}, id={uniqueFileName}, creation={creationDate.ToLocalTime().ToString()}, size={payloadLength}");
                        return new Data
                        {
                            Type = type,
                            CreationDate = ToUnixTime(creationDate),
                            IntegrationDate = ToUnixTime(DateTime.Now),
                            Value = uniqueFileName,
                            Description = fileName
                        };

                    case DataType.Temperature:
                        var value = reader.ReadInt32();
                        Log($"Received temperature={value}");
                        throw new NotImplementedException();

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
