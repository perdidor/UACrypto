using System;
using System.IO;
using System.Linq;

namespace GOST89
{
    public class gost89
    {
        public uint[] k = new uint[8];

        public uint[] k1 = new uint[16];
        public uint[] k2 = new uint[16];
        public uint[] k3 = new uint[16];
        public uint[] k4 = new uint[16];
        public uint[] k5 = new uint[16];
        public uint[] k6 = new uint[16];
        public uint[] k7 = new uint[16];
        public uint[] k8 = new uint[16];

        public uint[] k87 = new uint[256];
        public uint[] k65 = new uint[256];
        public uint[] k43 = new uint[256];
        public uint[] k21 = new uint[256];

        public uint[] n = new uint[2];

        public byte[] gamma = new byte[8];

        public gost89(byte[] sBox = null)
        {
            if (sBox == null)
            {
                sBox = DSTU.DefaultSBox;
            }

            var mem = new uint[1162];

            k = mem.Take(8).ToArray();
            k87 = mem.Skip(8).Take(256).ToArray();
            k65 = mem.Skip(264).Take(256).ToArray();
            k43 = mem.Skip(520).Take(256).ToArray();
            k21 = mem.Skip(776).Take(256).ToArray();

            sBox.CopyTo(mem, 1032);

            k1 = mem.Skip(1144).Take(16).ToArray();
            k2 = mem.Skip(1128).Take(16).ToArray();
            k3 = mem.Skip(1112).Take(16).ToArray();
            k4 = mem.Skip(1096).Take(16).ToArray();
            k5 = mem.Skip(1080).Take(16).ToArray();
            k6 = mem.Skip(1064).Take(16).ToArray();
            k7 = mem.Skip(1048).Take(16).ToArray();
            k8 = mem.Skip(1032).Take(16).ToArray();

            n = mem.Skip(1160).Take(2).ToArray();

            BoxInit();
        }

        public void BoxInit()
        {
            for (int i = 0; i < 256; i++)
            {
                k87[i] = (k8[UnsignedRightShift(i, 4)] << 4 | k7[i & 15]) << 24;
                k65[i] = (k6[UnsignedRightShift(i, 4)] << 4 | k5[i & 15]) << 16;
                k43[i] = (k4[UnsignedRightShift(i, 4)] << 4 | k3[i & 15]) << 8;
                k21[i] = (k2[UnsignedRightShift(i, 4)] << 4 | k1[i & 15]);
            }
        }

        public uint Pass(uint x)
        {
            x = k87[(x >> 24) & 255] | k65[(x >> 16) & 255] | k43[(x >> 8) & 255] | k21[x & 255];
            return x << 11 | (x >> 21);
        }

        public static int UnsignedRightShift(int signed, int places)
        {
            unchecked // just in case of unusual compiler switches; this is the default
            {
                var unsigned = (uint)signed;
                unsigned >>= places;
                return (int)unsigned;
            }
        }

