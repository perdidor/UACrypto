using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GOST89
{
    class PRNG
    {
        private byte[] Counter = new byte[8];
        private byte[] I = new byte[8];
        private byte[] X = new byte[8];
        private gost89 Gost = new gost89();

        public PRNG(byte[] Seed)
        {
            Gost.Key(Seed);
        }

        private void Increment()
        {
            var zero = 0;
            for (int idx = 0; idx < 8; idx++)
            {
                Counter[idx]++;
                if (Counter[idx] > 0)
                {
                    break;
                }
                zero = idx - 1;
            }
            for (int idx = 0; idx < zero; idx++)
            {
                Counter[idx] = 0;
            }
        }

        private byte[] Bit()
        {
            Gost.Crypt64(Counter, ref I);
            for (int idx = 0; idx < 8; idx++)
            {
                I[idx] ^= Counter[idx];
            }
            Gost.Crypt64(I, ref X);
            var res = X;
            for (int idx = 0; idx < 8; idx++)
            {
                X[idx] ^= I[idx];
            }
            Gost.Crypt64(X, ref Counter);
            Increment();
            return res;
        }

        private byte[] Value()
        {
            var res = new byte[8];
            for (int idx = 0; idx < 8; idx++)
            {
                for (int bidx = 0; bidx < 8; bidx++)
                {
                    var step = Bit();
                    res[idx] |= (byte)((step[0] & 1) << bidx);
                }
            }
            return res;
        }

        public byte[] Next(int Length = 8)
        {
            var res = new byte[8];
            var off = 0;
            while (Length > off)
            {
                var step = Value();
                step.CopyTo(res, off);
                off += step.Length;
            }
            return res;
        }
    }
}
