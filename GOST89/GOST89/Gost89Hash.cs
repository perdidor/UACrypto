using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GOST89
{
    class Gost89Hash
    {
        public gost89 Gost = new gost89();
        public byte[] Left = new byte[0];
        public int Length = 0;
        public byte[] HashMem = new byte[264];
        public byte[] Mem = new byte[264];
        public byte[] U = new byte[32];
        public byte[] V = new byte[32];
        public byte[] W = new byte[32];
        public byte[] _S = new byte[32];
        public byte[] Key = new byte[32];
        public byte[] C8Buf = new byte[8];
        public byte[] S = new byte[32];
        public byte[] H = new byte[32];
        public byte[] Buf = new byte[32];

        public int[] AB2 = new int[4];

        public static void SwapBytes(byte[] w, ref byte[] k)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    k[i + 4 * j] = w[8 * i + j];
                }
            }
        }

        public void Circle_XOR8(byte[] w, ref byte[] k)
        {
            var c8buf = this.C8Buf;
            for (int i = 0; i < 8; i++)
            {
                c8buf[i] = w[i];
            }
            for (int i = 0; i < 24; i++)
            {
                k[i] = w[i + 8];
            }
            for (int i = 0; i < 8; i++)
            {
                k[i + 24] = (byte)(c8buf[i] ^ k[i]);      
            }
        }

        public static void Transform_3(byte[] data)
        {
            var t16 = (data[0] ^ data[2] ^ data[4] ^ data[6] ^ data[24] ^ data[30]) | ((data[1] ^ data[3] ^ data[5] ^ data[7] ^ data[25] ^ data[31]) << 8);
            for (var i = 0; i < 30; i++)
            {
                data[i] = data[i + 2];
            }
            data[30] = (byte)(t16 & 0xff);
            data[31] = (byte)gost89.UnsignedRightShift(t16, 8);
        }

        public int AddBlocks(int n, byte[] left, byte[] right)
        {
            var ab2 = this.AB2;
            ab2[2] = 0;
            ab2[3] = 0;

            for (var i = 0; i < n; i++)
            {
                ab2[0] = left[i];
                ab2[1] = right[i];
                ab2[2] = ab2[0] + ab2[1] + ab2[3];
                left[i] = (byte)(ab2[2] & 0xFF);
                ab2[3] = (byte)gost89.UnsignedRightShift(ab2[2], 8);
            }

            return ab2[3];
        }

        public static void XorBlocks(ref byte[] ret, byte[] a, byte[] b)
        {
            var len = a.Length;
            for (var i = 0; i < len; i++)
            {
                ret[i] = (byte)(a[i] ^ b[i]);
            }
        }

        public void Step(byte[] H, byte[] M)
        {
            var U = this.U;
            var W = this.W;
            var V = this.V;
            var S = this._S;
            var Key = this.Key;
            XorBlocks(ref W, H, M);
            SwapBytes(W, ref Key);
            Gost.Key(Key);
            Gost.Crypt64(H, ref S);
            Circle_XOR8(H, ref U);
            Circle_XOR8(M, ref V);
            Circle_XOR8(V, ref V);
            XorBlocks(ref W, U, V);
            SwapBytes(W, ref Key);
            Gost.Key(Key);
            byte[] sres = S.Skip(8).Take(8).ToArray();
            Gost.Crypt64(H.Skip(8).Take(8).ToArray(), ref sres);
            sres.CopyTo(S, 8);
            Circle_XOR8(U, ref U);
            U[31] = (byte)~U[31]; U[29] = (byte)~U[29]; U[28] = (byte)~U[28]; U[24] = (byte)~U[24];
            U[23] = (byte)~U[23]; U[20] = (byte)~U[20]; U[18] = (byte)~U[18]; U[17] = (byte)~U[17];
            U[14] = (byte)~U[14]; U[12] = (byte)~U[12]; U[10] = (byte)~U[10]; U[8] = (byte)~U[8];
            U[7] = (byte)~U[7]; U[5] = (byte)~U[5]; U[3] = (byte)~U[3]; U[1] = (byte)~U[1];
            Circle_XOR8(V, ref V);
            Circle_XOR8(V, ref V);
            XorBlocks(ref W, U, V);
            SwapBytes(W, ref Key);
            Gost.Key(Key);
            sres = S.Skip(16).Take(8).ToArray();
            Gost.Crypt64(H.Skip(16).Take(8).ToArray(), ref sres);
            sres.CopyTo(S, 16);
            Circle_XOR8(U, ref U);
            Circle_XOR8(V, ref V);
            Circle_XOR8(V, ref V);
            XorBlocks(ref W, U, V);
            SwapBytes(W, ref Key);
            Gost.Key(Key);
            sres = S.Skip(24).Take(8).ToArray();
            Gost.Crypt64(H.Skip(24).Take(8).ToArray(), ref sres);
            sres.CopyTo(S, 24);
            for (var i = 0; i < 12; i++)
            {
                Transform_3(S);
            }
            XorBlocks(ref S, S, M);
            Transform_3(S);
            XorBlocks(ref S, S, H);
            for (var i = 0; i < 61; i++)
            {
                Transform_3(S);
            }
            for (var i = 0; i < 32; i++)
            {
                H[i] = S[i];
            }
        }

        public void Update(byte[] block)
        {
            if (Left.Length > 0)
            {
                Array.Resize(ref Left, Left.Length + block.Length);
                block.CopyTo(Left, Left.Length - block.Length);
            }
            var block32 = block;
            var off = 0;
            while ((block.Length - off) >= 32)
            {
                Step(H, block32);
                AddBlocks(32, S, block32);
                off += 32;
                block32 = block.Skip(off).Take(32).ToArray();
            }
            Length += off;

            if (block32.Length > 0)
            {
                Left = block32;
            }
        }

        public void Update32(byte[] block32)
        {
            Step(H, block32);
            AddBlocks(32, S, block32);
            Length += 32;
        }

        public byte[] Finish()
        {
            var buf = Buf;
            var fin_len = Length;
            var idx = 0;
            if (Left.Length > 0)
            {
                for (idx = 0; idx < Left.Length; idx++)
                {
                    Buf[idx] = Left[idx];
                }
                Step(H, buf);
                AddBlocks(32, S, buf);
                fin_len += Left.Length;
                Left = new byte[0];

                for (idx = 0; idx < 32; idx++)
                {
                    buf[idx] = 0;
                }
            }

            fin_len <<= 3;
            idx = 0;
            while (fin_len > 0)
            {
                buf[idx++] = (byte)(fin_len & 0xFF);
                fin_len >>= 8;
            }

            Step(H, buf);
            Step(H, S);

            var hashval = new byte[32];

            for (idx = 0; idx < 32; idx++)
            {
                hashval[idx] = H[idx];
            }

            return hashval;
        }

        public void Reset()
        {
            for (var idx = 0; idx < 32; idx++)
            {
                H[idx] = 0;
                S[idx] = 0;
            }
            Left = new byte[0];
            Length = 0;
        }



        public byte[] GostHash(byte[] data)
        {
            Update(data);
            return Finish();
        }
    }
}