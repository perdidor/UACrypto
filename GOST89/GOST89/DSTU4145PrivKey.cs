using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GOST89
{

    public class DSTU4145PrivKey
    {
        public byte Type;
        public byte[] BP;
        public byte[] Param_B;
        public int Param_A;
        public System.Numerics.BigInteger Order;
        public System.Numerics.BigInteger P_Param_M;
        public byte P_KS_Type;
        public System.Numerics.BigInteger P_KS_TrinominalValue;
    }
}
