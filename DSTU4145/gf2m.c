/*
 * gf2m.c
 *
 * Created: 04.09.2021 18:22:13
 *  Author: root
 */ 
#include "gf2m.h"
#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

uint32_t g2fmblength(uint32_t * bytes, int len) {
	uint32_t r = 1;

	uint32_t t = 0;

	uint32_t x = 0;

	int nz = len - 1;

	while (bytes[nz] == 0 && nz >= 0) {
		nz--;
	}

	x = bytes[nz];
	t = x >> 16;
	if (t != 0) {
		x = t;
		r += 16;
	}
	t = x >> 8;
	if (t != 0) {
		x = t;
		r += 8;
	}
	t = x >> 4;
	if (t != 0) {
		x = t;
		r += 4;
	}
	t = x >> 2;
	if (t != 0) {
		x = t;
		r += 2;
	}
	t = x >> 1;
	if (t != 0) {
		x = t;
		r += 1;
	}
	return r + nz * 32;
}

void g2fmshiftRight(uint32_t * bytes, size_t len, uint32_t right) {
	uint32_t wright = floor(right / 32);
	right %= 32;

	uint32_t idx;

	size_t b_len = len;

	uint32_t left = 32 - right;

	uint32_t mask_f = (1 << (1 + right)) - 1;

	uint32_t tmp;

	if (right == 31) mask_f = 0xffffffff;

	uint32_t *_rbytes = malloc(len * sizeof(uint32_t));

	memcpy(_rbytes, bytes, len * sizeof(uint32_t));

	_rbytes[0] = bytes[0] >> right;
	for (idx = 1; idx < b_len; idx++) {
		tmp = bytes[idx] & mask_f;

		_rbytes[idx] = bytes[idx] >> right;
		_rbytes[idx - 1] |= (tmp << left);
	}

	if (wright == 0) {
		memcpy(bytes, _rbytes, len * sizeof(uint32_t));
		free(_rbytes);
		return;
	}

	for (idx = 0; idx < b_len; idx++) {
		_rbytes[idx] = _rbytes[idx + wright] || 0;
	}

	memcpy(bytes, _rbytes, len * sizeof(uint32_t));
	free(_rbytes);
	return;
}

void g2fmmul_1x1(uint32_t * ret, uint16_t offset, uint32_t a, uint32_t b) {
	uint32_t top2b = a >> 30;
	uint32_t ol = offset;
	uint32_t oh = offset + 1;

	uint32_t s;
	uint32_t l;
	uint32_t h;

	uint32_t a1 = a & 0x3fffffff;
	uint32_t a2 = a1 << 1;
	uint32_t a4 = a2 << 1;

	uint32_t tab[] = { 0, a1, a2, a1 ^ a2, a4, a1 ^ a4, a2 ^ a4, a1 ^ a2 ^ a4 };

	s = tab[b & 0x7];
	l = s;
	s = tab[(b >> 3) & 0x7];
	l ^= s << 3;
	h = s >> 29;
	s = tab[(b >> 6) & 0x7];
	l ^= s << 6;
	h ^= s >> 26;
	s = tab[(b >> 9) & 0x7];
	l ^= s << 9;
	h ^= s >> 23;
	s = tab[(b >> 12) & 0x7];
	l ^= s << 12;
	h ^= s >> 20;
	s = tab[(b >> 15) & 0x7];
	l ^= s << 15;
	h ^= s >> 17;
	s = tab[(b >> 18) & 0x7];
	l ^= s << 18;
	h ^= s >> 14;
	s = tab[(b >> 21) & 0x7];
	l ^= s << 21;
	h ^= s >> 11;
	s = tab[(b >> 24) & 0x7];
	l ^= s << 24;
	h ^= s >> 8;
	s = tab[(b >> 27) & 0x7];
	l ^= s << 27;
	h ^= s >> 5;
	s = tab[b >> 30];
	l ^= s << 30;
	h ^= s >> 2;

	if (top2b & 1) {
		l ^= b << 30;
		h ^= b >> 2;
	}
	if (top2b & 2) {
		l ^= b << 31;
		h ^= b >> 1;
	}

	ret[oh] = h;
	ret[ol] = l;
}

void g2fmmul_2x2(uint32_t a1, uint32_t a0, uint32_t b1, uint32_t b0, uint32_t * ret) {
	g2fmmul_1x1(ret, 2, a1, b1);
	g2fmmul_1x1(ret, 0, a0, b0);
	g2fmmul_1x1(ret, 4, a0 ^ a1, b0 ^ b1);

	ret[2] = ret[2] ^ (ret[5] ^ (ret[1] ^ ret[3]));
	ret[1] = ret[3] ^ (ret[2] ^ (ret[0] ^ (ret[4] ^ ret[5])));
	ret[4] = 0;
	ret[5] = 0;
}