        public void Crypt64(byte[] clear, ref byte[] outres)
        {
            var n = this.n;

            n[0] = (uint)(clear[0]|(clear[1]<<8)|(clear[2]<<16)|(clear[3]<<24));
            n[1] = (uint)(clear[4]|(clear[5]<<8)|(clear[6]<<16)|(clear[7]<<24));
            /* Instead of swappclearg halves, swap names each round */

            n[1] ^= Pass(n[0] + k[0]); n[0] ^= Pass(n[1]+k[1]);
            n[1] ^= Pass(n[0]+k[2]); n[0] ^= Pass(n[1]+k[3]);
            n[1] ^= Pass(n[0]+k[4]); n[0] ^= Pass(n[1]+k[5]);
            n[1] ^= Pass(n[0]+k[6]); n[0] ^= Pass(n[1]+k[7]);

            n[1] ^= Pass(n[0]+k[0]); n[0] ^= Pass(n[1]+k[1]);
            n[1] ^= Pass(n[0]+k[2]); n[0] ^= Pass(n[1]+k[3]);
            n[1] ^= Pass(n[0]+k[4]); n[0] ^= Pass(n[1]+k[5]);
            n[1] ^= Pass(n[0]+k[6]); n[0] ^= Pass(n[1]+k[7]);

            n[1] ^= Pass(n[0]+k[0]); n[0] ^= Pass(n[1]+k[1]);
            n[1] ^= Pass(n[0]+k[2]); n[0] ^= Pass(n[1]+k[3]);
            n[1] ^= Pass(n[0]+k[4]); n[0] ^= Pass(n[1]+k[5]);
            n[1] ^= Pass(n[0]+k[6]); n[0] ^= Pass(n[1]+k[7]);

            n[1] ^= Pass(n[0]+k[7]); n[0] ^= Pass(n[1]+k[6]);
            n[1] ^= Pass(n[0]+k[5]); n[0] ^= Pass(n[1]+k[4]);
            n[1] ^= Pass(n[0]+k[3]); n[0] ^= Pass(n[1]+k[2]);
            n[1] ^= Pass(n[0]+k[1]); n[0] ^= Pass(n[1]+k[0]);

            outres[0] = (byte)(n[1] & 0xff); outres[1] = (byte)((n[1] >> 8) & 0xff);
            outres[2] = (byte)((n[1] >> 16) & 0xff); outres[3] = (byte)(n[1] >> 24);
            outres[4] = (byte)(n[0] & 0xff); outres[5] = (byte)((n[0] >> 8) & 0xff);
            outres[6] = (byte)((n[0] >> 16) & 0xff); outres[7] = (byte)(n[0] >> 24);
        }

        public void Decrypt64(byte[] crypt, ref byte[] outres)
        {
            var n = this.n;
            n[0] = (uint)(crypt[0] | (crypt[1] << 8) | (crypt[2] << 16) | (crypt[3] << 24));
            n[1] = (uint)(crypt[4] | (crypt[5] << 8) | (crypt[6] << 16) | (crypt[7] << 24));

            n[1] ^= Pass(n[0] + k[0]); n[0] ^= Pass(n[1] + k[1]);
            n[1] ^= Pass(n[0] + k[2]); n[0] ^= Pass(n[1] + k[3]);
            n[1] ^= Pass(n[0] + k[4]); n[0] ^= Pass(n[1] + k[5]);
            n[1] ^= Pass(n[0] + k[6]); n[0] ^= Pass(n[1] + k[7]);

            n[1] ^= Pass(n[0] + k[7]); n[0] ^= Pass(n[1] + k[6]);
            n[1] ^= Pass(n[0] + k[5]); n[0] ^= Pass(n[1] + k[4]);
            n[1] ^= Pass(n[0] + k[3]); n[0] ^= Pass(n[1] + k[2]);
            n[1] ^= Pass(n[0] + k[1]); n[0] ^= Pass(n[1] + k[0]);

            n[1] ^= Pass(n[0] + k[7]); n[0] ^= Pass(n[1] + k[6]);
            n[1] ^= Pass(n[0] + k[5]); n[0] ^= Pass(n[1] + k[4]);
            n[1] ^= Pass(n[0] + k[3]); n[0] ^= Pass(n[1] + k[2]);
            n[1] ^= Pass(n[0] + k[1]); n[0] ^= Pass(n[1] + k[0]);

            n[1] ^= Pass(n[0] + k[7]); n[0] ^= Pass(n[1] + k[6]);
            n[1] ^= Pass(n[0] + k[5]); n[0] ^= Pass(n[1] + k[4]);
            n[1] ^= Pass(n[0] + k[3]); n[0] ^= Pass(n[1] + k[2]);
            n[1] ^= Pass(n[0] + k[1]); n[0] ^= Pass(n[1] + k[0]);

            outres[0] = (byte)(n[1] & 0xff); outres[1] = (byte)((n[1] >> 8) & 0xff);
            outres[2] = (byte)((n[1] >> 16) & 0xff); outres[3]= (byte)(n[1] >> 24);
            outres[4] = (byte)(n[0] & 0xff); outres[5] = (byte)((n[0] >> 8) & 0xff);
            outres[6] = (byte)((n[0] >> 16) & 0xff); outres[7] = (byte)(n[0] >> 24);

        }

