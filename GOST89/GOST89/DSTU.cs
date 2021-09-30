using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GOST89
{
    public class DSTU
    {
        public static byte[] DefaultSBox => ConvertHelper.HexStringToByteArray("0102030E060D0B080F0A0C050709000403080B0506040E0A020C0107090F0D0002080907050F000B0C010D0E0A0306040F080E090702000D0C0601050B04030A03080D09060B0F0002050C0A040E01070F0605080E0B0A040C0003070209010D08000C040906070B0203010F050E0A0D0A090D060E0B04050F01030C07000802");

        public static byte[] PackBox(byte[] input)
        {
            var res = new byte[input.Length / 2];
            var rows = input.Length & 0xF0;
            for (var idx = 0; idx < input.Length; idx += 2)
            {
                var retIdx = (rows - 0x10 - (idx & 0xF0)) | idx & 0x0F;
                res[retIdx >> 1] = (byte)(input[idx] << 4 | input[idx + 1]);
            }
            return res;
        }
    }
}
