using SQLite;

namespace EvaGreen.Common
{
    public sealed class AgentConfiguration
    {
        public const int DEFAULT_WIDTH = 640;
        public const int DEFAULT_HEIGHT = 480;
        public const long DEFAULT_UPLOAD_INTERVAL = 0;
        public const long DEFAULT_SNAPSHOT_INTERVAL = 0;

        public AgentConfiguration()
        {
            UploadInterval = DEFAULT_UPLOAD_INTERVAL;
            SnapshotInterval = DEFAULT_SNAPSHOT_INTERVAL;
            WidthResolution = DEFAULT_WIDTH;
            HeightResolution = DEFAULT_HEIGHT;
            InitialContact = true;
        }

        [PrimaryKey]
        public int Id { get; set; }

        [MaxLength(512)]
        public string Description { get; set; }

        public long UploadInterval { get; set; }

        public long SnapshotInterval { get; set; }

        public int WidthResolution { get; set; }

        public int HeightResolution { get; set; }

        public bool InitialContact { get; set; }

        // TODO: GEOLOCALISATION
    }
}