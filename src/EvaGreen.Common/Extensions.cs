using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace EvaGreen.Common
{
    public static class Extensions
    {

        /*
            Marshal back given byte array into a structure
        */
        public static T ToStructure<T>(this IEnumerable<byte> bytes) where T : struct
        {
            var arr = bytes.ToArray();
            var handle = GCHandle.Alloc(arr, GCHandleType.Pinned);
            var stuff = Marshal.PtrToStructure<T>(handle.AddrOfPinnedObject());
            handle.Free();
            return stuff;
        }

        /*
            Marshal given struct instance into its byte array representation
        */
        public static byte[] ToByteArray<T>(this T obj) where T : struct
        {
            var len = Marshal.SizeOf(obj);
            var arr = new byte[len];
            var ptr = Marshal.AllocHGlobal(len);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, arr, 0, len);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }

        /*
            Transform a unix timestamp into a .net date
         */
        public static DateTime ToDateTime(this long unixTime)
        {
            var epoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
            return epoch.AddSeconds(unixTime);
        }

        /*
            Transform a .net date into a unix timestamp 
         */
        public static long ToUnixTime(this DateTime date)
        {
            var epoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
            return Convert.ToInt64((date - epoch).TotalSeconds);
        }
    }
}