using System;
using System.Linq;
using System.Collections.Generic;
using System.Net;
using System.Threading.Tasks;
using System.Net.Sockets;
using EvaGreen.Common;
using SQLite;
using System.Runtime.InteropServices;

namespace EvaGreen.Server
{
    public class Program
    {
        public const int PORT = 1337;
        public const int INPUT_BUFFER_LENGTH = 1024;

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
                Read(await server.AcceptTcpClientAsync());
            }
        }


        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct NetHeader
        {
            public int DataSize;
            public byte Opcode;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct RemoteConfiguration
        {
            public long UploadInterval;
            public long SnapshotInterval;
            public uint WidthResolution;
            public uint HeightResolution;
        }

        private static T ByteArrayToStructure<T>(byte[] bytes) where T : struct
        {
            GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            T stuff = Marshal.PtrToStructure<T>(handle.AddrOfPinnedObject());
            handle.Free();
            return stuff;
        }

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

        private async static Task Read(TcpClient client)
        {
            Console.WriteLine("Client connected.");

            client.SendBufferSize = INPUT_BUFFER_LENGTH;
            client.ReceiveBufferSize = INPUT_BUFFER_LENGTH;

            using (var stream = client.GetStream())
            {
                var packet = new List<byte>();
                var header = new NetHeader
                {
                    DataSize = -1,
                    Opcode = 0,
                };
                var headerSize = Marshal.SizeOf<NetHeader>();
                var chunk = new byte[INPUT_BUFFER_LENGTH];
                var bytesRead = 0;
                var offset = 0;
                while ((bytesRead = await stream.ReadAsync(chunk, 0, chunk.Length)) > 0)
                {
                    Console.WriteLine($"Received {bytesRead} bytes.");

                again:
                    if (header.DataSize == -1)
                    {
                        if (bytesRead - offset < headerSize)
                            continue;
                        header = ByteArrayToStructure<NetHeader>(chunk.Skip(offset).Take(headerSize).ToArray());
                        offset += headerSize;

                        Console.WriteLine($"Header: size={header.DataSize}, opcode={header.Opcode}");
                    }

                    while (offset < bytesRead && packet.Count < header.DataSize)
                    {
                        packet.Add(chunk[offset++]);
                    }

                    if (packet.Count >= header.DataSize - headerSize)
                    {
                        await Process(stream, header, packet.ToArray());
                        packet.Clear();
                        header = new NetHeader
                        {
                            DataSize = -1,
                            Opcode = 0,
                        };
                        goto again;
                    }

                    offset = 0;
                }
            }

            Console.WriteLine("Client disconnected.");
        }

        private static async Task Process(NetworkStream output, NetHeader header, byte[] data)
        {
            Console.WriteLine($"Processing: opcode={header.Opcode}, real size={data.Length}.");

            switch (header.Opcode)
            {
                case 0x0A:
                    var structure = StructureToByteArray(new RemoteConfiguration
                    {
                        UploadInterval = 3600 * 6,
                        SnapshotInterval = 1800,
                        WidthResolution = 1024,
                        HeightResolution = 1024
                    });
                    await output.WriteAsync(structure, 0, structure.Length);
                    break;

                case 0x0B:
                    var type = (DataType)data[0];
                    Console.WriteLine($"Received data: type={type}");
                    switch (type)
                    {
                        
                        case DataType.Temperature:
                            var value = BitConverter.ToInt32(data, 1);
                            Console.WriteLine($"Received temperature={value}, raw=[{string.Join(", ", data.Skip(1).Select(x => x.ToString("x2")))}]");
                            break;
                    }
                    break;
            }
        }
    }
}
