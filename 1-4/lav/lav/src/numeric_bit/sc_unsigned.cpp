/******************************************************************************
    Copyright (c) 1996-2000 Synopsys, Inc.    ALL RIGHTS RESERVED

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC(TM) Open Community License Software Download and
  Use License Version 1.1 (the "License"); you may not use this file except
  in compliance with such restrictions and limitations. You may obtain
  instructions on how to receive a copy of the License at
  http://www.systemc.org/. Software distributed by Original Contributor
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

******************************************************************************/


/******************************************************************************

    sc_unsigned.cpp -- Arbitrary precision signed arithmetic.
 
    This file includes the definitions of sc_unsigned_bitref,
    sc_unsigned_subref, and sc_unsigned classes. The first two classes
    are proxy classes to reference one bit and a range of bits of a
    sc_unsigned number, respectively. This file also includes
    sc_nbcommon.cpp and sc_nbfriends.cpp, which contain the
    definitions shared by sc_unsigned.

    Original Author: Ali Dasdan. Synopsys, Inc. (dasdan@synopsys.com)
  
******************************************************************************/


/******************************************************************************

    MODIFICATION LOG - modifiers, enter your name, affliation and
    changes you are making here:

    Modifier Name & Affiliation:
    Description of Modification:
    

******************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <cstring>
#if defined(__BCPLUSPLUS__)
#pragma hdrstop
#endif
#include "sc_unsigned.h"
#include "sc_signed.h"

#ifdef _MSC_VER
#define for if(0);else for
#endif


/////////////////////////////////////////////////////////////////////////////
// SECTION: Public members.
/////////////////////////////////////////////////////////////////////////////

// The public members are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Public members - Assignment operators.
/////////////////////////////////////////////////////////////////////////////


// Assignment from a char string v. 
sc_unsigned&
sc_unsigned::operator=(const char *v)
{

  if (!v) {
    printf("SystemC error: Null char string is not allowed.\n");
    abort();    
  }

  sgn = vec_from_str(nbits, ndigits, digit, v);
  convert_SM_to_2C_to_SM();
    
  return *this;

}


// Assignment from an int64 v. 
sc_unsigned&
sc_unsigned::operator=(int64 v)
{

  if (v == 0) {

    sgn = SC_ZERO;
    vec_zero(ndigits, digit);

  }
  else {

    sgn = SC_POS;
    from_uint(ndigits, digit, (uint64) v);
    convert_SM_to_2C_to_SM();
    
  }

  return *this;

}


// Assignment from an uint64 v. 
sc_unsigned&
sc_unsigned::operator=(uint64 v)
{

  if (v == 0) {

    sgn = SC_ZERO;
    vec_zero(ndigits, digit);
    
  }
  else {

    sgn = SC_POS;
    from_uint(ndigits, digit, v);
    convert_SM_to_2C_to_SM();

  }

  return *this;

}


// Assignment from a long v. 
sc_unsigned&
sc_unsigned::operator=(long v)
{

  if (v == 0) {

    sgn = SC_ZERO;
    vec_zero(ndigits, digit);
    
  }
  else {

    sgn = SC_POS;
    from_uint(ndigits, digit, (unsigned long) v);
    convert_SM_to_2C_to_SM();

  }

  return *this;

}


// Assignment from an unsigned long v. 
sc_unsigned&
sc_unsigned::operator=(unsigned long v)
{

  if (v == 0) {

    sgn = SC_ZERO;
    vec_zero(ndigits, digit);
    
  }
  else {

    sgn = SC_POS;
    from_uint(ndigits, digit, v);
    convert_SM_to_2C_to_SM();
    
  }

  return *this;

}


// Assignment from a double v.
sc_unsigned&
sc_unsigned::operator=(double v)
{

  is_bad_double(v);

  sgn = SC_POS;

  register length_type i = 0;

  while (floor(v) && (i < ndigits)) {
#ifndef WIN32
    digit[i++] = (digit_type) floor(remainder(v, DIGIT_RADIX));
#else
    digit[i++] = (digit_type) floor(fmod(v, DIGIT_RADIX));
#endif
    v /= DIGIT_RADIX;
  }

  vec_zero(i, ndigits, digit);

  convert_SM_to_2C_to_SM();

  return *this;  

}


/////////////////////////////////////////////////////////////////////////////
// SECTION: Input and output operators
/////////////////////////////////////////////////////////////////////////////

// The operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Operator macros.
/////////////////////////////////////////////////////////////////////////////


#define CONVERT_LONG(u) \
small_type u ## s = get_sign(u);                   \
digit_type u ## d[DIGITS_PER_ULONG];                    \
from_uint(DIGITS_PER_ULONG, u ## d, (unsigned long) u); 

#define CONVERT_LONG_2(u) \
digit_type u ## d[DIGITS_PER_ULONG];                     \
from_uint(DIGITS_PER_ULONG, u ## d, (unsigned long) u); 

#define CONVERT_INT64(u) \
small_type u ## s = get_sign(u);                   \
digit_type u ## d[DIGITS_PER_UINT64];              \
from_uint(DIGITS_PER_UINT64, u ## d, (uint64) u); 

#define CONVERT_INT64_2(u) \
digit_type u ## d[DIGITS_PER_UINT64];              \
from_uint(DIGITS_PER_UINT64, u ## d, (uint64) u); 

/***************************************************************************
 NEW SEMANTICS
 ***************************************************************************/

