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


/***************************************************************************

    sc_signed.h -- Arbitrary precision signed arithmetic.
 
    This file includes the definitions of sc_signed_bitref,
    sc_signed_subref, and sc_signed classes. The first two classes are
    proxy classes to reference one bit and a range of bits of a
    sc_signed number, respectively.

    An sc_signed number has the sign-magnitude representation
    internally. However, its interface guarantees a 2's-complement
    representation. The sign-magnitude representation is chosen
    because of its efficiency: The sc_signed and sc_unsigned types are
    optimized for arithmetic rather than bitwise operations. For
    arithmetic operations, the sign-magnitude representation performs
    better.

    The implementations of sc_signed and sc_unsigned classes are
    almost identical: Most of the member and friend functions are
    defined in sc_nbcommon.cpp and sc_nbfriends.cpp so that they can
    be shared by both of these classes. These functions are chosed by
    defining a few macros before including them such as IF_SC_SIGNED
    and CLASS_TYPE. Our implementation choices are mostly dictated by
    performance considerations in that we tried to provide the most
    efficient sc_signed and sc_unsigned types without compromising
    their interface. 
  
    Original Author: Ali Dasdan. Synopsys, Inc. (dasdan@synopsys.com)
 
  **************************************************************************/


/******************************************************************************

    MODIFICATION LOG - modifiers, enter your name, affliation and
    changes you are making here:

    Modifier Name & Affiliation:
    Description of Modification:
    

******************************************************************************/


#ifndef SC_SIGNED_H
#define SC_SIGNED_H

#ifndef _MSC_VER
#include <iostream>
using std::ostream;
using std::istream;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::hex;
#else
// MSVC6.0 has bugs in standard library
#include <iostream.h>
#endif
#ifdef __BCPLUSPLUS__
#pragma hdrstop
#endif

#include "sc_nbdefs.h"
#include "sc_nbutils.h"
#include "sc_nbexterns.h"

template< class A > class sc_2d;
class sc_logic_vector;
class sc_bool_vector;

class sc_signed;
class sc_unsigned;
class sc_unsigned_subref;
#ifdef SC_INCLUDE_FX
class sc_fxval;
class sc_fxval_fast;
class sc_fxnum;
class sc_fxnum_fast;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLASS: sc_signed_bitref -- Proxy class to reference a single bit.
/////////////////////////////////////////////////////////////////////////////

class sc_signed_bitref {

  friend class sc_signed;

  friend istream& operator >> (istream& is, sc_signed_bitref&       u);
  friend ostream& operator << (ostream& os, const sc_signed_bitref& u);

public:

  sc_signed_bitref& operator = (const sc_signed_bitref& v);
  sc_signed_bitref& operator = (bool v);

  sc_signed_bitref& operator &= (bool v);
  sc_signed_bitref& operator |= (bool v);
  sc_signed_bitref& operator ^= (bool v);

  operator bool() const;
  bool operator ~ () const;

private:

  sc_signed* snum;  // Signed number.
  length_type index;

  sc_signed_bitref(sc_signed* u, length_type i) { snum = u; index = i; }
  
};
// gcc 2.95.2 bug - bool comparison mapped into int comparison
// must provide explicit conversion, otherwise gcc is confused
//inline bool operator==(bool b,sc_signed_bitref& t){return t==b;}

/////////////////////////////////////////////////////////////////////////////
// CLASS: sc_signed_subref -- Proxy class to reference a range of bits. 
/////////////////////////////////////////////////////////////////////////////

class sc_signed_subref {

  friend class sc_signed;

  friend istream& operator >> (istream& is, sc_signed_subref&       u);
  friend ostream& operator << (ostream& os, const sc_signed_subref& u);
  
public:

  sc_signed_subref& operator = (const sc_signed_subref&   v);
  sc_signed_subref& operator = (const sc_signed&          v);
  sc_signed_subref& operator = (const sc_unsigned_subref& v);
  sc_signed_subref& operator = (const sc_unsigned&        v);
  sc_signed_subref& operator = (const char*               v);
  sc_signed_subref& operator = (int64                     v);
  sc_signed_subref& operator = (uint64                    v);
  sc_signed_subref& operator = (long                      v);
  sc_signed_subref& operator = (unsigned long             v);
  sc_signed_subref& operator = (int                       v) 
    { return operator=((long) v); }
  sc_signed_subref& operator = (unsigned int              v) 
    { return operator=((unsigned long) v); }
  sc_signed_subref& operator = (double                    v);  

  operator sc_signed() const;

private:

  sc_signed* snum;  // Signed number.
  length_type left, right;

  sc_signed_subref(sc_signed* u, length_type l, length_type r)
    { snum = u; left = l; right = r; }

};

/////////////////////////////////////////////////////////////////////////////
// CLASS: sc_signed -- Arbitrary precision signed number.
/////////////////////////////////////////////////////////////////////////////

class sc_signed {

  friend class sc_signed_bitref;
  friend class sc_signed_subref;
  friend class sc_unsigned;
  friend class sc_unsigned_subref;
  friend class sc_2d<sc_signed>;

  friend istream& operator >> (istream& is, sc_signed&       u);
  friend ostream& operator << (ostream& os, const sc_signed& u);

  // Needed for types using sc_signed.
  typedef bool elemtype;

public:

  // Public constructor.
  explicit sc_signed(int nb);
  explicit sc_signed(const char *v);

  // Public copy constructors.
  sc_signed(const sc_signed&   v);
  sc_signed(const sc_unsigned& v);

  // Assignment operators.
  sc_signed& operator = (const sc_signed&          v);
  sc_signed& operator = (const sc_signed_subref&   v);
  sc_signed& operator = (const sc_unsigned&        v);
  sc_signed& operator = (const sc_unsigned_subref& v);
  sc_signed& operator = (const char*               v);
  sc_signed& operator = (int64                     v);
  sc_signed& operator = (uint64                    v);
  sc_signed& operator = (long                      v);
  sc_signed& operator = (unsigned long             v);
  sc_signed& operator = (int                       v) 
    { return operator=((long) v); }
  sc_signed& operator = (unsigned int              v) 
    { return operator=((unsigned long) v); }
  sc_signed& operator = (double                    v);

#ifdef SC_INCLUDE_FX
  // Interfacing with fixed point types.
  sc_signed& operator = ( const sc_fxval&      v );
  sc_signed& operator = ( const sc_fxval_fast& v );
  sc_signed& operator = ( const sc_fxnum&      v );
  sc_signed& operator = ( const sc_fxnum_fast& v );
#endif

  // Destructor.
  ~sc_signed() 
    { 
#ifndef MAX_NBITS
      delete [] digit; 
#endif
    }

#ifdef SC_LOGIC_VECTOR_H
  // Interfacing with sc_logic_ and sc_bool_ vectors.
  sc_signed(const sc_logic_vector& v);
  sc_signed(const sc_bool_vector&  v);
  sc_signed& operator = (const sc_logic_vector& v);
  sc_signed& operator = (const sc_bool_vector&  v);
#endif

  // Increment operators.
  sc_signed& operator ++ ();
  const sc_signed operator ++ (int);

  // Decrement operators.
  sc_signed& operator -- ();
  const sc_signed operator -- (int);

  // Bitref operators. Help access the ith bit.
  sc_signed_bitref operator [] (int i)
    { return sc_signed_bitref(this, i); }

  const bool operator [] (int i) const
    { return test(i); }

