using System;
using System.Linq;
using System.Collections.Generic;
using SQLite;

namespace EvaGreen.Common
{
    public enum DataType
    {
        Photo = 0x01,
        Temperature = 0x02,
        Video = 0xFF
    }
}