        public byte[] Crypt64_CFB(byte[] iv, byte[] plain)
        {
            var resarray = new byte[8];
            var gamma = this.gamma;
            Crypt64(iv, ref gamma);
            for (int i = 0; i < 8; i++)
            {
                resarray[i] = (byte)(plain[i] ^ gamma[i]);
                iv[i] = resarray[i];
            }
            return resarray;
        }

        public byte[] Decrypt64_CFB(byte[] iv, byte[] crypted)
        {
            var resarray = new byte[8];
            var gamma = this.gamma;
            Crypt64(iv, ref gamma);
            for (int i = 0; i < 8; i++)
            {
                iv[i] = crypted[i];
                resarray[i] = (byte)(crypted[i] ^ gamma[i]);
            }
            return resarray;
        }

        public byte[] Crypt_CFB(byte[] iv, byte[] plain)
        {
            gamma = new byte[8];
            var blocks = Math.Ceiling((decimal)plain.Length / 8);
            if (blocks == 0)
            {
                return null;
            }
            var resarray = new byte[(int)blocks * 8];
            for (int i = 0; i < blocks; i++)
            {
                var restplain = plain.Skip(i * 8).ToArray();
                if (restplain.Length < 8)
                {
                    Array.Resize(ref restplain, 8);
                }
                var restblock = restplain.Take(8).ToArray();
                var cryptedres = Crypt64_CFB(iv, restblock);
                cryptedres.CopyTo(resarray, i * 8);
            }
            if (resarray.Length != plain.Length)
            {
                Array.Resize(ref resarray, plain.Length);
            }
            return resarray;
        }

        public byte[] Decrypt_CFB(byte[] iv, byte[] crypted)
        {
            gamma = new byte[8];
            var blocks = Math.Ceiling((decimal)crypted.Length / 8);
            if (blocks == 0)
            {
                return null;
            }
            var resarray = new byte[(int)blocks * 8];
            for (int i = 0; i < blocks; i++)
            {
                var restcrypted = crypted.Skip(i * 8).ToArray();
                if (restcrypted.Length < 8)
                {
                    Array.Resize(ref restcrypted, 8);
                }
                var restblock = restcrypted.Take(8).ToArray();
                var cryptedres = Decrypt64_CFB(iv, restblock);
                cryptedres.CopyTo(resarray, i * 8);
            }
            if (resarray.Length != crypted.Length)
            {
                Array.Resize(ref resarray, crypted.Length);
            }
            return resarray;
        }

        public byte[] Crypt(byte[] iv, byte[] plain)
        {
            var blocks = Math.Ceiling((decimal)plain.Length / 8);
            if (blocks == 0)
            {
                return null;
            }
            var resarray = new byte[(int)blocks * 8];
            for (int i = 0; i < blocks; i++)
            {
                var restplain = plain.Skip(i * 8).ToArray();
                if (restplain.Length < 8)
                {
                    Array.Resize(ref restplain, 8);
                }
                var restblock = restplain.Take(8).ToArray();
                var cryptedres = new byte[8];
                Decrypt64(restblock, ref cryptedres);
                cryptedres.CopyTo(resarray, i * 8);
            }
            if (resarray.Length != plain.Length)
            {
                Array.Resize(ref resarray, plain.Length);
            }
            return resarray;
        }

