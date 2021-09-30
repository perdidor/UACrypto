/*
 * PrivateKey.c
 *
 * Created: 09.09.2021 08:59:05
 *  Author: root
 */ 


#define F_CPU	16000000UL

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <string.h>
#include "PrivateKey.h"
#include "USART.h"
#include "dstu_types.h"
#include "Field.h"
#include "notaxTemplate.h"
#include "gost89.h"
#include "ADCops.h"
#include "PRNG.h"
#include "LEDC.h"

uint32_t Priv_param_d[8] = { 2082397019, 1468370442, 823178231, 2862269997, 2052372100, 4252500519, 3735157276, 574723566 };

uint32_t Priv_mod_bits[3] = { 257, 12, 0 };
uint8_t Priv_mod_words = 9;
uint8_t Priv_sbox[64] = { 169, 214, 235, 69, 241, 60, 112, 130, 128, 196, 150, 123, 35, 31, 94, 173, 246, 88, 235, 164, 192, 55, 41, 29, 56, 217, 107, 240, 37, 202, 78, 23, 248, 233, 114, 13, 198, 21, 180, 58, 40, 151, 95, 11, 193, 222, 163, 100, 56, 181, 100, 234, 44, 23, 159, 208, 18, 62, 109, 184, 250, 197, 121, 4 };

