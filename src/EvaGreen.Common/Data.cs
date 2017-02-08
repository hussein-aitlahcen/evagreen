using System;
using System.Linq;
using System.Collections.Generic;
using SQLite;

namespace EvaGreen.Common
{
    public sealed class Data
    {
        public const string FILE_PATH = "~/.evagreen/data.db";

        [AutoIncrement, PrimaryKey]
        public int Id { get; set; }

        public DataType Type { get; set; }

        public DateTime CreationDate { get; set; }

        public DateTime IntegrationDate { get; set; }

        [MaxLength(256)]
        public string Value { get; set; }
    }
}