using SQLite;

namespace EvaGreen.Common
{
    public sealed class AgentConfiguration
    {
        public const int DEFAULT_WIDTH = 640;
        public const int DEFAULT_HEIGHT = 480;
        public const long DEFAULT_UPLOAD_INTERVAL = 1000 * 60 * 60 * 12;
        public const long DEFAULT_SNAPSHOT_INTERVAL = 1000 * 60 * 5;

        public AgentConfiguration()
        {
            UploadInterval = DEFAULT_UPLOAD_INTERVAL;
            SnapshotInterval = DEFAULT_SNAPSHOT_INTERVAL;
            WidthResolution = DEFAULT_WIDTH;
            HeightResolution = DEFAULT_HEIGHT;
        }

        [PrimaryKey]
        public int AgentId { get; set; }

        [MaxLength(512)]
        public string AgentDescription { get; set; }

        public long UploadInterval { get; set; }

        public long SnapshotInterval { get; set; }

        public uint WidthResolution { get; set; }

        public uint HeightResolution { get; set; }

        public bool InitialContact { get; set; }

        // TODO: GEOLOCALISATION
    }
}