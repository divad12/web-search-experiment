/*
 * Copyright 2001-2004 Sean Owen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Converted from original Java Source (http://sourceforge.net/projects/rabinhash/files/rabin-hash-function/rabin-hash-function-2.0/rabin-hash-function-2.0.zip/download) to C++

#include "rabin-hash-64.h"

RabinHashFunction64::RabinHashFunction64() {
    P = DEFAULT_IRREDUCIBLE_POLY;
    initializeTables();
}

RabinHashFunction64::RabinHashFunction64(unsigned long long P) {
    P = P;
    initializeTables();
}

void RabinHashFunction64::initializeTables() {
    unsigned long long mods[P_DEGREE];

    // We want to have mods[i] == x^(P_DEGREE+i)
    mods[0] = P;
    for (int i = 1; i < P_DEGREE; i++) {
        const unsigned long long lastMod = mods[i - 1];
        // x^i == x(x^(i-1)) (mod P)
        unsigned long long thisMod = lastMod << 1;
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

    for (int i = 0; i < 256; i++) {
        unsigned int c = i;
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
}

unsigned long long RabinHashFunction64::computeWShifted(unsigned long long w) {
    return table32[(int) (w & 0xFF)] ^
            table40[(int) ((w >> 8) & 0xFF)] ^
            table48[(int) ((w >> 16) & 0xFF)] ^
            table56[(int) ((w >> 24) & 0xFF)] ^
            table64[(int) ((w >> 32) & 0xFF)] ^
            table72[(int) ((w >> 40) & 0xFF)] ^
            table80[(int) ((w >> 48) & 0xFF)] ^
            table88[(int) ((w >> 56) & 0xFF)];
}

unsigned long long RabinHashFunction64::hash(const char* A, const int offset,
        const int length, unsigned long long w) {
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
            ((unsigned long long)A[s] << 56) ^
            (((unsigned long long)A[s + 1] & 0xFF) << 48) ^
            (((unsigned long long)A[s + 2] & 0xFF) << 40) ^
            (((unsigned long long)A[s + 3] & 0xFF) << 32) ^
            (((unsigned long long)A[s + 4] & 0xFF) << 24) ^
            (((unsigned long long)A[s + 5] & 0xFF) << 16) ^
            (((unsigned long long)A[s + 6] & 0xFF) << 8) ^
            (A[s + 7] & 0xFF);
        s += 8;
    }

    return w;
}

unsigned long long RabinHashFunction64::hash(const char* A, int len) {
    int s = 0;
    unsigned long long w = 0;

    // First, process a few chars so that the number of bytes remaining is a multiple of 4.
    // This makes the later loop easier.
    const int starterChars = len % 4;
    while (s < starterChars) {
        w = (w << 16) ^ (A[s] & 0xFFFF);
        s++;
    }

    while (s < len) {
        w = computeWShifted(w) ^
            ((unsigned long long)(A[s] & 0xFFFF) << 48) ^
            ((unsigned long long)(A[s + 1] & 0xFFFF) << 32) ^
            ((unsigned long long)(A[s + 2] & 0xFFFF) << 16) ^
            (A[s + 3] & 0xFFFF);
        s += 4;
    }

    return w;
}
