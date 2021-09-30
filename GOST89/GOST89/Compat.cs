using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GOST89
{
    public class Compat
    {
        public static byte[] ConvertPassword(byte[] pw)
        {
            return Utils.Dumb_KDF(pw, 10000);
        }

        public static byte[] DecodeData(int bodylen, byte[] parsedwithpad, byte[] pw)
        {
            var gost = new gost89();
            var bkey = ConvertPassword(pw);
            File.WriteAllBytes("bkey.txt", bkey);
            gost.Key(bkey);
            var res = gost.Decrypt(parsedwithpad);
            return res.Take(bodylen).ToArray();
        }
    }
}
