using System.Runtime.InteropServices;
using EvaGreen.Common;

namespace EvaGreen.Server.Type
{
    /*
        Remote configuration structure
    */
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct NetRemoteConfiguration
    {
        public NetRemoteConfiguration(AgentConfiguration conf)
        {
            AgentId = conf.AgentId;
            UploadInterval = conf.UploadInterval;
            SnapshotInterval = conf.SnapshotInterval;
            WidthResolution = conf.WidthResolution;
            HeightResolution = conf.HeightResolution;
            InitialContact = conf.InitialContact;
        }
        public int AgentId;
        public long UploadInterval;
        public long SnapshotInterval;
        public uint WidthResolution;
        public uint HeightResolution;
        public bool InitialContact;
    }
}