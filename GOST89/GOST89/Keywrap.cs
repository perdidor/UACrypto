using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GOST89
{
    class Keywrapper
    {
        static readonly byte[] WRAP_IV = new byte[8] { 0x4a, 0xdd, 0xa2, 0x2c, 0x79, 0xe8, 0x21, 0x05 };

        public static byte[] KeyWrap(byte[] cek, byte[] kek, byte[] iv)
        {
            var gost = new gost89();
            var icv = new byte[40];
            var cekicv = new byte[40];
            var temp2 = new byte[44];
            var temp3 = new byte[48];
            gost.Key(kek);
            gost.Mac(32, cek, ref icv);
            for (var idx = 0; idx < 32; idx++)
            {
                cekicv[idx] = cek[idx];
            }
            for (var idx = 32; idx < 40; idx++)
            {
                cekicv[idx] = icv[idx - 32];
            }
            var temp1 = gost.Crypt_CFB(iv, cekicv);
            for (var idx  = 0; idx < 8; idx++)
            {
                temp2[idx] = iv[idx];
            }
            for (var idx  = 8; idx < 44; idx++)
            {
                temp2[idx] = temp1[idx - 8];
            }

            for (var idx  = 0; idx < 48; idx++)
            {
                temp3[idx] = temp2[44 - idx - 1];
            }
            var res = gost.Crypt_CFB(WRAP_IV, temp3);
            return res.Take(44).ToArray();
        }

        public static byte[] KeyUnwrap(byte[] wcek, byte[] kek)
        {
            var gost = new gost89();
            var icv = new byte[4];
            var iv = new byte[8];
            var temp1 = new byte[40];
            var temp2 = new byte[44];
            byte[] icv_check = new byte[] { };
            gost.Key(kek);
            var temp3 = gost.Decrypt_CFB(WRAP_IV, wcek);
            for (var idx = 0; idx < 44; idx++)
            {
                temp2[idx] = temp3[44 - idx - 1];
            }
            for (var idx = 0; idx < 8; idx++)
            {
                iv[idx] = temp2[idx];
            }
            for (var idx = 0; idx < 36; idx++)
            {
                temp1[idx] = temp2[idx + 8];
            }
            var cekicv = gost.Decrypt_CFB(iv, temp1);
            for (var idx = 0; idx < 4; idx++)
            {
                icv[idx] = cekicv[idx + 32];
            }
            gost.Mac(32, cekicv.Take(32).ToArray(), ref icv_check);
            var err = icv[0] ^ icv_check[0];
            err |= icv[1] ^ icv_check[1];
            err |= icv[2] ^ icv_check[2];
            err |= icv[3] ^ icv_check[3];

            if (err != 0)
            {
                throw new Exception("Key unwrap failed. Checksum mismatch");
            }

            return cekicv.Take(32).ToArray();
        }

    }
}
