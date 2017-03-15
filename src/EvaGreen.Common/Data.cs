using SQLite;

namespace EvaGreen.Common
{
    public sealed class Data
    {
        public const int STRING_MAX_SIZE = 256;

        [PrimaryKey, AutoIncrement]
        public int Id { get; set; }

        [Indexed]
        public int AgentId { get; set; }

        public DataType Type { get; set; }

        public long CreationDate { get; set; }

        public long IntegrationDate { get; set; }

        [MaxLength(STRING_MAX_SIZE)]
        public string Value { get; set; }

        [MaxLength(STRING_MAX_SIZE)]
        public string Description { get; set; }
    }
}