uint32_t Curve_mod_tmp[22] = { 1163133184, 1363497989, 1413747780, 22364500, 286605332, 269566032, 68243457, 1431650576, 67453200, 1142243600, 88101141, 20993345, 336663892, 65809, 1091572996, 1347704080, 1, 0, 0, 0, 0, 0 };
size_t Curve_m = 257;
size_t Curve_length = 8;
uint32_t Curve_inv_tmp1[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_inv_tmp2[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_a[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_b[18] = { 3689135632, 530508394, 1166825350, 4287899073, 2811503508, 418986199, 18179454, 3472135282, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_kofactor[1] = { 4 };
uint32_t Curve_field_modulus[9] = { 4097, 0, 0, 0, 0, 0, 0, 0, 2 };
uint32_t Curve_field_one[9] = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_order[17] = { 2424129293, 3554768660, 4051888519, 1733894458, 0, 0, 0, 2147483648, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t Curve_field_zero[1] = { 0 };
uint32_t Curve_basepoint_field_x[18] = { 3637645239, 883550428, 454084131, 2059702730, 812416512, 1439507979, 2098109292, 707391264, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_basepoint_field_y[9] = { 1457001711, 2339384561, 2849377175, 3867225235, 4003475714, 2681009880, 4148483140, 109499423, 1 };

precomp_set_t PreComputedPoints = { 
	{
		{ { 3637645239, 883550428, 454084131, 2059702730, 812416512, 1439507979, 2098109292, 707391264, 0 }, { 2383027032, 3218719789, 2999373236, 2621816153, 3737931522, 3389116627, 2320125736, 749681471, 1 } },
		{ { 2613360306, 1667938981, 607691220, 2742393884, 3212129274, 2003913559, 1986648485, 3784283411, 1 }, { 1138304892, 3466436294, 313807637, 2553483940, 1162913200, 3915477993, 3394539189, 3262203277, 0 } },
		{ { 1359973285, 2521481021, 3546914542, 1368010729, 352586562, 1939339632, 1400021079, 3387199750, 0 }, { 3484387018, 4161698692, 3036081286, 2990776814, 1036571425, 3937288102, 3065200500, 2330549393, 0 } },
		{ { 1824396833, 3748849886, 874943285, 602016625, 3271413152, 1476816882, 3849058325, 1559650936, 1 }, { 1537037456, 29932446, 2426028030, 2496872765, 2704923121, 1766060269, 1872300399, 1350427508, 0 } },
		{ { 3126071046, 3515982981, 1469322739, 2410083673, 185629223, 1236400318, 1409632104, 1792000909, 0 }, { 2207947819, 1276872806, 121325396, 3027006035, 3957982580, 240312262, 1180052502, 691564111, 1 } },
		{ { 1956918057, 2950331057, 2475894778, 3148246273, 2044379758, 3064275906, 3723970363, 3916612548, 1 }, { 2122645662, 3964170837, 3398025886, 545152164, 2779821046, 998860451, 3990897997, 2908117458, 0 } },
		{ { 3438787665, 832624869, 1998569257, 3913008110, 3681655448, 1142650923, 2136390646, 447278775, 0 }, { 1892190028, 1968831295, 165682964, 1827146304, 3283534597, 1232205612, 2772996257, 4139675577, 0 } },
		{ { 3366835394, 2171820301, 316349421, 1931728038, 2059043711, 374821021, 3833875755, 3011852200, 1 }, { 1588004793, 522562889, 896344552, 1573622348, 230115952, 2173749494, 652684018, 487699146, 0 } }
	}, 
	{
		{ { 3637645239, 883550428, 454084131, 2059702730, 812416512, 1439507979, 2098109292, 707391264, 0 }, { 1457001711, 2339384561, 2849377175, 3867225235, 4003475714, 2681009880, 4148483140, 109499423, 1 } },
		{ { 2613360306, 1667938981, 607691220, 2742393884, 3212129274, 2003913559, 1986648485, 3784283411, 1 }, { 3625816526, 2918655075, 915206849, 994479800, 4196767306, 2651863230, 3158136592, 603902110, 1 } },
		{ { 1359973285, 2521481021, 3546914542, 1368010729, 352586562, 1939339632, 1400021079, 3387199750, 0 }, { 2661289327, 1850000569, 1738502760, 3821637127, 684513379, 2570700502, 3854671651, 1124978071, 0 } },
		{ { 1824396833, 3748849886, 874943285, 602016625, 3271413152, 1476816882, 3849058325, 1559650936, 1 }, { 925057713, 3736752960, 2763828427, 3073460812, 1673794641, 826646303, 2331177338, 210469132, 1 } },
		{ { 3126071046, 3515982981, 1469322739, 2410083673, 185629223, 1236400318, 1409632104, 1792000909, 0 }, { 969839405, 2643078371, 1353667239, 1003128586, 3774512979, 1206069112, 307456894, 1140305346, 1 } },
		{ { 1956918057, 2950331057, 2475894778, 3148246273, 2044379758, 3064275906, 3723970363, 3916612548, 1 }, { 169948087, 1133644004, 1494934884, 2614636965, 3697963416, 2368488801, 806814326, 1143249430, 1 } },
		{ { 3438787665, 832624869, 1998569257, 3913008110, 3681655448, 1142650923, 2136390646, 447278775, 0 }, { 3158260509, 1157178330, 2130681917, 2245249454, 415717789, 225087239, 3659410263, 3960897806, 0 } },
		{ { 3366835394, 2171820301, 316349421, 1931728038, 2059043711, 374821021, 3833875755, 3011852200, 1 }, { 2517291899, 2656498756, 666253829, 786974442, 1997392143, 2546440299, 3261294553, 2928969058, 1 } }
	} 
};

uint32_t erand_bytes[9] = { 3664937398, 1653150529, 2808328673, 2317406940, 1733758146, 1812339012, 1127984026, 4253689913, 0 };

uint8_t signatureOut[64];

field_t curve_order;
field_t paramd;
field_t rand_e;
point_t basepoint;

uint8_t uint8_buffer[128];
uint8_t withzero[33];

void Signer_Setup() {
	FieldFromUint32Buf(Curve_field_order, 17, &curve_order);
	FieldFromUint32Buf(Curve_basepoint_field_x, 18, &basepoint.x);
	FieldFromUint32Buf(Curve_basepoint_field_y, 9, &basepoint.y);
	FieldFromUint32Buf(Priv_param_d, 8, &paramd);
	FieldFromUint32Buf(erand_bytes, 9, &rand_e);
}

void SignHash(uint8_t * hashvalue) {
	field_t hash_v;
	field_t r;
	field_t r2;
	
	point_t eG1;
	point_t eG2;

	bignumber_t R;
	bignumber_t R2;
	bignumber_t BigD;
	bignumber_t BigRandE;
	bignumber_t BigOrder;
	bignumber_t BigS;
	bignumber_t BigS2;
	bignumber_t BigS3;
	
	uint8_t signatureS[32];
	uint8_t signatureR[32];

	memset(withzero, 0x00, 33);
	for (int i = 0; i < 32; i++)
	{
		withzero[i + 1] = hashvalue[31 - i];
	}
	
	FieldFromByteArray(withzero, 33, 9, &hash_v);

	while (true) {
		//erand_bytes[0] = (uint32_t)((uint32_t)0x00 << 24) | ((uint32_t)GetRandomByte() << 16) | ((uint32_t)GetRandomByte() << 8) | (uint32_t)GetRandomByte();
		for (int i = 0; i < 8; i++)
		{
			erand_bytes[i] = (uint32_t)((uint32_t)GetRandomByte() << 24) | ((uint32_t)GetRandomByte() << 16) | ((uint32_t)GetRandomByte() << 8) | (uint32_t)GetRandomByte();
		}
		erand_bytes[8] = 0;
		FieldFromUint32Buf(erand_bytes, 9, &rand_e);
		if (FieldIs_Less(&rand_e, &curve_order)) {
			break;
		}
	}
	
	PointMulPos_Stage1(&basepoint, &rand_e, &eG1);
	FieldFromUint32Buf(erand_bytes, 9, &rand_e);
	PointMulPos_Stage2(&rand_e, &eG1, &eG2);
	FieldMod_Mul(&hash_v, &eG2.x, &r);
	FieldTruncate(&r, &r2);
	
	int r2buffsize = FieldBuf8(&r2, uint8_buffer);
	BNFromUint8Buf(uint8_buffer, r2buffsize, &R);
	FieldFromUint32Buf(erand_bytes, 9, &rand_e);
	int erandbuffsize = FieldBuf8(&rand_e, uint8_buffer);
	BNFromUint8Buf(uint8_buffer, erandbuffsize, &BigRandE);
	int paramdbuffsize = FieldBuf8(&paramd, uint8_buffer);
	BNFromUint8Buf(uint8_buffer, paramdbuffsize, &BigD);
	int orderbuffsize = FieldBuf8(&curve_order, uint8_buffer);
	BNFromUint8Buf(uint8_buffer, orderbuffsize, &BigOrder);
	BigOrder.Length = 10;
	BNComb10MulTo(&BigD, &R, &R2);
	BNWordDiv(&R2, &BigOrder, &BigS);
	BNAdd(&BigS, &BigRandE, &BigS2);
	BNFromUint8Buf(uint8_buffer, orderbuffsize, &BigOrder);
	BigOrder.Length = 10;
	BNWordDiv(&BigS2, &BigOrder, &BigS3);
	//PrintDebugUInt32Array(BigS3.words, BigS3.Length, -1);
	//PrintDebugUInt32Array(R.words, R.Length, -1);
	BNToBEUint8Array(&BigS3, signatureS);
	BNToBEUint8Array(&R, signatureR);
	for (int i = 31; i >= 0; i--)
	{
		signatureOut[i] = signatureR[31 - i];
		signatureOut[i + 32] = signatureS[31 - i];
	}
	
	SIGN_ACT_OFF();
}
