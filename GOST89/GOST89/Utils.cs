using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GOST89
{
    public static class Utils
    {
        public static byte[] Dumb_KDF(byte[] input, int n_passes)
        {
            var hasher = new Gost89Hash();
            hasher.Update(input);
            var hash = hasher.Finish();
            n_passes--;
            while (n_passes-- != 0)
            {
                hasher.Reset();
                hasher.Update32(hash);
                hash = hasher.Finish();
            }

            return hash;
        }

        public static byte[] PB_KDF(byte[] input, byte[] salt, int iterations)
        {
            var key = new byte[32];
            var pw_pad36 = new byte[32];
            var pw_pad5C = new byte[32];
            var hasher = new Gost89Hash();
            var ins = new byte[4] { 0,0,0,1 };

            for (var k = 0; k < 32; k++)
            {
                pw_pad36[k] = 0x36;
                pw_pad5C[k] = 0x5C;
            }
            for (var k=0; k < input.Length; k++) {
                pw_pad36[k] ^= input[k];
                pw_pad5C[k] ^= input[k];
            }
            hasher.Update32(pw_pad36);
            hasher.Update(salt);
            hasher.Update(ins);
            var hash = hasher.Finish();

            hasher.Reset();

            hasher.Update32(pw_pad5C);
            hasher.Update32(hash);
            hash = hasher.Finish();

            iterations--;

            for (var k = 0; k < 32; k++)
            {
                key[k] = hash[k];
            }

            while (iterations-- > 0)
            {
                hasher.Reset();
                hasher.Update32(pw_pad36);
                hasher.Update32(hash);
                hash = hasher.Finish();

                hasher.Reset();
                hasher.Update32(pw_pad5C);
                hasher.Update32(hash);
                hash = hasher.Finish();

                for (var k = 0; k < 32; k++)
                {
                    key[k] ^= hash[k];
                }
            }

            return key;
        }
    }
}
