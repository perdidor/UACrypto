/*
 * PrivateKey.c
 *
 * Created: 09.09.2021 08:59:05
 *  Author: root
 */ 

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "dstu_types.h"
#include "PrivateKey.h"
#include "Field.h"
#include "Point.h"
#include "bnops.h"
#include "USART.h"


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

uint32_t erand_bytes[9] = { 1735584209, 2323882957, 2410426351, 2233797261, 3727886745, 4055813777, 2664048444, 2043138973, 0 };

uint32_t basepointporecompneg1x[9] = { 3637645239, 883550428, 454084131, 2059702730, 812416512, 1439507979, 2098109292, 707391264, 0 };
uint32_t basepointporecompneg1y[9] = { 2383027032, 3218719789, 2999373236, 2621816153, 3737931522, 3389116627, 2320125736, 749681471, 1 };

uint32_t basepointporecompneg2x[9] = { 2613360306, 1667938981, 607691220, 2742393884, 3212129274, 2003913559, 1986648485, 3784283411, 1 };
uint32_t basepointporecompneg2y[9] = { 1138304892, 3466436294, 313807637, 2553483940, 1162913200, 3915477993, 3394539189, 3262203277, 0 };

uint32_t basepointporecompneg3x[9] = { 1359973285, 2521481021, 3546914542, 1368010729, 352586562, 1939339632, 1400021079, 3387199750, 0 };
uint32_t basepointporecompneg3y[9] = { 3484387018, 4161698692, 3036081286, 2990776814, 1036571425, 3937288102, 3065200500, 2330549393, 0 };

uint32_t basepointporecompneg4x[9] = { 1824396833, 3748849886, 874943285, 602016625, 3271413152, 1476816882, 3849058325, 1559650936, 1 };
uint32_t basepointporecompneg4y[9] = { 1537037456, 29932446, 2426028030, 2496872765, 2704923121, 1766060269, 1872300399, 1350427508, 0 };

uint32_t basepointporecompneg5x[9] = { 3126071046, 3515982981, 1469322739, 2410083673, 185629223, 1236400318, 1409632104, 1792000909, 0 };
uint32_t basepointporecompneg5y[9] = { 2207947819, 1276872806, 121325396, 3027006035, 3957982580, 240312262, 1180052502, 691564111, 1 };

uint32_t basepointporecompneg6x[9] = { 1956918057, 2950331057, 2475894778, 3148246273, 2044379758, 3064275906, 3723970363, 3916612548, 1 };
uint32_t basepointporecompneg6y[9] = { 2122645662, 3964170837, 3398025886, 545152164, 2779821046, 998860451, 3990897997, 2908117458, 0 };

uint32_t basepointporecompneg7x[9] = { 3438787665, 832624869, 1998569257, 3913008110, 3681655448, 1142650923, 2136390646, 447278775, 0 };
uint32_t basepointporecompneg7y[9] = { 1892190028, 1968831295, 165682964, 1827146304, 3283534597, 1232205612, 2772996257, 4139675577, 0 };

uint32_t basepointporecompneg8x[9] = { 3366835394, 2171820301, 316349421, 1931728038, 2059043711, 374821021, 3833875755, 3011852200, 1 };
uint32_t basepointporecompneg8y[9] = { 1588004793, 522562889, 896344552, 1573622348, 230115952, 2173749494, 652684018, 487699146, 0 };

uint32_t basepointporecomppos1x[9] = { 3637645239, 883550428, 454084131, 2059702730, 812416512, 1439507979, 2098109292, 707391264, 0 };
uint32_t basepointporecomppos1y[9] = { 1457001711, 2339384561, 2849377175, 3867225235, 4003475714, 2681009880, 4148483140, 109499423, 1 };

uint32_t basepointporecomppos2x[9] = { 2613360306, 1667938981, 607691220, 2742393884, 3212129274, 2003913559, 1986648485, 3784283411, 1 };
uint32_t basepointporecomppos2y[9] = { 3625816526, 2918655075, 915206849, 994479800, 4196767306, 2651863230, 3158136592, 603902110, 1 };

