/*
 * rabinhash64.cpp
 *
 *  Created on: Apr 6, 2009
 *      Author: luo
 */
#include "rabinhash64.h"
#include <iostream>

RabinHashFunction64::RabinHashFunction64(const long long _P) :
	P(_P) {
	initializeTables();
}

RabinHashFunction64::~RabinHashFunction64() {

	delete[] table32;
	delete[] table40;
	delete[] table48;
	delete[] table56;
	delete[] table64;
	delete[] table72;
	delete[] table80;
	delete[] table88;


}

long long RabinHashFunction64::getP() {
	return P;
}

void RabinHashFunction64::initializeTables() {
	long long* mods = new long long[P_DEGREE];

	// We want to have mods[i] == x^(P_DEGREE+i)
	mods[0] = P;
	for (int i = 1; i < P_DEGREE; i++) {
		const long long lastMod = mods[i - 1];
		// x^i == x(x^(i-1)) (mod P)
		long long thisMod = lastMod << 1;
		// if x^(i-1) had a x_(P_DEGREE-1) term then x^i has a
		// x^P_DEGREE term that 'fell off' the top end.
		// Since x^P_DEGREE == P (mod P), we should add P
		// to account for this:
		if ((lastMod & X_P_DEGREE) != 0) {
			thisMod ^= P;
		}
		mods[i] = thisMod;
	}

	// Let i be a number between 0 and 255 (i.e. a byte).
	// Let its bits be b0, b1, ..., b7.
	// Let Q32 be the polynomial b0*x^39 + b1*x^38 + ... + b7*x^32 (mod P).
	// Then table32[i] is Q32, represented as an int (see below).
	// Likewise Q40 be the polynomial b0*x^47 + b1*x^46 + ... + b7*x^40 (mod P).
	// table40[i] is Q40, represented as an int. Likewise table48 and table56, etc.

	table32 = new long long[256];
	table40 = new long long[256];
	table48 = new long long[256];
	table56 = new long long[256];
	table64 = new long long[256];
	table72 = new long long[256];
	table80 = new long long[256];
	table88 = new long long[256];

	for (int i = 0; i < 256; i++) {
		int c = i;
		for (int j = 0; j < 8 && c > 0; j++) {
			if ((c & 1) != 0) {
				table32[i] ^= mods[j];
				table40[i] ^= mods[j + 8];
				table48[i] ^= mods[j + 16];
				table56[i] ^= mods[j + 24];
				table64[i] ^= mods[j + 32];
				table72[i] ^= mods[j + 40];
				table80[i] ^= mods[j + 48];
				table88[i] ^= mods[j + 56];
			}
			c >>= 1;
		}
	}
	delete mods;
}

long long RabinHashFunction64::computeWShifted(const long long w) {

	return table32[(int) (w & 0xFF)] ^ table40[(int) ((w >> 8) & 0xFF)]
			^ table48[(int) ((w >> 16) & 0xFF)] ^ table56[(int) ((w >> 24)
			& 0xFF)] ^ table64[(int) ((w >> 32) & 0xFF)] ^ table72[(int) ((w
			>> 40) & 0xFF)] ^ table80[(int) ((w >> 48) & 0xFF)]
			^ table88[(int) ((w >> 56) & 0xFF)];
}

long long RabinHashFunction64::hash(const char A[], const int size) {
	return hash(A, 0, size, 0);
}
long long RabinHashFunction64::hash(const char A[], const int offset,
		const int length, long long w) {
	int s = offset;

	// First, process a few bytes so that the number of bytes remaining is a multiple of 8.
	// This makes the later loop easier.
	const int starterBytes = length % 8;
	if (starterBytes != 0) {
		const int max = offset + starterBytes;
		while (s < max) {
			w = (w << 8) ^ (A[s] & 0xFF);
			s++;
		}
	}

	const int max = offset + length;

	while (s < max) {
	            w = computeWShifted(w) ^
	                (A[s] << 56) ^
	                ((A[s + 1] & 0xFF) << 48) ^
	                ((A[s + 2] & 0xFF) << 40) ^
	                ((A[s + 3] & 0xFF) << 32) ^
	                ((A[s + 4] & 0xFF) << 24) ^
	                ((A[s + 5] & 0xFF) << 16) ^
	                ((A[s + 6] & 0xFF) << 8) ^
	                (A[s + 7] & 0xFF);
	            s += 8;
	        }

	return w;
}
