/*
 * rabinhash64.h
 *
 *  Created on: Apr 6, 2009
 *      Author: luo
 * 由Sean Owen写的java版本改编而来
 *
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
/**
 * <p>This class provides an implementation of a hash function based on Rabin fingerprints, one
 * which can efficiently produce a 64-bit hash value for a sequence of bytes. Its services and characteristics
 * are entirely analogous to that of {@link RabinHashFunction32}, except that hash values are 64 bits and
 * the implementation works in terms of degree 64 polynomials represented as <code>long</code>s.</p>
 *
 * <p>Please see the documentation and comments for {@link RabinHashFunction32} for more information.</p>
 *
 * @author Sean Owen
 * @version 2.0
 * @since 2.0
 */

#ifndef RABINHASH64_H_
#define RABINHASH64_H_

class RabinHashFunction64{
private:
	/** Represents x<sup>64</sup> + x<sup>4</sup> + x<sup>3</sup> + x + 1. */
	    static const long long DEFAULT_IRREDUCIBLE_POLY = 0x000000000000001BL;



	    static const int P_DEGREE = 64;
	    static const long long  X_P_DEGREE = 1LL << (P_DEGREE - 1);
	    static const int READ_BUFFER_SIZE = 1024;

	    const long long P;
	    long long *table32, *table40, *table48, *table56, *table64, *table72, *table80, *table88;

	    void initializeTables();
	    long long computeWShifted(const long long w);

public:
	/**
	     * <p>Creates a RabinHashFunction64 based on the specified polynomial.</p>
	     *
	     * <p>This class does not test the polynomial for irreducibility; therefore this constructor should
	     * only be used with polynomials that are already known to be irreducible, or else the hash function
	     * will not perform optimally.</p>
	     *
	     * @param P a degree 64 polynomial over GF(2), represented as a <code>long</code>
	     */
	RabinHashFunction64(const long long _P);
	~RabinHashFunction64();
	long long getP();
	/**
	     * <p>Return the Rabin hash value of an array of bytes.</p>
	     *
	     * @param A the array of bytes
	     * @return the hash value
	     * @throws NullPointerException if A is null
	     */
	long long hash(const char A[], const int size);
	long long hash(const char A[], const int offset, const int length, long long w);


};

#endif /* RABINHASH64_H_ */