uint32_t basepointporecomppos3x[9] = { 1359973285, 2521481021, 3546914542, 1368010729, 352586562, 1939339632, 1400021079, 3387199750, 0 };
uint32_t basepointporecomppos3y[9] = { 2661289327, 1850000569, 1738502760, 3821637127, 684513379, 2570700502, 3854671651, 1124978071, 0 };

uint32_t basepointporecomppos4x[9] = { 1824396833, 3748849886, 874943285, 602016625, 3271413152, 1476816882, 3849058325, 1559650936, 1 };
uint32_t basepointporecomppos4y[9] = { 925057713, 3736752960, 2763828427, 3073460812, 1673794641, 826646303, 2331177338, 210469132, 1 };

uint32_t basepointporecomppos5x[9] = { 3126071046, 3515982981, 1469322739, 2410083673, 185629223, 1236400318, 1409632104, 1792000909, 0 };
uint32_t basepointporecomppos5y[9] = { 969839405, 2643078371, 1353667239, 1003128586, 3774512979, 1206069112, 307456894, 1140305346, 1 };

uint32_t basepointporecomppos6x[9] = { 1956918057, 2950331057, 2475894778, 3148246273, 2044379758, 3064275906, 3723970363, 3916612548, 1 };
uint32_t basepointporecomppos6y[9] = { 169948087, 1133644004, 1494934884, 2614636965, 3697963416, 2368488801, 806814326, 1143249430, 1 };

uint32_t basepointporecomppos7x[9] = { 3438787665, 832624869, 1998569257, 3913008110, 3681655448, 1142650923, 2136390646, 447278775, 0 };
uint32_t basepointporecomppos7y[9] = { 3158260509, 1157178330, 2130681917, 2245249454, 415717789, 225087239, 3659410263, 3960897806, 0 };

uint32_t basepointporecomppos8x[9] = { 3366835394, 2171820301, 316349421, 1931728038, 2059043711, 374821021, 3833875755, 3011852200, 1 };
uint32_t basepointporecomppos8y[9] = { 2517291899, 2656498756, 666253829, 786974442, 1997392143, 2546440299, 3261294553, 2928969058, 1 };

uint8_t * ArrayAddZero(uint8_t * array, uint8_t len, bool reorder) {
	uint8_t newdatalen = len + 1;
	uint8_t * res = malloc(newdatalen);
	int index = 0;
	if (!reorder) {
		res[index] = 0;
	} else {
		res[len] = 0;
	}
	index++;
	while (index < newdatalen)
	{
		res[index] = array[(reorder) ? (len - index) : (index - 1)];
		index++;
	}
	return res;
}

