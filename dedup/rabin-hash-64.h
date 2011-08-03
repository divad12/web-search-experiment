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

#ifndef RABIN_HASH_64
#define RABIN_HASH_64

/**
 * <p>This class provides an implementation of a hash function based on Rabin fingerprints, one
 * which can efficiently produce a 64-bit hash value for a sequence of bytes. Its services and characteristics
 * are entirely analogous to that of {@link RabinHashFunction32}, except that hash values are 64 bits and
 * the implementation works in terms of degree 64 polynomials represented as <code>unsigned long long</code>s.</p>
 *
 * <p>Please see the documentation and comments for {@link RabinHashFunction32} for more information.</p>
 *
 * @author Sean Owen
 * @version 2.0
 * @since 2.0
 */

class RabinHashFunction64 {
  public:

    // Default ctor that uses default irreducible poly
    RabinHashFunction64();

    /**
     * <p>Creates a RabinHashFunction64 based on the specified polynomial.</p>
     *
     * <p>This class does not test the polynomial for irreducibility; therefore this constructor should
     * only be used with polynomials that are already known to be irreducible, or else the hash function
     * will not perform optimally.</p>
     *
     * @param P a degree 64 polynomial over GF(2), represented as a <code>unsigned long long</code>
     */
    RabinHashFunction64(unsigned long long P);

    void initializeTables();

    /**
     * @return irreducible polynomial used in this hash function, represented as a <code>unsigned long long</code>
     */
    unsigned long long getP() {
        return P;
    }

    unsigned long long computeWShifted(unsigned long long w);

    unsigned long long hash(const char* A, const int offset, const int length,
            unsigned long long w);

    /**
     * <p>Return the Rabin hash value of an array of chars.</p>
     *
     * @param A the array of chars
     * @return the hash value
     * @throws NullPointerException if A is null
     */
    unsigned long long hash(const char* A, int len);

  private:

    /** Represents x<sup>64</sup> + x<sup>4</sup> + x<sup>3</sup> + x + 1. */
    static const unsigned long long DEFAULT_IRREDUCIBLE_POLY = 0x000000000000001BL;

    static const int P_DEGREE = 64;
    static const unsigned long long X_P_DEGREE = 1L << (P_DEGREE - 1);

    unsigned long long P;
    unsigned long long table32[256], table40[256], table48[256], table56[256],
                  table64[256], table72[256], table80[256], table88[256];
};

#endif