#ifdef NEW_SEMANTICS

/////////////////////////////////////////////////////////////////////////////
// SECTION: PLUS operators: +, +=, ++
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u + v:
// 1. 0 + v = v
// 2. u + 0 = u
// 3. if sgn(u) == sgn(v)
//    3.1 u + v = +(u + v) = sgn(u) * (u + v) 
//    3.2 (-u) + (-v) = -(u + v) = sgn(u) * (u + v)
// 4. if sgn(u) != sgn(v)
//    4.1 u + (-v) = u - v = sgn(u) * (u - v)
//    4.2 (-u) + v = -(u - v) ==> sgn(u) * (u - v)
//
// Specialization of above cases for computing ++u or u++: 
// 1. 0 + 1 = 1
// 3. u + 1 = u + 1 = sgn(u) * (u + 1)
// 4. (-u) + 1 = -(u - 1) = sgn(u) * (u - 1)


sc_unsigned
operator+(const sc_unsigned& u, const sc_unsigned& v)
{

  if (u.sgn == SC_ZERO) // case 1
    return sc_unsigned(v);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(u);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator+(const sc_unsigned &u, uint64 v)
{

  if (v == 0)  // case 2
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 1
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator+(uint64 u, const sc_unsigned &v)
{

  if (u == 0) // case 1
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)  // case 2
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // cases 3 and 4

  return add_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator+(const sc_unsigned &u, unsigned long v)
{

  if (v == 0) // case 2
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 1
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator+(unsigned long u, const sc_unsigned &v)
{

  if (u == 0) // case 1
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)  // case 2
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // cases 3 and 4
  return add_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: MINUS operators: -, -=, --
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u + v:
// 1. u - 0 = u 
// 2. 0 - v = -v
// 3. if sgn(u) != sgn(v)
//    3.1 u - (-v) = u + v = sgn(u) * (u + v)
//    3.2 (-u) - v = -(u + v) ==> sgn(u) * (u + v)
// 4. if sgn(u) == sgn(v)
//    4.1 u - v = +(u - v) = sgn(u) * (u - v) 
//    4.2 (-u) - (-v) = -(u - v) = sgn(u) * (u - v)
//
// Specialization of above cases for computing --u or u--: 
// 1. 0 - 1 = -1
// 3. (-u) - 1 = -(u + 1) = sgn(u) * (u + 1)
// 4. u - 1 = u - 1 = sgn(u) * (u - 1)

// The operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: MULTIPLICATION operators: *, *=
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u * v:
// 1. u * 0 = 0 * v = 0
// 2. 1 * v = v and -1 * v = -v
// 3. u * 1 = u and u * -1 = -u
// 4. u * v = u * v

sc_unsigned
operator*(const sc_unsigned& u, const sc_unsigned& v)
{
 
  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator*(const sc_unsigned& u, uint64 v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_INT64_2(v);

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator*(uint64 u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_INT64_2(u);

  // cases 2-4
  return mul_unsigned_friend(s, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator*(const sc_unsigned& u, unsigned long v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_LONG_2(v);

  // else cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);
  
}

sc_unsigned
operator*(unsigned long u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_LONG_2(u);

  // cases 2-4
  return mul_unsigned_friend(s, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: DIVISION operators: /, /=
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when finding the quotient q = floor(u/v):
// Note that u = q * v + r for r < q.
// 1. 0 / 0 or u / 0 => error
// 2. 0 / v => 0 = 0 * v + 0
// 3. u / v && u = v => u = 1 * u + 0  - u or v can be 1 or -1
// 4. u / v && u < v => u = 0 * v + u  - u can be 1 or -1
// 5. u / v && u > v => u = q * v + r  - v can be 1 or -1

sc_unsigned
operator/(const sc_unsigned& u, const sc_unsigned& v)
{

  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) {
    div_by_zero(v.sgn); // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator/(const sc_unsigned& u, uint64 v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(v);

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator/(uint64 u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2

  }

  CONVERT_INT64_2(u);

  // other cases
  return div_unsigned_friend(s, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator/(const sc_unsigned& u, unsigned long v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(v);

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);
  
}


sc_unsigned
operator/(unsigned long u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2

  }

  CONVERT_LONG_2(u);

  // other cases
  return div_unsigned_friend(s, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: MOD operators: %, %=.
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when finding the remainder r = u % v:
// Note that u = q * v + r for r < q.
// 1. 0 % 0 or u % 0 => error
// 2. 0 % v => 0 = 0 * v + 0
// 3. u % v && u = v => u = 1 * u + 0  - u or v can be 1 or -1
// 4. u % v && u < v => u = 0 * v + u  - u can be 1 or -1
// 5. u % v && u > v => u = q * v + r  - v can be 1 or -1

sc_unsigned
operator%(const sc_unsigned& u, const sc_unsigned& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);
}


sc_unsigned
operator%(const sc_unsigned& u, uint64 v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(v);

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator%(uint64 u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64(u);

  // other cases
  return mod_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator%(const sc_unsigned& u, unsigned long v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(v);

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator%(unsigned long u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG(u);

  // other cases
  return mod_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise AND operators: &, &=
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u & v:
// 1. u & 0 = 0 & v = 0
// 2. u & v => sgn = +
// 3. (-u) & (-v) => sgn = -
// 4. u & (-v) => sgn = +
// 5. (-u) & v => sgn = +

sc_unsigned
operator&(const sc_unsigned& u, const sc_unsigned& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_unsigned& u, uint64 v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) // case 1
    return sc_unsigned();

  CONVERT_INT64(v);

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator&(uint64 u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  CONVERT_INT64(u);

  // other cases
  return and_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_unsigned& u, unsigned long v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) // case 1
    return sc_unsigned();

  CONVERT_LONG(v);

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator&(unsigned long u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  CONVERT_LONG(u);

  // other cases
  return and_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise OR operators: |, |=
/////////////////////////////////////////////////////////////////////////////
// Cases to consider when computing u | v:
// 1. u | 0 = u
// 2. 0 | v = v
// 3. u | v => sgn = +
// 4. (-u) | (-v) => sgn = -
// 5. u | (-v) => sgn = -
// 6. (-u) | v => sgn = -

sc_unsigned
operator|(const sc_unsigned& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_unsigned& u, uint64 v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator|(uint64 u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // other cases
  return or_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_unsigned& u, unsigned long v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator|(unsigned long u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // other cases
  return or_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise XOR operators: ^, ^=
/////////////////////////////////////////////////////////////////////////////
// Cases to consider when computing u ^ v:
// Note that  u ^ v = (~u & v) | (u & ~v).
// 1. u ^ 0 = u
// 2. 0 ^ v = v
// 3. u ^ v => sgn = +
// 4. (-u) ^ (-v) => sgn = -
// 5. u ^ (-v) => sgn = -
// 6. (-u) ^ v => sgn = +

sc_unsigned
operator^(const sc_unsigned& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_unsigned& u, uint64 v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}

sc_unsigned
operator^(uint64 u, const sc_unsigned& v)
{
  if (u == 0)
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // other cases
  return xor_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_unsigned& u, unsigned long v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}

sc_unsigned
operator^(unsigned long u, const sc_unsigned& v)
{
  if (u == 0)
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // other cases
  return xor_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise NOT operator: ~
/////////////////////////////////////////////////////////////////////////////

// Operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: LEFT SHIFT operators: <<, <<=
/////////////////////////////////////////////////////////////////////////////

sc_unsigned
operator<<(const sc_unsigned& u, const sc_signed& v)
{
  if ((v.sgn == SC_ZERO) || (v.sgn == SC_NEG))
    return sc_unsigned(u);

  return operator<<(u, v.to_ulong());
}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: RIGHT SHIFT operators: >>, >>=
/////////////////////////////////////////////////////////////////////////////

sc_unsigned
operator>>(const sc_unsigned& u, const sc_signed& v)
{

  if ((v.sgn == SC_ZERO) || (v.sgn == SC_NEG))
    return sc_unsigned(u);

  return operator>>(u, v.to_long());

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Unary arithmetic operators.
/////////////////////////////////////////////////////////////////////////////

sc_unsigned
operator+(const sc_unsigned& u)
{
  return sc_unsigned(u);
}

/***************************************************************************
 OLD SEMANTICS
 ***************************************************************************/

#else  // OLD SEMANTICS

/////////////////////////////////////////////////////////////////////////////
// SECTION: PLUS operators: +, +=, ++
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u + v:
// 1. 0 + v = v
// 2. u + 0 = u
// 3. if sgn(u) == sgn(v)
//    3.1 u + v = +(u + v) = sgn(u) * (u + v) 
//    3.2 (-u) + (-v) = -(u + v) = sgn(u) * (u + v)
// 4. if sgn(u) != sgn(v)
//    4.1 u + (-v) = u - v = sgn(u) * (u - v)
//    4.2 (-u) + v = -(u - v) ==> sgn(u) * (u - v)
//
// Specialization of above cases for computing ++u or u++: 
// 1. 0 + 1 = 1
// 3. u + 1 = u + 1 = sgn(u) * (u + 1)
// 4. (-u) + 1 = -(u - 1) = sgn(u) * (u - 1)

sc_unsigned
operator+(const sc_unsigned& u, const sc_signed& v)
{

  if (u.sgn == SC_ZERO) // case 1
    return sc_unsigned(v, v.sgn);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(u);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator+(const sc_signed& u, const sc_unsigned& v)
{

  if (u.sgn == SC_ZERO) // case 1
    return sc_unsigned(v);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(u, u.sgn);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator+(const sc_unsigned& u, const sc_unsigned& v)
{

  if (u.sgn == SC_ZERO) // case 1
    return sc_unsigned(v);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(u);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator+(const sc_unsigned &u, int64 v)
{

  if (v == 0)  // case 2
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 1
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator+(int64 u, const sc_unsigned &v)
{

  if (u == 0) // case 1
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)  // case 2
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // cases 3 and 4

  return add_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator+(const sc_unsigned &u, uint64 v)
{

  if (v == 0)  // case 2
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 1
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator+(uint64 u, const sc_unsigned &v)
{

  if (u == 0) // case 1
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)  // case 2
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // cases 3 and 4

  return add_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}



sc_unsigned
operator+(const sc_unsigned &u, long v)
{

  if (v == 0)  // case 2
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 1
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator+(long u, const sc_unsigned &v)
{

  if (u == 0) // case 1
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)  // case 2
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // cases 3 and 4
  return add_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator+(const sc_unsigned &u, unsigned long v)
{

  if (v == 0) // case 2
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 1
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator+(unsigned long u, const sc_unsigned &v)
{

  if (u == 0) // case 1
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)  // case 2
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // cases 3 and 4
  return add_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: MINUS operators: -, -=, --
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u + v:
// 1. u - 0 = u 
// 2. 0 - v = -v
// 3. if sgn(u) != sgn(v)
//    3.1 u - (-v) = u + v = sgn(u) * (u + v)
//    3.2 (-u) - v = -(u + v) ==> sgn(u) * (u + v)
// 4. if sgn(u) == sgn(v)
//    4.1 u - v = +(u - v) = sgn(u) * (u - v) 
//    4.2 (-u) - (-v) = -(u - v) = sgn(u) * (u - v)
//
// Specialization of above cases for computing --u or u--: 
// 1. 0 - 1 = -1
// 3. (-u) - 1 = -(u + 1) = sgn(u) * (u + 1)
// 4. u - 1 = u - 1 = sgn(u) * (u - 1)

sc_unsigned
operator-(const sc_unsigned& u, const sc_signed& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO) // case 2
    return sc_unsigned(v, -v.sgn);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             -v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator-(const sc_signed& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u, u.sgn);

  if (u.sgn == SC_ZERO) // case 2
    return sc_unsigned(v, -v.sgn);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             -v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator-(const sc_unsigned& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO) // case 2
    return sc_unsigned(v, -v.sgn);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             -v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator-(const sc_unsigned &u, int64 v)
{

  if (v == 0) // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO) // case 2
    return sc_unsigned(-vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             -vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator-(int64 u, const sc_unsigned& v)
{

  if (u == 0) // case 1
    return sc_unsigned(v, -v.sgn);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // cases 3 and 4

  return add_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             -v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator-(const sc_unsigned &u, uint64 v)
{

  if (v == 0) // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO) // case 2
    return sc_unsigned(-vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // cases 3 and 4

  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             -vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator-(uint64 u, const sc_unsigned& v)
{

  if (u == 0) // case 1
    return sc_unsigned(v, -v.sgn);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // cases 3 and 4
  return add_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             -v.sgn, v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator-(const sc_unsigned &u, long v)
{

  if (v == 0) // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO) // case 2
    return sc_unsigned(-vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             -vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator-(long u, const sc_unsigned& v)
{

  if (u == 0) // case 1
    return sc_unsigned(v, -v.sgn);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // cases 3 and 4
  return add_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             -v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator-(const sc_unsigned &u, unsigned long v)
{

  if (v == 0) // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO) // case 2
    return sc_unsigned(-vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // cases 3 and 4
  return add_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             -vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator-(unsigned long u, const sc_unsigned& v)
{
  if (u == 0) // case 1
    return sc_unsigned(v, -v.sgn);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO) // case 2
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // cases 3 and 4
  return add_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             -v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: MULTIPLICATION operators: *, *=
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u * v:
// 1. u * 0 = 0 * v = 0
// 2. 1 * v = v and -1 * v = -v
// 3. u * 1 = u and u * -1 = -u
// 4. u * v = u * v

sc_unsigned
operator*(const sc_unsigned& u, const sc_signed& v)
{
 
  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator*(const sc_signed& u, const sc_unsigned& v)
{
 
  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator*(const sc_unsigned& u, const sc_unsigned& v)
{
 
  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator*(const sc_unsigned& u, int64 v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_INT64_2(v);

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator*(int64 u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_INT64_2(u);

  // cases 2-4
  return mul_unsigned_friend(s, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator*(const sc_unsigned& u, uint64 v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_INT64_2(v);

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator*(uint64 u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_INT64_2(u);

  // cases 2-4
  return mul_unsigned_friend(s, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator*(const sc_unsigned& u, long v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_LONG_2(v);

  // cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);
  
}


sc_unsigned
operator*(long u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_LONG_2(u);

  // cases 2-4
  return mul_unsigned_friend(s, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator*(const sc_unsigned& u, unsigned long v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_LONG_2(v);

  // else cases 2-4
  return mul_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);
  
}

sc_unsigned
operator*(unsigned long u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) // case 1
    return sc_unsigned();

  CONVERT_LONG_2(u);

  // cases 2-4
  return mul_unsigned_friend(s, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: DIVISION operators: /, /=
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when finding the quotient q = floor(u/v):
// Note that u = q * v + r for r < q.
// 1. 0 / 0 or u / 0 => error
// 2. 0 / v => 0 = 0 * v + 0
// 3. u / v && u = v => u = 1 * u + 0  - u or v can be 1 or -1
// 4. u / v && u < v => u = 0 * v + u  - u can be 1 or -1
// 5. u / v && u > v => u = q * v + r  - v can be 1 or -1

sc_unsigned
operator/(const sc_unsigned& u, const sc_signed& v)
{

  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) {
    div_by_zero(v.sgn); // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator/(const sc_signed& u, const sc_unsigned& v)
{

  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) {
    div_by_zero(v.sgn); // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator/(const sc_unsigned& u, const sc_unsigned& v)
{

  small_type s = mul_signs(u.sgn, v.sgn);

  if (s == SC_ZERO) {
    div_by_zero(v.sgn); // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator/(const sc_unsigned& u, int64 v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(v);

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator/(int64 u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(u);

  // other cases
  return div_unsigned_friend(s, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator/(const sc_unsigned& u, uint64 v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(v);

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator/(uint64 u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2

  }

  CONVERT_INT64_2(u);

  // other cases
  return div_unsigned_friend(s, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator/(const sc_unsigned& u, long v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(v);

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);
  
}


sc_unsigned
operator/(long u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(u);

  // other cases
  return div_unsigned_friend(s, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}


sc_unsigned
operator/(const sc_unsigned& u, unsigned long v)
{

  small_type s = mul_signs(u.sgn, get_sign(v));

  if (s == SC_ZERO) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(v);

  // other cases
  return div_unsigned_friend(s, u.nbits, u.ndigits, u.digit, 
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);
  
}


sc_unsigned
operator/(unsigned long u, const sc_unsigned& v)
{

  small_type s = mul_signs(v.sgn, get_sign(u));

  if (s == SC_ZERO) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2

  }

  CONVERT_LONG_2(u);

  // other cases
  return div_unsigned_friend(s, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, 
                             v.nbits, v.ndigits, v.digit);
  
}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: MOD operators: %, %=.
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when finding the remainder r = u % v:
// Note that u = q * v + r for r < q.
// 1. 0 % 0 or u % 0 => error
// 2. 0 % v => 0 = 0 * v + 0
// 3. u % v && u = v => u = 1 * u + 0  - u or v can be 1 or -1
// 4. u % v && u < v => u = 0 * v + u  - u can be 1 or -1
// 5. u % v && u > v => u = q * v + r  - v can be 1 or -1

sc_unsigned
operator%(const sc_unsigned& u, const sc_signed& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);
}


sc_unsigned
operator%(const sc_signed& u, const sc_unsigned& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);
}


sc_unsigned
operator%(const sc_unsigned& u, const sc_unsigned& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.nbits, v.ndigits, v.digit);
}


sc_unsigned
operator%(const sc_unsigned& u, int64 v)
{

  small_type vs = get_sign(v);

  if ((u.sgn == SC_ZERO) || (vs == SC_ZERO)) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(v);

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator%(int64 u, const sc_unsigned& v)
{

  small_type us = get_sign(u);

  if ((us == SC_ZERO) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(u);

  // other cases
  return mod_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator%(const sc_unsigned& u, uint64 v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64_2(v);

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator%(uint64 u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_INT64(u);

  // other cases
  return mod_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator%(const sc_unsigned& u, long v)
{

  small_type vs = get_sign(v);

  if ((u.sgn == SC_ZERO) || (vs == SC_ZERO)) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(v);

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);
}


sc_unsigned
operator%(long u, const sc_unsigned& v)
{

  small_type us = get_sign(u);

  if ((us == SC_ZERO) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(u);

  // other cases
  return mod_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator%(const sc_unsigned& u, unsigned long v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) {
    div_by_zero(v);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG_2(v);

  // other cases
  return mod_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator%(unsigned long u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) {
    div_by_zero(v.sgn);  // case 1
    return sc_unsigned();  // case 2
  }

  CONVERT_LONG(u);

  // other cases
  return mod_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise AND operators: &, &=
/////////////////////////////////////////////////////////////////////////////

// Cases to consider when computing u & v:
// 1. u & 0 = 0 & v = 0
// 2. u & v => sgn = +
// 3. (-u) & (-v) => sgn = -
// 4. u & (-v) => sgn = +
// 5. (-u) & v => sgn = +

sc_unsigned
operator&(const sc_unsigned& u, const sc_signed& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_signed& u, const sc_unsigned& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_unsigned& u, const sc_unsigned& v)
{

  if ((u.sgn == SC_ZERO) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_unsigned& u, int64 v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) // case 1
    return sc_unsigned();

  CONVERT_INT64(v);

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator&(int64 u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  CONVERT_INT64(u);

  // other cases
  return and_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_unsigned& u, uint64 v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) // case 1
    return sc_unsigned();

  CONVERT_INT64(v);

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);
  
}


sc_unsigned
operator&(uint64 u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  CONVERT_INT64(u);

  // other cases
  return and_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_unsigned& u, long v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) // case 1
    return sc_unsigned();

  CONVERT_LONG(v);

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator&(long u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  CONVERT_LONG(u);

  // other cases
  return and_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator&(const sc_unsigned& u, unsigned long v)
{

  if ((u.sgn == SC_ZERO) || (v == 0)) // case 1
    return sc_unsigned();

  CONVERT_LONG(v);

  // other cases
  return and_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator&(unsigned long u, const sc_unsigned& v)
{

  if ((u == 0) || (v.sgn == SC_ZERO)) // case 1
    return sc_unsigned();

  CONVERT_LONG(u);

  // other cases
  return and_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise OR operators: |, |=
/////////////////////////////////////////////////////////////////////////////
// Cases to consider when computing u | v:
// 1. u | 0 = u
// 2. 0 | v = v
// 3. u | v => sgn = +
// 4. (-u) | (-v) => sgn = -
// 5. u | (-v) => sgn = -
// 6. (-u) | v => sgn = -

sc_unsigned
operator|(const sc_unsigned& u, const sc_signed& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v, v.sgn);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_signed& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u, u.sgn);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_unsigned& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_unsigned& u, int64 v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator|(int64 u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // other cases
  return or_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_unsigned& u, uint64 v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator|(uint64 u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // other cases
  return or_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_unsigned& u, long v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator|(long u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // other cases
  return or_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator|(const sc_unsigned& u, unsigned long v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // other cases
  return or_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                            vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator|(unsigned long u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // other cases
  return or_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                            v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise XOR operators: ^, ^=
/////////////////////////////////////////////////////////////////////////////
// Cases to consider when computing u ^ v:
// Note that  u ^ v = (~u & v) | (u & ~v).
// 1. u ^ 0 = u
// 2. 0 ^ v = v
// 3. u ^ v => sgn = +
// 4. (-u) ^ (-v) => sgn = -
// 5. u ^ (-v) => sgn = -
// 6. (-u) ^ v => sgn = +

sc_unsigned
operator^(const sc_unsigned& u, const sc_signed& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v, v.sgn);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_signed& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u, u.sgn);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_unsigned& u, const sc_unsigned& v)
{

  if (v.sgn == SC_ZERO)  // case 1
    return sc_unsigned(u);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(v);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_unsigned& u, int64 v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}


sc_unsigned
operator^(int64 u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // other cases
  return xor_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_unsigned& u, uint64 v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_INT64(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd, false);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd);

}

sc_unsigned
operator^(uint64 u, const sc_unsigned& v)
{
  if (u == 0)
    return sc_unsigned(v);

  CONVERT_INT64(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud, false);

  // other cases
  return xor_unsigned_friend(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_unsigned& u, long v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}


sc_unsigned
operator^(long u, const sc_unsigned& v)
{

  if (u == 0)
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // other cases
  return xor_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}


sc_unsigned
operator^(const sc_unsigned& u, unsigned long v)
{

  if (v == 0)  // case 1
    return sc_unsigned(u);

  CONVERT_LONG(v);

  if (u.sgn == SC_ZERO)  // case 2
    return sc_unsigned(vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd, false);

  // other cases
  return xor_unsigned_friend(u.sgn, u.nbits, u.ndigits, u.digit,
                             vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd);

}

sc_unsigned
operator^(unsigned long u, const sc_unsigned& v)
{
  if (u == 0)
    return sc_unsigned(v);

  CONVERT_LONG(u);

  if (v.sgn == SC_ZERO)
    return sc_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud, false);

  // other cases
  return xor_unsigned_friend(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                             v.sgn, v.nbits, v.ndigits, v.digit);

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Bitwise NOT operator: ~
/////////////////////////////////////////////////////////////////////////////

// Operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: LEFT SHIFT operators: <<, <<=
/////////////////////////////////////////////////////////////////////////////

sc_unsigned
operator<<(const sc_unsigned& u, const sc_signed& v)
{
  if ((v.sgn == SC_ZERO) || (v.sgn == SC_NEG))
    return sc_unsigned(u);

  return operator<<(u, v.to_ulong());
}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: RIGHT SHIFT operators: >>, >>=
/////////////////////////////////////////////////////////////////////////////

sc_unsigned
operator>>(const sc_unsigned& u, const sc_signed& v)
{

  if ((v.sgn == SC_ZERO) || (v.sgn == SC_NEG))
    return sc_unsigned(u);

  return operator>>(u, v.to_ulong());

}

// The rest of the operators in this section are included from
// sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Unary arithmetic operators.
/////////////////////////////////////////////////////////////////////////////

sc_unsigned
operator+(const sc_unsigned& u)
{
  return sc_unsigned(u);
}

sc_unsigned
operator-(const sc_unsigned& u)
{
  return sc_unsigned(u, -u.sgn);
}

#endif  // END OF OLD SEMANTICS

/////////////////////////////////////////////////////////////////////////////
// SECTION: EQUAL operator: ==
/////////////////////////////////////////////////////////////////////////////

bool
operator==(const sc_unsigned& u, const sc_unsigned& v)
{

  if (&u == &v)
    return true;

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       v.sgn, v.nbits, v.ndigits, v.digit) != 0)
    return false;

  return true;

}


bool
operator==(const sc_unsigned& u, const sc_signed& v)
{

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       v.sgn, v.nbits, v.ndigits, v.digit, 0, 1) != 0)
    return false;

  return true;

}


bool
operator==(const sc_signed& u, const sc_unsigned& v)
{

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       v.sgn, v.nbits, v.ndigits, v.digit, 1, 0) != 0)
    return false;

  return true;

}


bool
operator==(const sc_unsigned& u, int64 v)
{

  CONVERT_INT64(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd) != 0)
    return false;

  return true;
  
}


bool
operator==(int64 u, const sc_unsigned& v)
{

  CONVERT_INT64(u);

  if (compare_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) != 0)
    return false;

  return true;
  
}


bool
operator==(const sc_unsigned& u, uint64 v)
{

  CONVERT_INT64(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd) != 0)
    return false;

  return true;
  
}


bool
operator==(uint64 u, const sc_unsigned& v)
{

  CONVERT_INT64(u);

  if (compare_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) != 0)
    return false;

  return true;
  
}


bool
operator==(const sc_unsigned& u, long v)
{

  CONVERT_LONG(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd) != 0)
    return false;

  return true;
  
}


bool
operator==(long u, const sc_unsigned& v)
{

  CONVERT_LONG(u);

  if (compare_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) != 0)
    return false;

  return true;
  
}


bool
operator==(const sc_unsigned& u, unsigned long v)
{

  CONVERT_LONG(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd) != 0)
    return false;

  return true;
  
}


bool
operator==(unsigned long u, const sc_unsigned& v)
{

  CONVERT_LONG(u);

  if (compare_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) != 0)
    return false;

  return true;
  
}


/////////////////////////////////////////////////////////////////////////////
// SECTION: NOT_EQUAL operator: !=
/////////////////////////////////////////////////////////////////////////////


bool
operator!=(const sc_unsigned& u, const sc_signed& v)
{
  return (! operator==(u, v));
}


bool
operator!=(const sc_signed& u, const sc_unsigned& v)
{
  return (! operator==(u, v));
}

// The rest of the operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: LESS THAN operator: <
/////////////////////////////////////////////////////////////////////////////


bool
operator<(const sc_unsigned& u, const sc_unsigned& v)
{

  if (&u == &v)
    return false;

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       v.sgn, v.nbits, v.ndigits, v.digit) < 0)
    return true;

  return false;

}


bool
operator<(const sc_unsigned& u, const sc_signed& v)
{

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       v.sgn, v.nbits, v.ndigits, v.digit, 0, 1) < 0)
    return true;

  return false;

}


bool
operator<(const sc_signed& u, const sc_unsigned& v)
{

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       v.sgn, v.nbits, v.ndigits, v.digit, 1, 0) < 0)
    return true;

  return false;

}


bool
operator<(const sc_unsigned& u, int64 v)
{

  CONVERT_INT64(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd) < 0)
    return true;

  return false;

}


bool
operator<(int64 u, const sc_unsigned& v)
{

  CONVERT_INT64(u);

  if (compare_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) < 0)
    return true;

  return false;

}


bool
operator<(const sc_unsigned& u, uint64 v)
{

  CONVERT_INT64(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_UINT64, DIGITS_PER_UINT64, vd) < 0)
    return true;

  return false;

}


bool
operator<(uint64 u, const sc_unsigned& v)
{

  CONVERT_INT64(u);

  if (compare_unsigned(us, BITS_PER_UINT64, DIGITS_PER_UINT64, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) < 0)
    return true;

  return false;    

}


bool
operator<(const sc_unsigned& u, long v)
{

  CONVERT_LONG(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd) < 0)
    return true;

  return false;

}


bool
operator<(long u, const sc_unsigned& v)
{

  CONVERT_LONG(u);

  if (compare_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) < 0)
    return true;

  return false;

}


bool
operator<(const sc_unsigned& u, unsigned long v)
{

  CONVERT_LONG(v);

  if (compare_unsigned(u.sgn, u.nbits, u.ndigits, u.digit, 
                       vs, BITS_PER_ULONG, DIGITS_PER_ULONG, vd) < 0)
    return true;

  return false;

}


bool
operator<(unsigned long u, const sc_unsigned& v)
{

  CONVERT_LONG(u);

  if (compare_unsigned(us, BITS_PER_ULONG, DIGITS_PER_ULONG, ud,
                       v.sgn, v.nbits, v.ndigits, v.digit) < 0)
    return true;

  return false;    

}


/////////////////////////////////////////////////////////////////////////////
// SECTION: LESS THAN or EQUAL operator: <=
/////////////////////////////////////////////////////////////////////////////


bool
operator<=(const sc_unsigned& u, const sc_signed& v)
{
  return (operator<(u, v) || operator==(u, v));
}


bool
operator<=(const sc_signed& u, const sc_unsigned& v)
{
  return (operator<(u, v) || operator==(u, v));
}

// The rest of the operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: GREATER THAN operator: >
/////////////////////////////////////////////////////////////////////////////


bool
operator>(const sc_unsigned& u, const sc_signed& v)
{
  return (! (operator<=(u, v)));
}


bool
operator>(const sc_signed& u, const sc_unsigned& v)
{
  return (! (operator<=(u, v)));
}

// The rest of the operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: GREATER THAN or EQUAL operator: >=
/////////////////////////////////////////////////////////////////////////////


bool
operator>=(const sc_unsigned& u, const sc_signed& v)
{
  return (! (operator<(u, v)));
}


bool
operator>=(const sc_signed& u, const sc_unsigned& v)
{
  return (! (operator<(u, v)));
}

// The rest of the operators in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Friends 
/////////////////////////////////////////////////////////////////////////////

// Compare u and v as unsigned and return r
//  r = 0 if u == v
//  r < 0 if u < v
//  r > 0 if u > v
length_type
compare_unsigned(small_type us, 
                 length_type unb, length_type und, const digit_type *ud, 
                 small_type vs, 
                 length_type vnb, length_type vnd, const digit_type *vd,
                 small_type if_u_signed, 
                 small_type if_v_signed)
{

  if (us == vs) {

    if (us == SC_ZERO)
      return 0;

    else {

      length_type cmp_res = vec_skip_and_cmp(und, ud, vnd, vd);

      if (us == SC_POS)
        return cmp_res;
      else
        return -cmp_res;

    }
  }
  else {

    if (us == SC_ZERO)
      return -vs;

    if (vs == SC_ZERO)
      return us;

    length_type cmp_res;

    length_type nd = (us == SC_NEG ? und : vnd);

#ifdef MAX_NBITS
    digit_type d[MAX_NDIGITS];
#else
    digit_type *d = new digit_type[nd];
#endif

    if (us == SC_NEG) {

      vec_copy(nd, d, ud);
      vec_complement(nd, d);
      trim(if_u_signed, unb, nd, d);
      cmp_res = vec_skip_and_cmp(nd, d, vnd, vd);

    }
    else {

      vec_copy(nd, d, vd);
      vec_complement(nd, d);
      trim(if_v_signed, vnb, nd, d);
      cmp_res = vec_skip_and_cmp(und, ud, nd, d);

    }

#ifndef MAX_NBITS
    delete [] d;
#endif

    return cmp_res;

  }
}


/////////////////////////////////////////////////////////////////////////////
// SECTION: Public members - Other utils.
/////////////////////////////////////////////////////////////////////////////


bool 
sc_unsigned::iszero() const
{

  if (sgn == SC_ZERO)
    return true;

  else if (sgn == SC_NEG) {

    // A negative unsigned number can be zero, e.g., -16 in 4 bits, so
    // check that.

#ifdef MAX_NBITS
    digit_type d[MAX_NDIGITS];
#else
    digit_type *d = new digit_type[ndigits];
#endif

    vec_copy(ndigits, d, digit);
    vec_complement(ndigits, d);
    trim_unsigned(nbits, ndigits, d);

    bool res = check_for_zero(ndigits, d);

#ifndef MAX_NBITS
    delete [] d;
#endif

    return res;
    
  }
  else
    return false;

}

// The rest of the utils in this section are included from sc_nbcommon.cpp.

/////////////////////////////////////////////////////////////////////////////
// SECTION: Private members.
/////////////////////////////////////////////////////////////////////////////

// The private members in this section are included from
// sc_nbcommon.cpp.

#define CLASS_TYPE sc_unsigned

#define ADD_HELPER add_unsigned_friend
#define SUB_HELPER sub_unsigned_friend
#define MUL_HELPER mul_unsigned_friend
#define DIV_HELPER div_unsigned_friend
#define MOD_HELPER mod_unsigned_friend
#define AND_HELPER and_unsigned_friend
#define  OR_HELPER  or_unsigned_friend
#define XOR_HELPER xor_unsigned_friend 

#include "sc_nbfriends.cpp"

#undef  SC_SIGNED
#define SC_UNSIGNED
#define IF_SC_SIGNED              0  // 0 = sc_unsigned
#define CLASS_TYPE_SUBREF         sc_unsigned_subref
#define OTHER_CLASS_TYPE          sc_signed
#define OTHER_CLASS_TYPE_SUBREF   sc_signed_subref

#define MUL_ON_HELPER mul_on_help_unsigned
#define DIV_ON_HELPER div_on_help_unsigned
#define MOD_ON_HELPER mod_on_help_unsigned

#include "sc_nbcommon.cpp"

#undef MOD_ON_HELPER
#undef DIV_ON_HELPER
#undef MUL_ON_HELPER

#undef OTHER_CLASS_TYPE_SUBREF
#undef OTHER_CLASS_TYPE
#undef CLASS_TYPE_SUBREF
#undef IF_SC_SIGNED
#undef SC_UNSIGNED

#undef XOR_HELPER
#undef  OR_HELPER
#undef AND_HELPER
#undef MOD_HELPER
#undef DIV_HELPER
#undef MUL_HELPER
#undef SUB_HELPER
#undef ADD_HELPER

#undef CLASS_TYPE

#include "sc_unsigned_bitref.cpp"
#include "sc_unsigned_subref.cpp"

#undef CONVERT_LONG
#undef CONVERT_LONG_2
#undef CONVERT_INT64
#undef CONVERT_INT64_2

// End of file.

