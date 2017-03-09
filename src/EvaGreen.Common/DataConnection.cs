using System;
using System.IO;
using SQLite;

namespace EvaGreen.Common
{
    public sealed class DataConnection : SQLiteConnection
    {
        public static string DB_DIR = Path.Combine(Environment.GetEnvironmentVariable("HOME"), ".evagreen/");
        public static string DB_PATH = Path.Combine(DB_DIR, "data.db");
        public static string DB_IMAGES_PATH = Path.Combine(DB_DIR, "images");

        static DataConnection()
        {
            if (!Directory.Exists(DB_DIR))
                Directory.CreateDirectory(DB_DIR);
            if (!Directory.Exists(DB_IMAGES_PATH))
                Directory.CreateDirectory(DB_IMAGES_PATH);
        }

        public TableQuery<Data> Table => Table<Data>();

        public DataConnection() : base(DB_PATH)
        {
            CreateTable<Data>();
        }
    }
}