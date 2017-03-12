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

        public TableQuery<Data> Data => Table<Data>();
        public TableQuery<AgentConfiguration> AgentConfiguration => Table<AgentConfiguration>();

        public DataConnection() : base(DB_PATH)
        {
            CreateTable<Data>();
            CreateTable<AgentConfiguration>();
        }

        public AgentConfiguration GetOrCreateAgentConfiguration(int agentId)
        {
            var agentConf = AgentConfiguration.FirstOrDefault(c => c.AgentId == agentId);
            if (agentConf == null)
            {
                agentConf = new AgentConfiguration();
            }
            InsertOrReplace(agentConf);
            return agentConf;
        }
    }
}