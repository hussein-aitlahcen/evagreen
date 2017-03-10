using SQLite;

namespace EvaGreen.Common
{
    public sealed class AgentConfiguration
    {
        [AutoIncrement, PrimaryKey]
        public int AgentId { get; set; }

        [MaxLength(512)]
        public string AgentDescription { get; set; }

        public long UploadInterval { get; set; }

        public long SnapshotInterval { get; set; }

        public uint WidthResolution { get; set; }

        public uint HeightResolution { get; set; }

        // TODO: GEOLOCALISATION
    }
}