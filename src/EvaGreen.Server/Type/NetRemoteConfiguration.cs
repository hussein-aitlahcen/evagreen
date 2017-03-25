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
            UploadInterval = conf.UploadInterval;
            SnapshotInterval = conf.SnapshotInterval;
            WidthResolution = conf.WidthResolution;
            HeightResolution = conf.HeightResolution;
            InitialContact = conf.InitialContact;
        }
        public long UploadInterval;
        public long SnapshotInterval;
        public int WidthResolution;
        public int HeightResolution;
        public bool InitialContact;
    }
}