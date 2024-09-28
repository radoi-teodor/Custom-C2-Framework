using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TeamServer
{
    internal class BinaryUtils
    {
        public static byte[] getShellcodeFromBinary(String path)
        {
            
            int chunkSize = 8192;

            byte[] content = File.ReadAllBytes(path);

            return content;

        }
    }
}