  // Subref operators. Help access the range of bits from the ith to
  // jth. These indices have arbitrary precedence with respect to each
  // other, i.e., we can have i <= j or i > j. Note the equivalence
  // between range(i, j) and operator(i, j). Also note that
  // operator(i, i) returns a signed number that corresponds to the
  // bit operator[i], so these two forms are not the same.
  sc_signed_subref range(int i, int j)
    { return sc_signed_subref(this, i, j); }

  const sc_signed range(int i, int j) const
    { return sc_signed(this, i, j); }

  sc_signed_subref operator () (int i, int j)
    { return sc_signed_subref(this, i, j); }

  const sc_signed operator () (int i, int j) const
    { return sc_signed(this, i, j); }
  
  // Conversion functions: to convert to (mostly) built-in types in
  // that to_X converts to a number of type X.
  sc_string     to_string(sc_numrep base = SC_DEC, 
                          bool formatted = false) const;
  sc_string     to_string(int base, bool formatted = false) const
    { return to_string(sc_numrep(base), formatted); }

  int64         to_int64() const;
  uint64        to_uint64() const;
  long          to_long() const;
  unsigned long to_ulong() const;
  unsigned long to_unsigned_long() const { return to_ulong(); } // For VSI.
  int           to_int() const;
  int           to_signed() const { return to_int(); }
  unsigned int  to_uint() const;
  unsigned int  to_unsigned() const { return to_uint(); }
  unsigned int  to_unsigned_int() const { return to_uint(); }   // For VSI.
  double        to_double() const;

  // Print functions. dump prints the internals of the class.
  void print() const { print(cout); }
  void print(ostream &os) const;
  void dump() const { dump(cout); }
  void dump(ostream &os) const;

  // Functions to find various properties.
  int  length() const { return nbits; }  // Bit width.
  bool iszero() const;                   // Is the number zero?
  bool sign() const;                     // Sign.

  // Functions to access individual bits. 
  bool test(length_type i) const;      // Is the ith bit 0 or 1?
  void set(length_type i);             // Set the ith bit to 1.
  void clear(length_type i);           // Set the ith bit to 0.
  void set(length_type i, bool v)      // Set the ith bit to v.
    { if (v) set(i); else clear(i);  }
  void invert(length_type i)           // Negate the ith bit.
    { if (test(i)) clear(i); else set(i);  }

  // Make the number equal to its mirror image.
  void reverse();

  // Get/set a packed bit representation of the number.
  void get_packed_rep(digit_type *buf) const;
  void set_packed_rep(digit_type *buf);

  /*
    The comparison of the old and new semantics are as follows:

    Let s = sc_signed, 
        u = sc_unsigned,
        un = { uint64, unsigned long, unsigned int },
        sn = { int64, long, int, char* }, and
        OP = { +, -, *, /, % }.

    Old semantics:                     New semantics:
      u OP u -> u                        u OP u -> u
      s OP u -> u                        s OP u -> s
      u OP s -> u                        u OP s -> s
      s OP s -> s                        s OP s -> s

      u OP un = un OP u -> u             u OP un = un OP u -> u
      u OP sn = sn OP u -> u             u OP sn = sn OP u -> s

      s OP un = un OP s -> s             s OP un = un OP s -> s
      s OP sn = sn OP s -> s             s OP sn = sn OP s -> s

    In the new semantics, the result is u if both operands are u; the
    result is s otherwise. The only exception is subtraction. The result
    of a subtraction is always s.
  */

#ifdef NEW_SEMANTICS

  // ARITHMETIC OPERATORS:

  // ADDition operators:

