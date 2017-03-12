using System.Runtime.InteropServices;

namespace EvaGreen.Server.Type
{
    /*
        Network message header
    */
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct NetHeader
    {
        public int DataSize;
        public byte OpCode;
        public int AgentId;
    }
}