        public byte[] Decrypt(byte[] crypted)
        {
            var blocks = Math.Ceiling((decimal)crypted.Length / 8);
            if (blocks == 0)
            {
                return null;
            }
            var resarray = new byte[(int)blocks * 8];
            for (int i = 0; i < blocks; i++)
            {
                var restcrypted = crypted.Skip(i * 8).ToArray();
                if (restcrypted.Length < 8)
                {
                    Array.Resize(ref restcrypted, 8);
                }
                var restblock = restcrypted.Take(8).ToArray();
                var plainres = new byte[8];
                Decrypt64(restblock, ref plainres);
                plainres.CopyTo(resarray, i * 8);
            }
            if (resarray.Length != crypted.Length)
            {
                Array.Resize(ref resarray, crypted.Length);
            }
            return resarray;
        }

        public void Key(byte[] k)
        {
            var j = 0;
            for (int i = 0; i < 8; i++)
            {
                this.k[i] = (uint)(k[j] | ((k[j + 1] << 8) | (k[j + 2] << 16) | (k[j + 3] << 24)));
                j += 4;
            }
        }

        public void Mac(int len, byte[] data, ref byte[] outres)
        {
            var buf = new byte[8];
            for (int i = 0; i < data.Length; i += 8)
            {
                var plainrest = data.Skip(i).ToArray();
                if (plainrest.Length < 8)
                {
                    Array.Resize(ref plainrest, 8);
                }
                var restblock = plainrest.Take(8).ToArray();
                Mac64(restblock, ref buf);
            }
            MacOut(buf, len, ref outres);
        }

        public void MacOut(byte[] buf, int nbits, ref byte[] outres)
        {
            var nbytes = UnsignedRightShift(nbits, 3);
            Array.Resize(ref outres, nbytes);
            var rembits = nbits & 7;
            var mask = (rembits > 1) ? ((int)Convert.ToDecimal(1 < rembits) - 1) : 0;

            for (int i = 0; i < nbytes; i++)
            {
                outres[i] = buf[i];
            }
            if (rembits > 1)
            {
                outres[nbytes] = (byte)(buf[nbytes] & mask);
            }
        }

        public void Mac64(byte[] datablock, ref byte[] outres)
        {
            var n = this.n;
            for (int i = 0; i < 8; i++)
            {
                outres[i] ^= datablock[i];
            }
            n[0] = (uint)(datablock[0] | (datablock[1] << 8) | (datablock[2] << 16) | (datablock[3] << 24));
            n[1] = (uint)(datablock[4] | (datablock[5] << 8) | (datablock[6] << 16) | (datablock[7] << 24));

            n[1] ^= Pass(n[0] + k[0]); n[0] ^= Pass(n[1] + k[1]);
            n[1] ^= Pass(n[0] + k[2]); n[0] ^= Pass(n[1] + k[3]);
            n[1] ^= Pass(n[0] + k[4]); n[0] ^= Pass(n[1] + k[5]);
            n[1] ^= Pass(n[0] + k[6]); n[0] ^= Pass(n[1] + k[7]);

            n[1] ^= Pass(n[0] + k[0]); n[0] ^= Pass(n[1] + k[1]);
            n[1] ^= Pass(n[0] + k[2]); n[0] ^= Pass(n[1] + k[3]);
            n[1] ^= Pass(n[0] + k[4]); n[0] ^= Pass(n[1] + k[5]);
            n[1] ^= Pass(n[0] + k[6]); n[0] ^= Pass(n[1] + k[7]);

            outres[0] = (byte)(n[0] & 0xff);
            outres[1] = (byte)((n[0] >> 8) & 0xff);
            outres[2] = (byte)((n[0] >> 16) & 0xff);
            outres[3] = (byte)(n[0] >> 24);
            outres[4] = (byte)(n[1] & 0xff);
            outres[5] = (byte)((n[1] >> 8) & 0xff);
            outres[6] = (byte)((n[1] >> 16) & 0xff);
            outres[7] = (byte)(n[1] >> 24);
        }
    }
}