  friend sc_signed operator + (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator + (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator + (const sc_unsigned& u, int64              v); 
  friend sc_signed operator + (const sc_unsigned& u, long               v); 
  friend sc_signed operator + (const sc_unsigned& u, int                v) 
    { return operator+(u, (long) v); }

  friend sc_signed operator + (int64              u, const sc_unsigned& v); 
  friend sc_signed operator + (long               u, const sc_unsigned& v); 
  friend sc_signed operator + (int                u, const sc_unsigned& v)  
    { return operator+((long) u, v); } 

  friend sc_signed operator + (const sc_signed&   u, const sc_signed&   v);
  friend sc_signed operator + (const sc_signed&   u, int64              v); 
  friend sc_signed operator + (const sc_signed&   u, uint64             v); 
  friend sc_signed operator + (const sc_signed&   u, long               v); 
  friend sc_signed operator + (const sc_signed&   u, unsigned long      v);
  friend sc_signed operator + (const sc_signed&   u, int                v) 
    { return operator+(u, (long) v); }
  friend sc_signed operator + (const sc_signed&   u, unsigned int       v) 
    { return operator+(u, (unsigned long) v); }

  friend sc_signed operator + (int64              u, const sc_signed&   v); 
  friend sc_signed operator + (uint64             u, const sc_signed&   v); 
  friend sc_signed operator + (long               u, const sc_signed&   v); 
  friend sc_signed operator + (unsigned long      u, const sc_signed&   v);
  friend sc_signed operator + (int                u, const sc_signed&   v)  
    { return operator+((long) u, v); } 
  friend sc_signed operator + (unsigned int       u, const sc_signed&   v)  
    { return operator+((unsigned long) u, v); } 

  sc_signed& operator += (const sc_signed&   v); 
  sc_signed& operator += (const sc_unsigned& v); 
  sc_signed& operator += (int64              v); 
  sc_signed& operator += (uint64             v); 
  sc_signed& operator += (long               v); 
  sc_signed& operator += (unsigned long      v); 
  sc_signed& operator += (int                v) 
    { return operator+=((long) v); }
  sc_signed& operator += (unsigned int       v) 
    { return operator+=((unsigned long) v); }

  // SUBtraction operators:
   
  friend sc_signed operator - (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator - (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator - (const sc_unsigned& u, const sc_unsigned& v);
  friend sc_signed operator - (const sc_unsigned& u, int64              v); 
  friend sc_signed operator - (const sc_unsigned& u, uint64             v); 
  friend sc_signed operator - (const sc_unsigned& u, long               v); 
  friend sc_signed operator - (const sc_unsigned& u, unsigned long      v);
  friend sc_signed operator - (const sc_unsigned& u, int                v) 
    { return operator-(u, (long) v); }
  friend sc_signed operator - (const sc_unsigned& u, unsigned int       v) 
    { return operator-(u, (unsigned long) v); }

  friend sc_signed operator - (int64              u, const sc_unsigned& v); 
  friend sc_signed operator - (uint64             u, const sc_unsigned& v); 
  friend sc_signed operator - (long               u, const sc_unsigned& v); 
  friend sc_signed operator - (unsigned long      u, const sc_unsigned& v);
  friend sc_signed operator - (int                u, const sc_unsigned& v)  
    { return operator-((long) u, v); } 
  friend sc_signed operator - (unsigned int       u, const sc_unsigned& v)  
    { return operator-((unsigned long) u, v); } 

  friend sc_signed operator - (const sc_signed&   u, const sc_signed&   v);
  friend sc_signed operator - (const sc_signed&   u, int64              v); 
  friend sc_signed operator - (const sc_signed&   u, uint64             v); 
  friend sc_signed operator - (const sc_signed&   u, long               v); 
  friend sc_signed operator - (const sc_signed&   u, unsigned long      v);
  friend sc_signed operator - (const sc_signed&   u, int                v) 
    { return operator-(u, (long) v); }
  friend sc_signed operator - (const sc_signed&   u, unsigned int       v) 
    { return operator-(u, (unsigned long) v); }

  friend sc_signed operator - (int64              u, const sc_signed&   v); 
  friend sc_signed operator - (uint64             u, const sc_signed&   v); 
  friend sc_signed operator - (long               u, const sc_signed&   v); 
  friend sc_signed operator - (unsigned long      u, const sc_signed&   v);
  friend sc_signed operator - (int                u, const sc_signed&   v)  
    { return operator-((long) u, v); } 
  friend sc_signed operator - (unsigned int       u, const sc_signed&   v)  
    { return operator-((unsigned long) u, v); } 

  sc_signed& operator -= (const sc_signed&   v); 
  sc_signed& operator -= (const sc_unsigned& v); 
  sc_signed& operator -= (int64              v); 
  sc_signed& operator -= (uint64             v); 
  sc_signed& operator -= (long               v); 
  sc_signed& operator -= (unsigned long      v); 
  sc_signed& operator -= (int                v) 
    { return operator -= ((long) v); }
  sc_signed& operator -= (unsigned int       v) 
    { return operator -= ((unsigned long) v); }

  // MULtiplication operators:
   
  friend sc_signed operator * (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator * (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator * (const sc_unsigned& u, int64              v); 
  friend sc_signed operator * (const sc_unsigned& u, long               v); 
  friend sc_signed operator * (const sc_unsigned& u, int                v) 
    { return operator*(u, (long) v); }

  friend sc_signed operator * (int64              u, const sc_unsigned& v); 
  friend sc_signed operator * (long               u, const sc_unsigned& v); 
  friend sc_signed operator * (int                u, const sc_unsigned& v)  
    { return operator*((long) u, v); } 

  friend sc_signed operator * (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator * (const sc_signed& u, int64            v); 
  friend sc_signed operator * (const sc_signed& u, uint64           v); 
  friend sc_signed operator * (const sc_signed& u, long             v); 
  friend sc_signed operator * (const sc_signed& u, unsigned long    v);
  friend sc_signed operator * (const sc_signed& u, int              v) 
    { return operator*(u, (long) v); }
  friend sc_signed operator * (const sc_signed& u, unsigned int     v) 
    { return operator*(u, (unsigned long) v); }

  friend sc_signed operator * (int64            u, const sc_signed& v); 
  friend sc_signed operator * (uint64           u, const sc_signed& v); 
  friend sc_signed operator * (long             u, const sc_signed& v); 
  friend sc_signed operator * (unsigned long    u, const sc_signed& v);
  friend sc_signed operator * (int              u, const sc_signed& v)  
    { return operator*((long) u, v); } 
  friend sc_signed operator * (unsigned int     u, const sc_signed& v)  
    { return operator*((unsigned long) u, v); } 

  sc_signed& operator *= (const sc_signed&   v); 
  sc_signed& operator *= (const sc_unsigned& v); 
  sc_signed& operator *= (int64              v); 
  sc_signed& operator *= (uint64             v); 
  sc_signed& operator *= (long               v); 
  sc_signed& operator *= (unsigned long      v); 
  sc_signed& operator *= (int                v) 
    { return operator*=((long) v); }
  sc_signed& operator *= (unsigned int       v) 
    { return operator*=((unsigned long) v); }

  // DIVision operators:
   
  friend sc_signed operator / (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator / (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator / (const sc_unsigned& u, int64              v); 
  friend sc_signed operator / (const sc_unsigned& u, long               v); 
  friend sc_signed operator / (const sc_unsigned& u, int                v) 
    { return operator/(u, (long) v); }

  friend sc_signed operator / (int64              u, const sc_unsigned& v); 
  friend sc_signed operator / (long               u, const sc_unsigned& v); 
  friend sc_signed operator / (int                u, const sc_unsigned& v)  
    { return operator/((long) u, v); } 

  friend sc_signed operator / (const sc_signed&   u, const sc_signed&   v);
  friend sc_signed operator / (const sc_signed&   u, int64              v); 
  friend sc_signed operator / (const sc_signed&   u, uint64             v); 
  friend sc_signed operator / (const sc_signed&   u, long               v); 
  friend sc_signed operator / (const sc_signed&   u, unsigned long      v);
  friend sc_signed operator / (const sc_signed&   u, int                v) 
    { return operator/(u, (long) v); }
  friend sc_signed operator / (const sc_signed&   u, unsigned int       v) 
    { return operator/(u, (unsigned long) v); }

  friend sc_signed operator / (int64              u, const sc_signed&   v); 
  friend sc_signed operator / (uint64             u, const sc_signed&   v); 
  friend sc_signed operator / (long               u, const sc_signed&   v); 
  friend sc_signed operator / (unsigned long      u, const sc_signed&   v);
  friend sc_signed operator / (int                u, const sc_signed&   v)  
    { return operator/((long) u, v); } 
  friend sc_signed operator / (unsigned int       u, const sc_signed&   v)  
    { return operator/((unsigned long) u, v); } 

  sc_signed& operator /= (const sc_signed&   v); 
  sc_signed& operator /= (const sc_unsigned& v); 
  sc_signed& operator /= (int64              v); 
  sc_signed& operator /= (uint64             v); 
  sc_signed& operator /= (long               v); 
  sc_signed& operator /= (unsigned long      v); 
  sc_signed& operator /= (int                v) 
    { return operator/=((long) v); }
  sc_signed& operator /= (unsigned int       v) 
    { return operator/=((unsigned long) v); }

  // MODulo operators:
   
  friend sc_signed operator % (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator % (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator % (const sc_unsigned& u, int64              v); 
  friend sc_signed operator % (const sc_unsigned& u, long               v); 
  friend sc_signed operator % (const sc_unsigned& u, int                v) 
    { return operator%(u, (long) v); }

  friend sc_signed operator % (int64              u, const sc_unsigned& v); 
  friend sc_signed operator % (long               u, const sc_unsigned& v); 
  friend sc_signed operator % (int                u, const sc_unsigned& v)  
    { return operator%((long) u, v); } 

  friend sc_signed operator % (const sc_signed&   u, const sc_signed&   v);
  friend sc_signed operator % (const sc_signed&   u, int64              v); 
  friend sc_signed operator % (const sc_signed&   u, uint64             v); 
  friend sc_signed operator % (const sc_signed&   u, long               v); 
  friend sc_signed operator % (const sc_signed&   u, unsigned long      v);
  friend sc_signed operator % (const sc_signed&   u, int                v) 
    { return operator%(u, (long) v); }
  friend sc_signed operator % (const sc_signed&   u, unsigned int       v) 
    { return operator%(u, (unsigned long) v); }

  friend sc_signed operator % (int64              u, const sc_signed&   v); 
  friend sc_signed operator % (uint64             u, const sc_signed&   v); 
  friend sc_signed operator % (long               u, const sc_signed&   v); 
  friend sc_signed operator % (unsigned long      u, const sc_signed&   v);
  friend sc_signed operator % (int                u, const sc_signed&   v)  
    { return operator%((long) u, v); } 
  friend sc_signed operator % (unsigned int       u, const sc_signed&   v)  
    { return operator%((unsigned long) u, v); } 

  sc_signed& operator %= (const sc_signed&   v); 
  sc_signed& operator %= (const sc_unsigned& v); 
  sc_signed& operator %= (int64              v); 
  sc_signed& operator %= (uint64             v); 
  sc_signed& operator %= (long               v); 
  sc_signed& operator %= (unsigned long      v); 
  sc_signed& operator %= (int                v) 
    { return operator%=((long) v); }
  sc_signed& operator %= (unsigned int       v) 
    { return operator%=((unsigned long) v); }

  // BITWISE OPERATORS:

  // Bitwise AND operators:
   
  friend sc_signed operator & (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator & (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator & (const sc_unsigned& u, int64              v); 
  friend sc_signed operator & (const sc_unsigned& u, long               v); 
  friend sc_signed operator & (const sc_unsigned& u, int                v) 
    { return operator&(u, (long) v); }

  friend sc_signed operator & (int64              u, const sc_unsigned& v); 
  friend sc_signed operator & (long               u, const sc_unsigned& v); 
  friend sc_signed operator & (int                u, const sc_unsigned& v)  
    { return operator&((long) u, v); } 

  friend sc_signed operator & (const sc_signed&   u, const sc_signed&   v);
  friend sc_signed operator & (const sc_signed&   u, int64              v); 
  friend sc_signed operator & (const sc_signed&   u, uint64             v); 
  friend sc_signed operator & (const sc_signed&   u, long               v); 
  friend sc_signed operator & (const sc_signed&   u, unsigned long      v);
  friend sc_signed operator & (const sc_signed&   u, int                v) 
    { return operator&(u, (long) v); }
  friend sc_signed operator & (const sc_signed&   u, unsigned int       v) 
    { return operator&(u, (unsigned long) v); }

  friend sc_signed operator & (int64            u, const sc_signed& v); 
  friend sc_signed operator & (uint64           u, const sc_signed& v); 
  friend sc_signed operator & (long             u, const sc_signed& v); 
  friend sc_signed operator & (unsigned long    u, const sc_signed& v);
  friend sc_signed operator & (int              u, const sc_signed& v)  
    { return operator&((long) u, v); } 
  friend sc_signed operator & (unsigned int     u, const sc_signed& v)  
    { return operator&((unsigned long) u, v); } 

  sc_signed& operator &= (const sc_signed&   v); 
  sc_signed& operator &= (const sc_unsigned& v); 
  sc_signed& operator &= (int64              v); 
  sc_signed& operator &= (uint64             v); 
  sc_signed& operator &= (long               v); 
  sc_signed& operator &= (unsigned long      v); 
  sc_signed& operator &= (int                v) 
    { return operator&=((long) v); }
  sc_signed& operator &= (unsigned int       v) 
    { return operator&=((unsigned long) v); }

  // Bitwise OR operators:
   
  friend sc_signed operator | (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator | (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator | (const sc_unsigned& u, int64              v); 
  friend sc_signed operator | (const sc_unsigned& u, long               v); 
  friend sc_signed operator | (const sc_unsigned& u, int                v) 
    { return operator|(u, (long) v); }

  friend sc_signed operator | (int64              u, const sc_unsigned& v); 
  friend sc_signed operator | (long               u, const sc_unsigned& v); 
  friend sc_signed operator | (int                u, const sc_unsigned& v)  
    { return operator|((long) u, v); } 

  friend sc_signed operator | (const sc_signed&   u, const sc_signed&   v);
  friend sc_signed operator | (const sc_signed&   u, int64              v); 
  friend sc_signed operator | (const sc_signed&   u, uint64             v); 
  friend sc_signed operator | (const sc_signed&   u, long               v); 
  friend sc_signed operator | (const sc_signed&   u, unsigned long      v);
  friend sc_signed operator | (const sc_signed&   u, int                v) 
    { return operator|(u, (long) v); }
  friend sc_signed operator | (const sc_signed&   u, unsigned int       v) 
    { return operator|(u, (unsigned long) v); }

  friend sc_signed operator | (int64            u, const sc_signed& v); 
  friend sc_signed operator | (uint64           u, const sc_signed& v); 
  friend sc_signed operator | (long             u, const sc_signed& v); 
  friend sc_signed operator | (unsigned long    u, const sc_signed& v);
  friend sc_signed operator | (int              u, const sc_signed& v)  
    { return operator|((long) u, v); } 
  friend sc_signed operator | (unsigned int     u, const sc_signed& v)  
    { return operator|((unsigned long) u, v); } 

  sc_signed& operator |= (const sc_signed&   v); 
  sc_signed& operator |= (const sc_unsigned& v); 
  sc_signed& operator |= (int64              v); 
  sc_signed& operator |= (uint64             v); 
  sc_signed& operator |= (long               v); 
  sc_signed& operator |= (unsigned long      v); 
  sc_signed& operator |= (int                v) 
    { return operator|=((long) v); }
  sc_signed& operator |= (unsigned int       v) 
    { return operator|=((unsigned long) v); }

  // Bitwise XOR operators:
   
  friend sc_signed operator ^ (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_signed operator ^ (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator ^ (const sc_unsigned& u, int64              v); 
  friend sc_signed operator ^ (const sc_unsigned& u, long               v); 
  friend sc_signed operator ^ (const sc_unsigned& u, int                v) 
    { return operator^(u, (long) v); }

  friend sc_signed operator ^ (int64              u, const sc_unsigned& v); 
  friend sc_signed operator ^ (long               u, const sc_unsigned& v); 
  friend sc_signed operator ^ (int                u, const sc_unsigned& v)  
    { return operator^((long) u, v); } 

  friend sc_signed operator ^ (const sc_signed&   u, const sc_signed&   v);
  friend sc_signed operator ^ (const sc_signed&   u, int64              v); 
  friend sc_signed operator ^ (const sc_signed&   u, uint64             v); 
  friend sc_signed operator ^ (const sc_signed&   u, long               v); 
  friend sc_signed operator ^ (const sc_signed&   u, unsigned long      v);
  friend sc_signed operator ^ (const sc_signed&   u, int                v) 
    { return operator^(u, (long) v); }
  friend sc_signed operator ^ (const sc_signed&   u, unsigned int       v) 
    { return operator^(u, (unsigned long) v); }

  friend sc_signed operator ^ (int64            u, const sc_signed& v); 
  friend sc_signed operator ^ (uint64           u, const sc_signed& v); 
  friend sc_signed operator ^ (long             u, const sc_signed& v); 
  friend sc_signed operator ^ (unsigned long    u, const sc_signed& v);
  friend sc_signed operator ^ (int              u, const sc_signed& v)  
    { return operator^((long) u, v); } 
  friend sc_signed operator ^ (unsigned int     u, const sc_signed& v)  
    { return operator^((unsigned long) u, v); } 

  sc_signed& operator ^= (const sc_signed&   v); 
  sc_signed& operator ^= (const sc_unsigned& v); 
  sc_signed& operator ^= (int64              v); 
  sc_signed& operator ^= (uint64             v); 
  sc_signed& operator ^= (long               v); 
  sc_signed& operator ^= (unsigned long      v); 
  sc_signed& operator ^= (int                v) 
    { return operator^=((long) v); }
  sc_signed& operator ^= (unsigned int       v) 
    { return operator^=((unsigned long) v); }

  // SHIFT OPERATORS:

  // LEFT SHIFT operators:
   
  friend sc_unsigned operator << (const sc_unsigned& u, const sc_signed&   v); 
  friend   sc_signed operator << (const sc_signed&   u, const sc_unsigned& v); 

  friend   sc_signed operator << (const sc_signed&   u, const sc_signed&   v);
  friend   sc_signed operator << (const sc_signed&   u, int64              v); 
  friend   sc_signed operator << (const sc_signed&   u, uint64             v); 
  friend   sc_signed operator << (const sc_signed&   u, long               v); 
  friend   sc_signed operator << (const sc_signed&   u, unsigned long      v);
  friend   sc_signed operator << (const sc_signed&   u, int                v) 
    { return operator<<(u, (long) v); }
  friend   sc_signed operator << (const sc_signed&   u, unsigned int       v) 
    { return operator<<(u, (unsigned long) v); }

  sc_signed& operator <<= (const sc_signed&   v); 
  sc_signed& operator <<= (const sc_unsigned& v); 
  sc_signed& operator <<= (int64              v); 
  sc_signed& operator <<= (uint64             v); 
  sc_signed& operator <<= (long               v); 
  sc_signed& operator <<= (unsigned long      v); 
  sc_signed& operator <<= (int                v) 
    { return operator<<=((long) v); }
  sc_signed& operator <<= (unsigned int       v) 
    { return operator<<=((unsigned long) v); }

  // RIGHT SHIFT operators:
   
  friend sc_unsigned operator >> (const sc_unsigned& u, const sc_signed&   v); 
  friend   sc_signed operator >> (const sc_signed&   u, const sc_unsigned& v); 

  friend   sc_signed operator >> (const sc_signed&   u, const sc_signed&   v);
  friend   sc_signed operator >> (const sc_signed&   u, int64              v); 
  friend   sc_signed operator >> (const sc_signed&   u, uint64             v); 
  friend   sc_signed operator >> (const sc_signed&   u, long               v); 
  friend   sc_signed operator >> (const sc_signed&   u, unsigned long      v);
  friend   sc_signed operator >> (const sc_signed&   u, int                v) 
    { return operator>>(u, (long) v); }
  friend   sc_signed operator >> (const sc_signed&   u, unsigned int       v) 
    { return operator>>(u, (unsigned long) v); }

  sc_signed& operator >>= (const sc_signed&   v); 
  sc_signed& operator >>= (const sc_unsigned& v); 
  sc_signed& operator >>= (int64              v); 
  sc_signed& operator >>= (uint64             v); 
  sc_signed& operator >>= (long               v); 
  sc_signed& operator >>= (unsigned long      v); 
  sc_signed& operator >>= (int                v) 
    { return operator>>=((long) v); }
  sc_signed& operator >>= (unsigned int       v) 
    { return operator>>=((unsigned long) v); }

  // Unary arithmetic operators
  friend sc_signed operator + (const sc_signed&   u);
  friend sc_signed operator - (const sc_signed&   u); 
  friend sc_signed operator - (const sc_unsigned& u);

#else  // OLD SEMANTICS

  // ARITHMETIC OPERATORS:

  // ADDition operators:

  friend sc_unsigned operator + (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator + (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator + (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator + (const sc_signed& u, int64            v); 
  friend sc_signed operator + (const sc_signed& u, uint64           v); 
  friend sc_signed operator + (const sc_signed& u, long             v); 
  friend sc_signed operator + (const sc_signed& u, unsigned long    v);
  friend sc_signed operator + (const sc_signed& u, int              v) 
    { return operator+(u, (long) v); }
  friend sc_signed operator + (const sc_signed& u, unsigned int     v) 
    { return operator+(u, (unsigned long) v); }

  friend sc_signed operator + (int64            u, const sc_signed& v); 
  friend sc_signed operator + (uint64           u, const sc_signed& v); 
  friend sc_signed operator + (long             u, const sc_signed& v); 
  friend sc_signed operator + (unsigned long    u, const sc_signed& v);
  friend sc_signed operator + (int              u, const sc_signed& v)  
    { return operator+((long) u, v); } 
  friend sc_signed operator + (unsigned int     u, const sc_signed& v)  
    { return operator+((unsigned long) u, v); } 

  sc_signed& operator += (const sc_signed&   v); 
  sc_signed& operator += (const sc_unsigned& v); 
  sc_signed& operator += (int64              v); 
  sc_signed& operator += (uint64             v); 
  sc_signed& operator += (long               v); 
  sc_signed& operator += (unsigned long      v); 
  sc_signed& operator += (int                v) 
    { return operator+=((long) v); }
  sc_signed& operator += (unsigned int       v) 
    { return operator+=((unsigned long) v); }

  // SUBtraction operators:
   
  friend sc_unsigned operator - (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator - (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator - (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator - (const sc_signed& u, int64            v); 
  friend sc_signed operator - (const sc_signed& u, uint64           v); 
  friend sc_signed operator - (const sc_signed& u, long             v); 
  friend sc_signed operator - (const sc_signed& u, unsigned long    v);
  friend sc_signed operator - (const sc_signed& u, int              v) 
    { return operator-(u, (long) v); }
  friend sc_signed operator - (const sc_signed& u, unsigned int     v) 
    { return operator-(u, (unsigned long) v); }

  friend sc_signed operator - (int64            u, const sc_signed& v); 
  friend sc_signed operator - (uint64           u, const sc_signed& v); 
  friend sc_signed operator - (long             u, const sc_signed& v); 
  friend sc_signed operator - (unsigned long    u, const sc_signed& v);
  friend sc_signed operator - (int              u, const sc_signed& v)  
    { return operator-((long) u, v); } 
  friend sc_signed operator - (unsigned int     u, const sc_signed& v)  
    { return operator-((unsigned long) u, v); } 

  sc_signed& operator -= (const sc_signed&   v); 
  sc_signed& operator -= (const sc_unsigned& v); 
  sc_signed& operator -= (int64              v); 
  sc_signed& operator -= (uint64             v); 
  sc_signed& operator -= (long               v); 
  sc_signed& operator -= (unsigned long      v); 
  sc_signed& operator -= (int                v) 
    { return operator -= ((long) v); }
  sc_signed& operator -= (unsigned int       v) 
    { return operator -= ((unsigned long) v); }

  // MULtiplication operators:
   
  friend sc_unsigned operator * (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator * (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator * (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator * (const sc_signed& u, int64            v); 
  friend sc_signed operator * (const sc_signed& u, uint64           v); 
  friend sc_signed operator * (const sc_signed& u, long             v); 
  friend sc_signed operator * (const sc_signed& u, unsigned long    v);
  friend sc_signed operator * (const sc_signed& u, int              v) 
    { return operator*(u, (long) v); }
  friend sc_signed operator * (const sc_signed& u, unsigned int     v) 
    { return operator*(u, (unsigned long) v); }

  friend sc_signed operator * (int64            u, const sc_signed& v); 
  friend sc_signed operator * (uint64           u, const sc_signed& v); 
  friend sc_signed operator * (long             u, const sc_signed& v); 
  friend sc_signed operator * (unsigned long    u, const sc_signed& v);
  friend sc_signed operator * (int              u, const sc_signed& v)  
    { return operator*((long) u, v); } 
  friend sc_signed operator * (unsigned int     u, const sc_signed& v)  
    { return operator*((unsigned long) u, v); } 

  sc_signed& operator *= (const sc_signed&   v); 
  sc_signed& operator *= (const sc_unsigned& v); 
  sc_signed& operator *= (int64              v); 
  sc_signed& operator *= (uint64             v); 
  sc_signed& operator *= (long               v); 
  sc_signed& operator *= (unsigned long      v); 
  sc_signed& operator *= (int                v) 
    { return operator*=((long) v); }
  sc_signed& operator *= (unsigned int       v) 
    { return operator*=((unsigned long) v); }

  // DIVision operators:
   
  friend sc_unsigned operator / (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator / (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator / (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator / (const sc_signed& u, int64            v); 
  friend sc_signed operator / (const sc_signed& u, uint64           v); 
  friend sc_signed operator / (const sc_signed& u, long             v); 
  friend sc_signed operator / (const sc_signed& u, unsigned long    v);
  friend sc_signed operator / (const sc_signed& u, int              v) 
    { return operator/(u, (long) v); }
  friend sc_signed operator / (const sc_signed& u, unsigned int     v) 
    { return operator/(u, (unsigned long) v); }

  friend sc_signed operator / (int64            u, const sc_signed& v); 
  friend sc_signed operator / (uint64           u, const sc_signed& v); 
  friend sc_signed operator / (long             u, const sc_signed& v); 
  friend sc_signed operator / (unsigned long    u, const sc_signed& v);
  friend sc_signed operator / (int              u, const sc_signed& v)  
    { return operator/((long) u, v); } 
  friend sc_signed operator / (unsigned int     u, const sc_signed& v)  
    { return operator/((unsigned long) u, v); } 

  sc_signed& operator /= (const sc_signed&   v); 
  sc_signed& operator /= (const sc_unsigned& v); 
  sc_signed& operator /= (int64              v); 
  sc_signed& operator /= (uint64             v); 
  sc_signed& operator /= (long               v); 
  sc_signed& operator /= (unsigned long      v); 
  sc_signed& operator /= (int                v) 
    { return operator/=((long) v); }
  sc_signed& operator /= (unsigned int       v) 
    { return operator/=((unsigned long) v); }

  // MODulo operators:
   
  friend sc_unsigned operator % (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator % (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator % (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator % (const sc_signed& u, int64            v); 
  friend sc_signed operator % (const sc_signed& u, uint64           v); 
  friend sc_signed operator % (const sc_signed& u, long             v); 
  friend sc_signed operator % (const sc_signed& u, unsigned long    v);
  friend sc_signed operator % (const sc_signed& u, int              v) 
    { return operator%(u, (long) v); }
  friend sc_signed operator % (const sc_signed& u, unsigned int     v) 
    { return operator%(u, (unsigned long) v); }

  friend sc_signed operator % (int64            u, const sc_signed& v); 
  friend sc_signed operator % (uint64           u, const sc_signed& v); 
  friend sc_signed operator % (long             u, const sc_signed& v); 
  friend sc_signed operator % (unsigned long    u, const sc_signed& v);
  friend sc_signed operator % (int              u, const sc_signed& v)  
    { return operator%((long) u, v); } 
  friend sc_signed operator % (unsigned int     u, const sc_signed& v)  
    { return operator%((unsigned long) u, v); } 

  sc_signed& operator %= (const sc_signed&   v); 
  sc_signed& operator %= (const sc_unsigned& v); 
  sc_signed& operator %= (int64              v); 
  sc_signed& operator %= (uint64             v); 
  sc_signed& operator %= (long               v); 
  sc_signed& operator %= (unsigned long      v); 
  sc_signed& operator %= (int                v) 
    { return operator%=((long) v); }
  sc_signed& operator %= (unsigned int       v) 
    { return operator%=((unsigned long) v); }

  // BITWISE OPERATORS:

  // Bitwise AND operators:
   
  friend sc_unsigned operator & (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator & (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator & (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator & (const sc_signed& u, int64            v); 
  friend sc_signed operator & (const sc_signed& u, uint64           v); 
  friend sc_signed operator & (const sc_signed& u, long             v); 
  friend sc_signed operator & (const sc_signed& u, unsigned long    v);
  friend sc_signed operator & (const sc_signed& u, int              v) 
    { return operator&(u, (long) v); }
  friend sc_signed operator & (const sc_signed& u, unsigned int     v) 
    { return operator&(u, (unsigned long) v); }

  friend sc_signed operator & (int64            u, const sc_signed& v); 
  friend sc_signed operator & (uint64           u, const sc_signed& v); 
  friend sc_signed operator & (long             u, const sc_signed& v); 
  friend sc_signed operator & (unsigned long    u, const sc_signed& v);
  friend sc_signed operator & (int              u, const sc_signed& v)  
    { return operator&((long) u, v); } 
  friend sc_signed operator & (unsigned int     u, const sc_signed& v)  
    { return operator&((unsigned long) u, v); } 

  sc_signed& operator &= (const sc_signed&   v); 
  sc_signed& operator &= (const sc_unsigned& v); 
  sc_signed& operator &= (int64              v); 
  sc_signed& operator &= (uint64             v); 
  sc_signed& operator &= (long               v); 
  sc_signed& operator &= (unsigned long      v); 
  sc_signed& operator &= (int                v) 
    { return operator&=((long) v); }
  sc_signed& operator &= (unsigned int       v) 
    { return operator&=((unsigned long) v); }

  // Bitwise OR operators:
   
  friend sc_unsigned operator | (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator | (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator | (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator | (const sc_signed& u, int64            v); 
  friend sc_signed operator | (const sc_signed& u, uint64           v); 
  friend sc_signed operator | (const sc_signed& u, long             v); 
  friend sc_signed operator | (const sc_signed& u, unsigned long    v);
  friend sc_signed operator | (const sc_signed& u, int              v) 
    { return operator|(u, (long) v); }
  friend sc_signed operator | (const sc_signed& u, unsigned int     v) 
    { return operator|(u, (unsigned long) v); }

  friend sc_signed operator | (int64            u, const sc_signed& v); 
  friend sc_signed operator | (uint64           u, const sc_signed& v); 
  friend sc_signed operator | (long             u, const sc_signed& v); 
  friend sc_signed operator | (unsigned long    u, const sc_signed& v);
  friend sc_signed operator | (int              u, const sc_signed& v)  
    { return operator|((long) u, v); } 
  friend sc_signed operator | (unsigned int     u, const sc_signed& v)  
    { return operator|((unsigned long) u, v); } 

  sc_signed& operator |= (const sc_signed&   v); 
  sc_signed& operator |= (const sc_unsigned& v); 
  sc_signed& operator |= (int64              v); 
  sc_signed& operator |= (uint64             v); 
  sc_signed& operator |= (long               v); 
  sc_signed& operator |= (unsigned long      v); 
  sc_signed& operator |= (int                v) 
    { return operator|=((long) v); }
  sc_signed& operator |= (unsigned int       v) 
    { return operator|=((unsigned long) v); }

  // Bitwise XOR operators:
   
  friend sc_unsigned operator ^ (const sc_unsigned& u, const sc_signed&   v); 
  friend sc_unsigned operator ^ (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator ^ (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator ^ (const sc_signed& u, int64            v); 
  friend sc_signed operator ^ (const sc_signed& u, uint64           v); 
  friend sc_signed operator ^ (const sc_signed& u, long             v); 
  friend sc_signed operator ^ (const sc_signed& u, unsigned long    v);
  friend sc_signed operator ^ (const sc_signed& u, int              v) 
    { return operator^(u, (long) v); }
  friend sc_signed operator ^ (const sc_signed& u, unsigned int     v) 
    { return operator^(u, (unsigned long) v); }

  friend sc_signed operator ^ (int64            u, const sc_signed& v); 
  friend sc_signed operator ^ (uint64           u, const sc_signed& v); 
  friend sc_signed operator ^ (long             u, const sc_signed& v); 
  friend sc_signed operator ^ (unsigned long    u, const sc_signed& v);
  friend sc_signed operator ^ (int              u, const sc_signed& v)  
    { return operator^((long) u, v); } 
  friend sc_signed operator ^ (unsigned int     u, const sc_signed& v)  
    { return operator^((unsigned long) u, v); } 

  sc_signed& operator ^= (const sc_signed&   v); 
  sc_signed& operator ^= (const sc_unsigned& v); 
  sc_signed& operator ^= (int64              v); 
  sc_signed& operator ^= (uint64             v); 
  sc_signed& operator ^= (long               v); 
  sc_signed& operator ^= (unsigned long      v); 
  sc_signed& operator ^= (int                v) 
    { return operator^=((long) v); }
  sc_signed& operator ^= (unsigned int       v) 
    { return operator^=((unsigned long) v); }

  // SHIFT OPERATORS:

  // LEFT SHIFT operators:
   
  friend sc_unsigned operator << (const sc_unsigned& u, const sc_signed&   v); 
  friend   sc_signed operator << (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator << (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator << (const sc_signed& u, int64            v); 
  friend sc_signed operator << (const sc_signed& u, uint64           v); 
  friend sc_signed operator << (const sc_signed& u, long             v); 
  friend sc_signed operator << (const sc_signed& u, unsigned long    v);
  friend sc_signed operator << (const sc_signed& u, int              v) 
    { return operator<<(u, (long) v); }
  friend sc_signed operator << (const sc_signed& u, unsigned int     v) 
    { return operator<<(u, (unsigned long) v); }

  sc_signed& operator <<= (const sc_signed&   v); 
  sc_signed& operator <<= (const sc_unsigned& v); 
  sc_signed& operator <<= (int64              v); 
  sc_signed& operator <<= (uint64             v); 
  sc_signed& operator <<= (long               v); 
  sc_signed& operator <<= (unsigned long      v); 
  sc_signed& operator <<= (int                v) 
    { return operator<<=((long) v); }
  sc_signed& operator <<= (unsigned int       v) 
    { return operator<<=((unsigned long) v); }

  // RIGHT SHIFT operators:
   
  friend sc_unsigned operator >> (const sc_unsigned& u, const sc_signed&   v); 
  friend   sc_signed operator >> (const sc_signed&   u, const sc_unsigned& v); 

  friend sc_signed operator >> (const sc_signed& u, const sc_signed& v);
  friend sc_signed operator >> (const sc_signed& u, int64            v); 
  friend sc_signed operator >> (const sc_signed& u, uint64           v); 
  friend sc_signed operator >> (const sc_signed& u, long             v); 
  friend sc_signed operator >> (const sc_signed& u, unsigned long    v);
  friend sc_signed operator >> (const sc_signed& u, int              v) 
    { return operator>>(u, (long) v); }
  friend sc_signed operator >> (const sc_signed& u, unsigned int     v) 
    { return operator>>(u, (unsigned long) v); }

  sc_signed& operator >>= (const sc_signed&   v); 
  sc_signed& operator >>= (const sc_unsigned& v); 
  sc_signed& operator >>= (int64              v); 
  sc_signed& operator >>= (uint64             v); 
  sc_signed& operator >>= (long               v); 
  sc_signed& operator >>= (unsigned long      v); 
  sc_signed& operator >>= (int                v) 
    { return operator>>=((long) v); }
  sc_signed& operator >>= (unsigned int       v) 
    { return operator>>=((unsigned long) v); }

  // Unary arithmetic operators
  friend sc_signed operator - (const sc_signed& u); 
  friend sc_signed operator + (const sc_signed& u);

#endif  // END OF OLD SEMANTICS

  // LOGICAL OPERATORS:

  // Logical EQUAL operators:
   
  friend bool operator == (const sc_unsigned& u, const sc_signed&   v); 
  friend bool operator == (const sc_signed&   u, const sc_unsigned& v); 

  friend bool operator == (const sc_signed& u, const sc_signed& v);
  friend bool operator == (const sc_signed& u, int64            v); 
  friend bool operator == (const sc_signed& u, uint64           v); 
  friend bool operator == (const sc_signed& u, long             v); 
  friend bool operator == (const sc_signed& u, unsigned long    v);
  friend bool operator == (const sc_signed& u, int              v) 
    { return operator==(u, (long) v); }
  friend bool operator == (const sc_signed& u, unsigned int     v) 
    { return operator==(u, (unsigned long) v); }

  friend bool operator == (int64            u, const sc_signed& v); 
  friend bool operator == (uint64           u, const sc_signed& v); 
  friend bool operator == (long             u, const sc_signed& v); 
  friend bool operator == (unsigned long    u, const sc_signed& v);
  friend bool operator == (int              u, const sc_signed& v)  
    { return operator==((long) u, v); } 
  friend bool operator == (unsigned int     u, const sc_signed& v)  
    { return operator==((unsigned long) u, v); } 

  // Logical NOT_EQUAL operators:
   
  friend bool operator != (const sc_unsigned& u, const sc_signed&   v); 
  friend bool operator != (const sc_signed&   u, const sc_unsigned& v); 

  friend bool operator != (const sc_signed& u, const sc_signed& v);
  friend bool operator != (const sc_signed& u, int64            v); 
  friend bool operator != (const sc_signed& u, uint64           v); 
  friend bool operator != (const sc_signed& u, long             v); 
  friend bool operator != (const sc_signed& u, unsigned long    v);
  friend bool operator != (const sc_signed& u, int              v) 
    { return operator!=(u, (long) v); }
  friend bool operator != (const sc_signed& u, unsigned int     v) 
    { return operator!=(u, (unsigned long) v); }

  friend bool operator != (int64            u, const sc_signed& v); 
  friend bool operator != (uint64           u, const sc_signed& v); 
  friend bool operator != (long             u, const sc_signed& v); 
  friend bool operator != (unsigned long    u, const sc_signed& v);
  friend bool operator != (int              u, const sc_signed& v)  
    { return operator!=((long) u, v); } 
  friend bool operator != (unsigned int     u, const sc_signed& v)  
    { return operator!=((unsigned long) u, v); } 

  // Logical LESS_THAN operators:
   
  friend bool operator < (const sc_unsigned& u, const sc_signed&   v); 
  friend bool operator < (const sc_signed&   u, const sc_unsigned& v); 

  friend bool operator < (const sc_signed& u, const sc_signed& v);
  friend bool operator < (const sc_signed& u, int64            v); 
  friend bool operator < (const sc_signed& u, uint64           v); 
  friend bool operator < (const sc_signed& u, long             v); 
  friend bool operator < (const sc_signed& u, unsigned long    v);
  friend bool operator < (const sc_signed& u, int              v) 
    { return operator<(u, (long) v); }
  friend bool operator < (const sc_signed& u, unsigned int     v) 
    { return operator<(u, (unsigned long) v); }

  friend bool operator < (int64            u, const sc_signed& v); 
  friend bool operator < (uint64           u, const sc_signed& v); 
  friend bool operator < (long             u, const sc_signed& v); 
  friend bool operator < (unsigned long    u, const sc_signed& v);
  friend bool operator < (int              u, const sc_signed& v)  
    { return operator<((long) u, v); } 
  friend bool operator < (unsigned int     u, const sc_signed& v)  
    { return operator<((unsigned long) u, v); } 

  // Logical LESS_THAN_AND_EQUAL operators:
   
  friend bool operator <= (const sc_unsigned& u, const sc_signed&   v); 
  friend bool operator <= (const sc_signed&   u, const sc_unsigned& v); 

  friend bool operator <= (const sc_signed& u, const sc_signed& v);
  friend bool operator <= (const sc_signed& u, int64            v); 
  friend bool operator <= (const sc_signed& u, uint64           v); 
  friend bool operator <= (const sc_signed& u, long             v); 
  friend bool operator <= (const sc_signed& u, unsigned long    v);
  friend bool operator <= (const sc_signed& u, int              v) 
    { return operator<=(u, (long) v); }
  friend bool operator <= (const sc_signed& u, unsigned int     v) 
    { return operator<=(u, (unsigned long) v); }

  friend bool operator <= (int64            u, const sc_signed& v); 
  friend bool operator <= (uint64           u, const sc_signed& v); 
  friend bool operator <= (long             u, const sc_signed& v); 
  friend bool operator <= (unsigned long    u, const sc_signed& v);
  friend bool operator <= (int              u, const sc_signed& v)  
    { return operator<=((long) u, v); } 
  friend bool operator <= (unsigned int     u, const sc_signed& v)  
    { return operator<=((unsigned long) u, v); } 

  // Logical GREATER_THAN operators:
   
  friend bool operator > (const sc_unsigned& u, const sc_signed&   v); 
  friend bool operator > (const sc_signed&   u, const sc_unsigned& v); 

  friend bool operator > (const sc_signed& u, const sc_signed& v);
  friend bool operator > (const sc_signed& u, int64            v); 
  friend bool operator > (const sc_signed& u, uint64           v); 
  friend bool operator > (const sc_signed& u, long             v); 
  friend bool operator > (const sc_signed& u, unsigned long    v);
  friend bool operator > (const sc_signed& u, int              v) 
    { return operator>(u, (long) v); }
  friend bool operator > (const sc_signed& u, unsigned int     v) 
    { return operator>(u, (unsigned long) v); }

  friend bool operator > (int64            u, const sc_signed& v); 
  friend bool operator > (uint64           u, const sc_signed& v); 
  friend bool operator > (long             u, const sc_signed& v); 
  friend bool operator > (unsigned long    u, const sc_signed& v);
  friend bool operator > (int              u, const sc_signed& v)  
    { return operator>((long) u, v); } 
  friend bool operator > (unsigned int     u, const sc_signed& v)  
    { return operator>((unsigned long) u, v); } 

  // Logical GREATER_THAN_AND_EQUAL operators:
   
  friend bool operator >= (const sc_unsigned& u, const sc_signed&   v); 
  friend bool operator >= (const sc_signed&   u, const sc_unsigned& v); 

  friend bool operator >= (const sc_signed& u, const sc_signed& v);
  friend bool operator >= (const sc_signed& u, int64            v); 
  friend bool operator >= (const sc_signed& u, uint64           v); 
  friend bool operator >= (const sc_signed& u, long             v); 
  friend bool operator >= (const sc_signed& u, unsigned long    v);
  friend bool operator >= (const sc_signed& u, int              v) 
    { return operator>=(u, (long) v); }
  friend bool operator >= (const sc_signed& u, unsigned int     v) 
    { return operator>=(u, (unsigned long) v); }

  friend bool operator >= (int64            u, const sc_signed& v); 
  friend bool operator >= (uint64           u, const sc_signed& v); 
  friend bool operator >= (long             u, const sc_signed& v); 
  friend bool operator >= (unsigned long    u, const sc_signed& v);
  friend bool operator >= (int              u, const sc_signed& v)  
    { return operator>=((long) u, v); } 
  friend bool operator >= (unsigned int     u, const sc_signed& v)  
    { return operator>=((unsigned long) u, v); } 

  // Bitwise NOT operator (unary).
  friend sc_signed operator ~ (const sc_signed& u); 

  // Helper functions.
  friend sc_signed add_signed_friend(small_type us, 
                                     length_type unb,
                                     length_type und, 
                                     const digit_type *ud, 
                                     small_type vs, 
                                     length_type vnb,
                                     length_type vnd,
                                     const digit_type *vd);

  friend sc_signed sub_signed_friend(small_type us, 
                                     length_type unb,
                                     length_type und, 
                                     const digit_type *ud, 
                                     small_type vs, 
                                     length_type vnb,
                                     length_type vnd, 
                                     const digit_type *vd);
  
  friend sc_signed mul_signed_friend(small_type s,
                                     length_type unb,
                                     length_type und, 
                                     const digit_type *ud, 
                                     length_type vnb,
                                     length_type vnd,
                                     const digit_type *vd);
  
  friend sc_signed div_signed_friend(small_type s,
                                     length_type unb,
                                     length_type und, 
                                     const digit_type *ud, 
                                     length_type vnb,
                                     length_type vnd,
                                     const digit_type *vd);
  
  friend sc_signed mod_signed_friend(small_type us,
                                     length_type unb,
                                     length_type und, 
                                     const digit_type *ud, 
                                     length_type vnb,
                                     length_type vnd,
                                     const digit_type *vd);
  
  friend sc_signed and_signed_friend(small_type us, 
                                     length_type unb, 
                                     length_type und, 
                                     const digit_type *ud, 
                                     small_type vs,
                                     length_type vnb, 
                                     length_type vnd,
                                     const digit_type *vd);
  
  friend sc_signed or_signed_friend(small_type us, 
                                    length_type unb, 
                                    length_type und, 
                                    const digit_type *ud, 
                                    small_type vs,
                                    length_type vnb, 
                                    length_type vnd,
                                    const digit_type *vd);
  
  friend sc_signed xor_signed_friend(small_type us, 
                                     length_type unb, 
                                     length_type und, 
                                     const digit_type *ud, 
                                     small_type vs,
                                     length_type vnb, 
                                     length_type vnd,
                                     const digit_type *vd);
  
private:
  
  small_type  sgn;         // Shortened as s.
  length_type nbits;       // Shortened as nb.
  length_type ndigits;     // Shortened as nd.
  
#ifdef MAX_NBITS
  digit_type digit[DIV_CEIL(MAX_NBITS)];   // Shortened as d.
#else
  digit_type *digit;                       // Shortened as d.
#endif

  // Private constructors: 

  // Create zero.
  sc_signed();

  // Create a copy of v with sign s.
  sc_signed(const sc_signed&   v, small_type s);
  sc_signed(const sc_unsigned& v, small_type s);

  // Create a signed number with the given attributes.
  sc_signed(small_type s, length_type nb, length_type nd, 
            digit_type *d, bool alloc = true);

 // Create a signed number using the bits u[l..r].
  sc_signed(const sc_signed* u, length_type l, length_type r);

  // Private member functions. The called functions are inline functions.

  small_type default_sign() const
    { return SC_NOSIGN; }

  length_type num_bits(length_type nb) const { return nb; }

  bool check_if_outside(length_type bit_num) const;

  void copy_digits(length_type nb, length_type nd, const digit_type *d)
    { copy_digits_signed(sgn, nbits, ndigits, digit, nb, nd, d); }

  void makezero()
    { sgn = make_zero(ndigits, digit); }

  // Conversion functions between 2's complement (2C) and
  // sign-magnitude (SM):
  void convert_2C_to_SM()
    { sgn = convert_signed_2C_to_SM(nbits, ndigits, digit); }

  void convert_SM_to_2C_to_SM()
    { sgn = convert_signed_SM_to_2C_to_SM(sgn, nbits, ndigits, digit); }

  void convert_SM_to_2C()
    { convert_signed_SM_to_2C(sgn, ndigits, digit); }

#if 0
  void convert_SM_to_2C_trimmed()
    { convert_signed_SM_to_2C_trimmed(sgn, nbits, ndigits, digit); }
#endif

  // Needed for types using sc_signed.
  typedef long          asgn_type1;
  typedef unsigned long asgn_type2;
  typedef int           asgn_type3;
  typedef unsigned int  asgn_type4;
  typedef const char*    asgn_type5;

};

#endif