void g2fmfmul(uint32_t * a, size_t alen, uint32_t * b, size_t blen, uint32_t * s, size_t slen) {
	uint32_t y1;
	uint32_t y0;
	uint32_t x1;
	uint32_t x0;
	//size_t a_len = alen/sizeof(uint32_t);
	//size_t b_len = blen/sizeof(uint32_t);
	//size_t s_len = slen/sizeof(uint32_t);

	for (int i = 0; i < slen; i++) {
		s[i] = 0;
	}

	uint32_t x22[6];

	for (int j = 0; j < blen; j += 2) {
		y0 = b[j];
		y1 = j + 1 == blen ? 0 : b[j + 1];

		for (int i = 0; i < alen; i += 2) {
			x0 = a[i];
			x1 = i + 1 == alen ? 0 : a[i + 1];

			g2fmmul_2x2(x1, x0, y1, y0, x22);
			s[j + i + 0] ^= x22[0];
			s[j + i + 1] ^= x22[1];
			s[j + i + 2] ^= x22[2];
			s[j + i + 3] ^= x22[3];
		}
	}
}

void g2fmffmod(uint32_t * a, size_t alen, uint32_t * p, size_t p_len, uint32_t * ret) {
	uint32_t zz;
	uint32_t k;
	uint32_t n;
	uint32_t d0;
	uint32_t d1;
	uint32_t tmp_ulong;
	int j;
	//size_t a_len = alen/sizeof(uint32_t);

	size_t ret_len = alen;
	for (k = 0; k < alen; k++) ret[k] = a[k];

	/* start reduction */
	uint32_t dN = floor(p[0] / 32);
	for (j = ret_len - 1; j > dN; ) {
		zz = ret[j];
		if (ret[j] == 0) {
			j--;
			continue;
		}
		ret[j] = 0;

		for (k = 1; k < p_len - 1; k++) {
			/* reducing component t^p[k] */
			n = p[0] - p[k];
			d0 = n % 32;
			d1 = 32 - d0;
			n = floor(n / 32);
			ret[j - n] ^= zz >> d0;
			if (d0) ret[j - n - 1] ^= zz << d1;
		}

		/* reducing component t^0 */
		n = dN;
		d0 = p[0] % 32;
		d1 = 32 - d0;
		ret[j - n] ^= zz >> d0;
		if (d0) ret[j - n - 1] ^= zz << d1;
	}

	/* final round of reduction */
	while (j == dN) {
		d0 = p[0] % 32;
		zz = ret[dN] >> d0;
		if (zz == 0) break;
		d1 = 32 - d0;

		/* clear up the top d1 bits */
		if (d0) ret[dN] = (ret[dN] << d1) >> d1;
		else ret[dN] = 0;
		ret[0] ^= zz; /* reduction t^0 component */

		for (k = 1; k < p_len - 1; k++) {
			/* reducing component t^p[k] */
			n = floor(p[k] / 32);
			d0 = p[k] % 32;
			d1 = 32 - d0;
			ret[n] ^= zz << d0;
			tmp_ulong = zz >> d1;
			if (d0 && tmp_ulong) ret[n + 1] ^= tmp_ulong;
		}
	}
}

void g2fmfinv(uint32_t * a, size_t alen, uint32_t * p, size_t plen, uint32_t * ret) {
	uint32_t b[10];
	uint32_t c[10];
	uint32_t v[10];
	uint32_t u[10];
	//size_t p_len = plen/sizeof(uint32_t);

	b[0] = 1;
	//memcpy(u, a, alen * sizeof(uint32_t));
	//memcpy(v, p, plen * sizeof(uint32_t));
	for (int idx = 0; idx < alen; idx++) {
		u[idx] = a[idx];
	}
	for (int idx = 0; idx < plen; idx++) {
		v[idx] = p[idx];
	}

	uint32_t ubits = g2fmblength(u, alen);
	uint32_t vbits = g2fmblength(v, alen);

	while (1) {
		//uint32_t sad = u[0];
		while (ubits >= 0 && !((u[0] & 1) == 1)) {
			uint32_t u0 = u[0];
			uint32_t b0 = b[0];
			uint32_t u1;
			uint32_t b1;

			uint32_t mask = b0 & 1 ? 0xffffffff : 0;
			b0 ^= p[0] & mask;

			uint32_t idx = 0;
			for (; idx < plen - 1; idx++) {
				u1 = u[idx + 1];
				u[idx] = (u0 >> 1) | (u1 << 31);
				u0 = u1;
				b1 = b[idx + 1] ^ (p[idx + 1] & mask);
				b[idx] = (b0 >> 1) | (b1 << 31);
				b0 = b1;
			}

			u[idx] = u0 >> 1;
			b[idx] = b0 >> 1;
			ubits--;
		}

		if (ubits <= 32 && u[0] == 1) break;

		if (ubits < vbits) {
			uint32_t tmp = ubits;
			ubits = vbits;
			vbits = tmp;
			uint32_t tmparr[alen];
			memcpy(tmparr, u, alen * 4);
			memcpy(u, v, alen * 4);
			memcpy(v, tmparr, alen * 4);
			memcpy(tmparr, b, alen * 4);
			memcpy(b, c, alen * 4);
			memcpy(c, tmparr, alen * 4);
			//free(tmparr);
		}

		//memcpy(u, v, plen * 4);
		//memcpy(b, c, plen * 4);
		for (int idx = 0; idx < plen; idx++) {
			u[idx] ^= v[idx];
			b[idx] ^= c[idx];
		}

		if (ubits == vbits) {
			ubits = g2fmblength(u, alen);
		}
	}

	//for (int idx = 0; idx < alen; idx++) {
		//ret[idx] = b[idx];
	//}
	memcpy(ret, b, alen * 4);
	//free(b);
	//free(c);
	//free(u);
	//free(v);
}
