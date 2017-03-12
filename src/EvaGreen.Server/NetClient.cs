using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using EvaGreen.Server.Type;
using EvaGreen.Common;
using System.Text;
using System.IO;
using System;

namespace EvaGreen.Server
{
    public sealed class NetClient
    {
        private const byte CMSG_CONFIGREQ = 0x0A;
        private const byte CMSG_UPLOADDATA = 0x0B;
        private const int INPUT_BUFFER_LENGTH = 2048;

        private TcpClient _client;
        private List<byte> _message;
        private NetHeader _header;
        private NetClientState _state;
        private int _offset;
        private byte[] _buffer;

        public NetClient(TcpClient client)
        {
            _client = client;
            _message = new List<byte>();
            _state = NetClientState.WaitingHeader;
            _offset = 0;
            _buffer = new byte[INPUT_BUFFER_LENGTH];
        }

        public async Task Read()
        {
            var stream = _client.GetStream();
            var bytesRead = await _client.GetStream().ReadAsync(_buffer, 0, _buffer.Length);
            if (bytesRead > 0)
            {
                var chunk = false;
                var offset = 0;
                _message.AddRange(_buffer.Take(bytesRead));
                while (!chunk && offset != bytesRead)
                {
                    var lastOffset = offset;
                    switch (_state)
                    {
                        case NetClientState.WaitingHeader:
                            var headerSize = Marshal.SizeOf<NetHeader>();
                            if (_message.Count >= headerSize)
                            {
                                Log("Received header");
                                _state = NetClientState.WaitingMessage;
                                _header = _message
                                    .Take(headerSize)
                                    .ToStructure<NetHeader>();
                                offset += headerSize;

                                /*
                                    There is a special case when a message withouth payload 
                                    is comming. We should handle it right now
                                */
                                if (_header.DataSize == 0)
                                {
                                    _state = NetClientState.WaitingHeader;
                                    await Process(new byte[0]);
                                }
                            }
                            else
                            {
                                chunk = true;
                            }
                            break;

                        case NetClientState.WaitingMessage:
                            var bytesLeft = _message.Count;
                            if (bytesLeft >= _header.DataSize)
                            {
                                _state = NetClientState.WaitingHeader;
                                Log($"Received body: Size={_header.DataSize}");
                                var data = _message
                                        .Take(_header.DataSize)
                                        .ToArray();
                                await Process(data);
                                offset += data.Length;
                            }
                            else
                            {
                                chunk = true;
                            }
                            break;
                    }
                    var used = offset - lastOffset;
                    if (used > 0)
                    {
                        Log($"Flushing processed data, Used={used}");
                        _message.RemoveRange(0, used);
                    }
                }

                await Task.Factory.StartNew(Read);
            }
            else
            {
                _client.Dispose();
                Log("Client disconnected");
            }
        }

        private async Task Process(byte[] data)
        {
            var output = _client.GetStream();

            Log($"Processing: OpCode={_header.OpCode}");

            using (var con = new DataConnection())
            {
                switch (_header.OpCode)
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

        private Data CreateDataFromRaw(byte[] data)
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