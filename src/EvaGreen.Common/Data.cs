using SQLite;

namespace EvaGreen.Common
{
    public sealed class Data
    {

        [AutoIncrement, PrimaryKey]
        public int Id { get; set; }

        public DataType Type { get; set; }

        public long CreationDate { get; set; }

        public long IntegrationDate { get; set; }

        [MaxLength(256)]
        public string Value { get; set; }

        [MaxLength(256)]
        public string Description { get; set; }
    }
}