void SignHash(uint8_t * hashvalue) {
	field_t hash_v;
	field_t rand_e;
	//field_t basex;
	//field_t basey;
	field_t r;
	uint8_t * withzero = ArrayAddZero(hashvalue, 32, true);
	FieldFromByteArray(withzero, 33, 9, &hash_v);
	//FieldCreateRandom(&rand_e);
	//rand_e.length = 9;
	//rand_e._is_field = true;
	//rand_e.bytes = malloc(sizeof(uint32_t) * 8);
	//memcpy(rand_e.bytes, erand_bytes, sizeof(uint32_t) * 9);
	FieldFromUint32Buf(erand_bytes, 9, &rand_e);
	//FieldFromUint32Buf(Curve_basepoint_field_x, 18, &basex);
	//FieldFromUint32Buf(Curve_basepoint_field_y, 9, &basey);
	point_t basepoint;
	FieldFromUint32Buf(basepointporecompneg1x, 9, &basepoint.precomp_neg[0].x);
	FieldFromUint32Buf(basepointporecompneg2x, 9, &basepoint.precomp_neg[1].x);
	FieldFromUint32Buf(basepointporecompneg3x, 9, &basepoint.precomp_neg[2].x);
	FieldFromUint32Buf(basepointporecompneg4x, 9, &basepoint.precomp_neg[3].x);
	FieldFromUint32Buf(basepointporecompneg5x, 9, &basepoint.precomp_neg[4].x);
	FieldFromUint32Buf(basepointporecompneg6x, 9, &basepoint.precomp_neg[5].x);
	FieldFromUint32Buf(basepointporecompneg7x, 9, &basepoint.precomp_neg[6].x);
	FieldFromUint32Buf(basepointporecompneg8x, 9, &basepoint.precomp_neg[7].x);
	FieldFromUint32Buf(basepointporecompneg1y, 9, &basepoint.precomp_neg[0].y);
	FieldFromUint32Buf(basepointporecompneg2y, 9, &basepoint.precomp_neg[1].y);
	FieldFromUint32Buf(basepointporecompneg3y, 9, &basepoint.precomp_neg[2].y);
	FieldFromUint32Buf(basepointporecompneg4y, 9, &basepoint.precomp_neg[3].y);
	FieldFromUint32Buf(basepointporecompneg5y, 9, &basepoint.precomp_neg[4].y);
	FieldFromUint32Buf(basepointporecompneg6y, 9, &basepoint.precomp_neg[5].y);
	FieldFromUint32Buf(basepointporecompneg7y, 9, &basepoint.precomp_neg[6].y);
	FieldFromUint32Buf(basepointporecompneg8y, 9, &basepoint.precomp_neg[7].y);

	FieldFromUint32Buf(basepointporecomppos1x, 9, &basepoint.precomp_pos[0].x);
	FieldFromUint32Buf(basepointporecomppos2x, 9, &basepoint.precomp_pos[1].x);
	FieldFromUint32Buf(basepointporecomppos3x, 9, &basepoint.precomp_pos[2].x);
	FieldFromUint32Buf(basepointporecomppos4x, 9, &basepoint.precomp_pos[3].x);
	FieldFromUint32Buf(basepointporecomppos5x, 9, &basepoint.precomp_pos[4].x);
	FieldFromUint32Buf(basepointporecomppos6x, 9, &basepoint.precomp_pos[5].x);
	FieldFromUint32Buf(basepointporecomppos7x, 9, &basepoint.precomp_pos[6].x);
	FieldFromUint32Buf(basepointporecomppos8x, 9, &basepoint.precomp_pos[7].x);
	FieldFromUint32Buf(basepointporecomppos1y, 9, &basepoint.precomp_pos[0].y);
	FieldFromUint32Buf(basepointporecomppos2y, 9, &basepoint.precomp_pos[1].y);
	FieldFromUint32Buf(basepointporecomppos3y, 9, &basepoint.precomp_pos[2].y);
	FieldFromUint32Buf(basepointporecomppos4y, 9, &basepoint.precomp_pos[3].y);
	FieldFromUint32Buf(basepointporecomppos5y, 9, &basepoint.precomp_pos[4].y);
	FieldFromUint32Buf(basepointporecomppos6y, 9, &basepoint.precomp_pos[5].y);
	FieldFromUint32Buf(basepointporecomppos7y, 9, &basepoint.precomp_pos[6].y);
	FieldFromUint32Buf(basepointporecomppos8y, 9, &basepoint.precomp_pos[7].y);

	FieldFromUint32Buf(Curve_basepoint_field_x, 18, &basepoint.x);
	FieldFromUint32Buf(Curve_basepoint_field_y, 9, &basepoint.y);
	point_t eG;
	PointMulPos(&basepoint, &rand_e, &eG);
	FieldMod_Mul(&hash_v, &eG.x, &r);
	field_t r2;
	FieldTruncate(&r, &r2);
	bignumber_t R;
	BNFromField(&r2, &R);
	bignumber_t BigD;
	BNFromUInt32Buf(&Priv_param_d[0], 8, &R);
	bignumber_t BigRandE;
	BNFromField(&rand_e, &BigRandE);
	bignumber_t BigOrder;
	BNFromUInt32Buf(&Curve_field_order[0], 17, &BigOrder);
	bignumber_t bn1;
	BNComb10MulTo(&BigD, &R, &bn1);
	divmodres_t s;
	BNDivMod(&bn1, &BigOrder, true, &s);
	bignumber_t sukka;
	BNAdd(s.mod, &BigRandE, &sukka);
	BNDivMod(&sukka, &BigOrder, true, &s);
}
