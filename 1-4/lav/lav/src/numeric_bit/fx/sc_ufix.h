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

    sc_ufix.h - 

    Original Author: Martin Janssen. Synopsys, Inc. (mjanssen@synopsys.com)

******************************************************************************/

/******************************************************************************

    MODIFICATION LOG - modifiers, enter your name, affliation and
    changes you are making here:

    Modifier Name & Affiliation:
    Description of Modification:
    

******************************************************************************/


#ifndef SC_UFIX_H
#define SC_UFIX_H


#ifndef   SC_FXNUM_H
#include "sc_fxnum.h"
#endif

#if defined(__BCPLUSPLUS__)
#pragma hdrstop
#endif

class sc_ufix_fast;


// ----------------------------------------------------------------------------
//  CLASS : sc_ufix
//
//  "Unconstrained" unsigned fixed-point class; arbitrary precision.
// ----------------------------------------------------------------------------

class sc_ufix : public sc_fxnum
{

public:

    // constructors

    sc_ufix( sc_fxnum_observer* = 0 );
    sc_ufix( int, int,
	     sc_fxnum_observer* = 0 );
    sc_ufix( sc_q_mode, sc_o_mode,
	     sc_fxnum_observer* = 0 );
    sc_ufix( sc_q_mode, sc_o_mode, int,
	     sc_fxnum_observer* = 0 );
    sc_ufix( int, int, sc_q_mode, sc_o_mode,
	     sc_fxnum_observer* = 0 );
    sc_ufix( int, int, sc_q_mode, sc_o_mode, int,
	     sc_fxnum_observer* = 0 );
    sc_ufix( const sc_fxcast_switch&,
	     sc_fxnum_observer* = 0 );
    sc_ufix( int, int,
	     const sc_fxcast_switch&,
	     sc_fxnum_observer* = 0 );
    sc_ufix( sc_q_mode, sc_o_mode,
	     const sc_fxcast_switch&,
	     sc_fxnum_observer* = 0 );
    sc_ufix( sc_q_mode, sc_o_mode, int,
	     const sc_fxcast_switch&,
	     sc_fxnum_observer* = 0 );
    sc_ufix( int, int, sc_q_mode, sc_o_mode,
	     const sc_fxcast_switch&,
	     sc_fxnum_observer* = 0 );
    sc_ufix( int, int, sc_q_mode, sc_o_mode, int,
	     const sc_fxcast_switch&,
	     sc_fxnum_observer* = 0 );
    sc_ufix( const sc_fxtype_params&,
	     sc_fxnum_observer* = 0 );
    sc_ufix( const sc_fxtype_params&,
	     const sc_fxcast_switch&,
	     sc_fxnum_observer* = 0 );

#define DECL_CTORS_T(tp)                                                      \
    sc_ufix( tp,                                                              \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     int, int,                                                        \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     sc_q_mode, sc_o_mode,                                            \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     sc_q_mode, sc_o_mode, int,                                       \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     int, int, sc_q_mode, sc_o_mode,                                  \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     int, int, sc_q_mode, sc_o_mode, int,                             \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     const sc_fxcast_switch&,                                         \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     int, int,                                                        \
	     const sc_fxcast_switch&,                                         \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     sc_q_mode, sc_o_mode,                                            \
	     const sc_fxcast_switch&,                                         \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     sc_q_mode, sc_o_mode, int,                                       \
	     const sc_fxcast_switch&,                                         \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     int, int, sc_q_mode, sc_o_mode,                                  \
	     const sc_fxcast_switch&,                                         \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     int, int, sc_q_mode, sc_o_mode, int,                             \
	     const sc_fxcast_switch&,                                         \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     const sc_fxtype_params&,                                         \
	     sc_fxnum_observer* = 0 );                                        \
    sc_ufix( tp,                                                              \
	     const sc_fxtype_params&,                                         \
	     const sc_fxcast_switch&,                                         \
	     sc_fxnum_observer* = 0 );

    DECL_CTORS_T(short)
    DECL_CTORS_T(unsigned short)
    DECL_CTORS_T(int)
    DECL_CTORS_T(unsigned int)
    DECL_CTORS_T(long)
    DECL_CTORS_T(unsigned long)
    DECL_CTORS_T(float)
    DECL_CTORS_T(double)
    DECL_CTORS_T(const char*)
    DECL_CTORS_T(int64)
    DECL_CTORS_T(uint64)
    DECL_CTORS_T(const sc_int_base&)
    DECL_CTORS_T(const sc_uint_base&)
    DECL_CTORS_T(const sc_signed&)
    DECL_CTORS_T(const sc_unsigned&)
    DECL_CTORS_T(const sc_fxval&)
    DECL_CTORS_T(const sc_fxval_fast&)
    DECL_CTORS_T(const sc_fxnum&)
    DECL_CTORS_T(const sc_fxnum_fast&)

#undef DECL_CTORS_T

    // copy constructor

    sc_ufix( const sc_ufix& );


    // unary bitwise operators

    const sc_ufix operator ~ () const;


    // unary bitwise functions

    friend void b_not( sc_ufix&, const sc_ufix& );


    // binary bitwise operators

    friend const sc_ufix operator & ( const sc_ufix&, const sc_ufix& );
    friend const sc_ufix operator & ( const sc_ufix&, const sc_ufix_fast& );
    friend const sc_ufix operator & ( const sc_ufix_fast&, const sc_ufix& );
    friend const sc_ufix operator | ( const sc_ufix&, const sc_ufix& );
    friend const sc_ufix operator | ( const sc_ufix&, const sc_ufix_fast& );
    friend const sc_ufix operator | ( const sc_ufix_fast&, const sc_ufix& );
    friend const sc_ufix operator ^ ( const sc_ufix&, const sc_ufix& );
    friend const sc_ufix operator ^ ( const sc_ufix&, const sc_ufix_fast& );
    friend const sc_ufix operator ^ ( const sc_ufix_fast&, const sc_ufix& );


    // binary bitwise functions

    friend void b_and( sc_ufix&, const sc_ufix&, const sc_ufix& );
    friend void b_and( sc_ufix&, const sc_ufix&, const sc_ufix_fast& );
    friend void b_and( sc_ufix&, const sc_ufix_fast&, const sc_ufix& );
    friend void b_or ( sc_ufix&, const sc_ufix&, const sc_ufix& );
    friend void b_or ( sc_ufix&, const sc_ufix&, const sc_ufix_fast& );
    friend void b_or ( sc_ufix&, const sc_ufix_fast&, const sc_ufix& );
    friend void b_xor( sc_ufix&, const sc_ufix&, const sc_ufix& );
    friend void b_xor( sc_ufix&, const sc_ufix&, const sc_ufix_fast& );
    friend void b_xor( sc_ufix&, const sc_ufix_fast&, const sc_ufix& );


    // assignment operators

#define DECL_ASN_OP_T(op,tp)                                                  \
    sc_ufix& operator op ( tp );

#define DECL_ASN_OP(op)                                                       \
    DECL_ASN_OP_T(op,short)                                                   \
    DECL_ASN_OP_T(op,unsigned short)                                          \
    DECL_ASN_OP_T(op,int)                                                     \
    DECL_ASN_OP_T(op,unsigned int)                                            \
    DECL_ASN_OP_T(op,long)                                                    \
    DECL_ASN_OP_T(op,unsigned long)                                           \
    DECL_ASN_OP_T(op,float)                                                   \
    DECL_ASN_OP_T(op,double)                                                  \
    DECL_ASN_OP_T(op,const char*)                                             \
    DECL_ASN_OP_T(op,int64)                                                   \
    DECL_ASN_OP_T(op,uint64)                                                  \
    DECL_ASN_OP_T(op,const sc_int_base&)                                      \
    DECL_ASN_OP_T(op,const sc_uint_base&)                                     \
    DECL_ASN_OP_T(op,const sc_signed&)                                        \
    DECL_ASN_OP_T(op,const sc_unsigned&)                                      \
    DECL_ASN_OP_T(op,const sc_fxval&)                                         \
    DECL_ASN_OP_T(op,const sc_fxval_fast&)                                    \
    DECL_ASN_OP_T(op,const sc_fxnum&)                                         \
    DECL_ASN_OP_T(op,const sc_fxnum_fast&)

    DECL_ASN_OP(=)

    DECL_ASN_OP(*=)
    DECL_ASN_OP(/=)
    DECL_ASN_OP(+=)
    DECL_ASN_OP(-=)

    DECL_ASN_OP_T(<<=,int)
    DECL_ASN_OP_T(>>=,int)

    DECL_ASN_OP_T(&=,const sc_ufix&)
    DECL_ASN_OP_T(&=,const sc_ufix_fast&)
    DECL_ASN_OP_T(|=,const sc_ufix&)
    DECL_ASN_OP_T(|=,const sc_ufix_fast&)
    DECL_ASN_OP_T(^=,const sc_ufix&)
    DECL_ASN_OP_T(^=,const sc_ufix_fast&)

#undef DECL_ASN_OP_T
#undef DECL_ASN_OP


    // auto-increment and auto-decrement

    const sc_fxval operator ++ ( int );
    const sc_fxval operator -- ( int );

    sc_ufix& operator ++ ();
    sc_ufix& operator -- ();

};


// ----------------------------------------------------------------------------
//  CLASS : sc_ufix_fast
//
//  "Unconstrained" unsigned fixed-point class; limited precision.
// ----------------------------------------------------------------------------

class sc_ufix_fast : public sc_fxnum_fast
{

public:

    // constructors

    sc_ufix_fast( sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( int, int,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( sc_q_mode, sc_o_mode,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( sc_q_mode, sc_o_mode, int,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( int, int, sc_q_mode, sc_o_mode,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( int, int, sc_q_mode, sc_o_mode, int,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( const sc_fxcast_switch&,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( int, int,
		  const sc_fxcast_switch&,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( sc_q_mode, sc_o_mode,
		  const sc_fxcast_switch&,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( sc_q_mode, sc_o_mode, int,
		  const sc_fxcast_switch&,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( int, int, sc_q_mode, sc_o_mode,
		  const sc_fxcast_switch&,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( int, int, sc_q_mode, sc_o_mode, int,
		  const sc_fxcast_switch&,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( const sc_fxtype_params&,
		  sc_fxnum_fast_observer* = 0 );
    sc_ufix_fast( const sc_fxtype_params&,
		  const sc_fxcast_switch&,
		  sc_fxnum_fast_observer* = 0 );

#define DECL_CTORS_T(tp)                                                      \
    sc_ufix_fast( tp,                                                         \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  int, int,                                                   \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  sc_q_mode, sc_o_mode,                                       \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  sc_q_mode, sc_o_mode, int,                                  \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  int, int, sc_q_mode, sc_o_mode,                             \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  int, int, sc_q_mode, sc_o_mode, int,                        \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  const sc_fxcast_switch&,                                    \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  int, int,                                                   \
		  const sc_fxcast_switch&,                                    \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  sc_q_mode, sc_o_mode,                                       \
		  const sc_fxcast_switch&,                                    \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  sc_q_mode, sc_o_mode, int,                                  \
		  const sc_fxcast_switch&,                                    \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  int, int, sc_q_mode, sc_o_mode,                             \
		  const sc_fxcast_switch&,                                    \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  int, int, sc_q_mode, sc_o_mode, int,                        \
		  const sc_fxcast_switch&,                                    \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  const sc_fxtype_params&,                                    \
		  sc_fxnum_fast_observer* = 0 );                              \
    sc_ufix_fast( tp,                                                         \
		  const sc_fxtype_params&,                                    \
		  const sc_fxcast_switch&,                                    \
		  sc_fxnum_fast_observer* = 0 );

    DECL_CTORS_T(short)
    DECL_CTORS_T(unsigned short)
    DECL_CTORS_T(int)
    DECL_CTORS_T(unsigned int)
    DECL_CTORS_T(long)
    DECL_CTORS_T(unsigned long)
    DECL_CTORS_T(float)
    DECL_CTORS_T(double)
    DECL_CTORS_T(const char*)
    DECL_CTORS_T(int64)
    DECL_CTORS_T(uint64)
    DECL_CTORS_T(const sc_int_base&)
    DECL_CTORS_T(const sc_uint_base&)
    DECL_CTORS_T(const sc_signed&)
    DECL_CTORS_T(const sc_unsigned&)
    DECL_CTORS_T(const sc_fxval&)
    DECL_CTORS_T(const sc_fxval_fast&)
    DECL_CTORS_T(const sc_fxnum&)
    DECL_CTORS_T(const sc_fxnum_fast&)

#undef DECL_CTORS_T

    // copy constructor

    sc_ufix_fast( const sc_ufix_fast& );


    // unary bitwise operators

    const sc_ufix_fast operator ~ () const;


    // unary bitwise functions

    friend void b_not( sc_ufix_fast&, const sc_ufix_fast& );


    // binary bitwise operators

    friend const sc_ufix_fast operator & ( const sc_ufix_fast&,
					   const sc_ufix_fast& );
    friend const sc_ufix_fast operator ^ ( const sc_ufix_fast&,
					   const sc_ufix_fast& );
    friend const sc_ufix_fast operator | ( const sc_ufix_fast&,
					   const sc_ufix_fast& );


    // binary bitwise functions

    friend void b_and( sc_ufix_fast&, const sc_ufix_fast&,
		                      const sc_ufix_fast& );
    friend void b_or ( sc_ufix_fast&, const sc_ufix_fast&,
		                      const sc_ufix_fast& );
    friend void b_xor( sc_ufix_fast&, const sc_ufix_fast&,
		                      const sc_ufix_fast& );


    // assignment operators

#define DECL_ASN_OP_T(op,tp)                                                  \
    sc_ufix_fast& operator op ( tp );

#define DECL_ASN_OP(op)                                                       \
    DECL_ASN_OP_T(op,short)                                                   \
    DECL_ASN_OP_T(op,unsigned short)                                          \
    DECL_ASN_OP_T(op,int)                                                     \
    DECL_ASN_OP_T(op,unsigned int)                                            \
    DECL_ASN_OP_T(op,long)                                                    \
    DECL_ASN_OP_T(op,unsigned long)                                           \
    DECL_ASN_OP_T(op,float)                                                   \
    DECL_ASN_OP_T(op,double)                                                  \
    DECL_ASN_OP_T(op,const char*)                                             \
    DECL_ASN_OP_T(op,int64)                                                   \
    DECL_ASN_OP_T(op,uint64)                                                  \
    DECL_ASN_OP_T(op,const sc_int_base&)                                      \
    DECL_ASN_OP_T(op,const sc_uint_base&)                                     \
    DECL_ASN_OP_T(op,const sc_signed&)                                        \
    DECL_ASN_OP_T(op,const sc_unsigned&)                                      \
    DECL_ASN_OP_T(op,const sc_fxval&)                                         \
    DECL_ASN_OP_T(op,const sc_fxval_fast&)                                    \
    DECL_ASN_OP_T(op,const sc_fxnum&)                                         \
    DECL_ASN_OP_T(op,const sc_fxnum_fast&)

    DECL_ASN_OP(=)

    DECL_ASN_OP(*=)
    DECL_ASN_OP(/=)
    DECL_ASN_OP(+=)
    DECL_ASN_OP(-=)

    DECL_ASN_OP_T(<<=,int)
    DECL_ASN_OP_T(>>=,int)

    DECL_ASN_OP_T(&=,const sc_ufix&)
    DECL_ASN_OP_T(&=,const sc_ufix_fast&)
    DECL_ASN_OP_T(|=,const sc_ufix&)
    DECL_ASN_OP_T(|=,const sc_ufix_fast&)
    DECL_ASN_OP_T(^=,const sc_ufix&)
    DECL_ASN_OP_T(^=,const sc_ufix_fast&)

#undef DECL_ASN_OP_T
#undef DECL_ASN_OP


    // auto-increment and auto-decrement

    const sc_fxval_fast operator ++ ( int );
    const sc_fxval_fast operator -- ( int );

    sc_ufix_fast& operator ++ ();
    sc_ufix_fast& operator -- ();

};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

// ----------------------------------------------------------------------------
//  CLASS : sc_ufix
//
//  "Unconstrained" unsigned fixed-point class; arbitrary precision.
// ----------------------------------------------------------------------------

// constructors

inline
sc_ufix::sc_ufix( sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params(),
	    SC_US,
	    sc_fxcast_switch(),
	    observer_ )
{}

inline
sc_ufix::sc_ufix( int wl_, int iwl_,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( wl_, iwl_ ),
	    SC_US,
	    sc_fxcast_switch(),
	    observer_ )
{}

inline
sc_ufix::sc_ufix( sc_q_mode qm, sc_o_mode om,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( qm, om ),
	    SC_US,
	    sc_fxcast_switch(),
	    observer_ )
{}

inline
sc_ufix::sc_ufix( sc_q_mode qm, sc_o_mode om, int nb,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( qm, om, nb ),
	    SC_US,
	    sc_fxcast_switch(),
	    observer_ )
{}

inline
sc_ufix::sc_ufix( int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( wl_, iwl_, qm, om ),
	    SC_US,
	    sc_fxcast_switch(),
	    observer_ )
{}

inline
sc_ufix::sc_ufix( int wl_, int iwl_, sc_q_mode qm, sc_o_mode om, int nb,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( wl_, iwl_, qm, om, nb ),
	    SC_US,
	    sc_fxcast_switch(),
	    observer_ )
{}

inline
sc_ufix::sc_ufix( const sc_fxcast_switch& cast_sw,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params(),
	    SC_US,
	    cast_sw,
	    observer_ )
{}

inline
sc_ufix::sc_ufix( int wl_, int iwl_,
		  const sc_fxcast_switch& cast_sw,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( wl_, iwl_ ),
	    SC_US,
	    cast_sw,
	    observer_ )
{}

inline
sc_ufix::sc_ufix( sc_q_mode qm, sc_o_mode om,
		  const sc_fxcast_switch& cast_sw,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( qm, om ),
	    SC_US,
	    cast_sw,
	    observer_ )
{}

inline
sc_ufix::sc_ufix( sc_q_mode qm, sc_o_mode om, int nb,
		  const sc_fxcast_switch& cast_sw,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( qm, om, nb ),
	    SC_US,
	    cast_sw,
	    observer_ )
{}

inline
sc_ufix::sc_ufix( int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,
		  const sc_fxcast_switch& cast_sw,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( wl_, iwl_, qm, om ),
	    SC_US,
	    cast_sw,
	    observer_ )
{}

inline
sc_ufix::sc_ufix( int wl_, int iwl_, sc_q_mode qm, sc_o_mode om, int nb,
		  const sc_fxcast_switch& cast_sw,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( sc_fxtype_params( wl_, iwl_, qm, om, nb ),
	    SC_US,
	    cast_sw,
	    observer_ )
{}

inline
sc_ufix::sc_ufix( const sc_fxtype_params& type_params,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( type_params,
	    SC_US,
	    sc_fxcast_switch(),
	    observer_ )
{}

inline
sc_ufix::sc_ufix( const sc_fxtype_params& type_params,
		  const sc_fxcast_switch& cast_sw,
		  sc_fxnum_observer* observer_ )
: sc_fxnum( type_params,
	    SC_US,
	    cast_sw,
	    observer_ )
{}

#define DEFN_CTORS_T(tp)                                                      \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params(),                                               \
	    SC_US,                                                            \
	    sc_fxcast_switch(),                                               \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  int wl_, int iwl_,                                          \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( wl_, iwl_ ),                                    \
	    SC_US,                                                            \
	    sc_fxcast_switch(),                                               \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  sc_q_mode qm, sc_o_mode om,                                 \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( qm, om ),                                       \
	    SC_US,                                                            \
	    sc_fxcast_switch(),                                               \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  sc_q_mode qm, sc_o_mode om, int nb,                         \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( qm, om, nb ),                                   \
	    SC_US,                                                            \
	    sc_fxcast_switch(),                                               \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,              \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( wl_, iwl_, qm, om ),                            \
	    SC_US,                                                            \
	    sc_fxcast_switch(),                                               \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  int wl_, int iwl_, sc_q_mode qm, sc_o_mode om, int nb,      \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( wl_, iwl_, qm, om, nb ),                        \
	    SC_US,                                                            \
	    sc_fxcast_switch(),                                               \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  const sc_fxcast_switch& cast_sw,                            \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params(),                                               \
	    SC_US,                                                            \
	    cast_sw,                                                          \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  int wl_, int iwl_,                                          \
		  const sc_fxcast_switch& cast_sw,                            \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( wl_, iwl_ ),                                    \
	    SC_US,                                                            \
	    cast_sw,                                                          \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  sc_q_mode qm, sc_o_mode om,                                 \
		  const sc_fxcast_switch& cast_sw,                            \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( qm, om ),                                       \
	    SC_US,                                                            \
	    cast_sw,                                                          \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  sc_q_mode qm, sc_o_mode om, int nb,                         \
		  const sc_fxcast_switch& cast_sw,                            \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( qm, om, nb ),                                   \
	    SC_US,                                                            \
	    cast_sw,                                                          \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,              \
		  const sc_fxcast_switch& cast_sw,                            \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( wl_, iwl_, qm, om ),                            \
	    SC_US,                                                            \
	    cast_sw,                                                          \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  int wl_, int iwl_, sc_q_mode qm, sc_o_mode om, int nb,      \
		  const sc_fxcast_switch& cast_sw,                            \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    sc_fxtype_params( wl_, iwl_, qm, om, nb ),                        \
	    SC_US,                                                            \
	    cast_sw,                                                          \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  const sc_fxtype_params& type_params,                        \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    type_params,                                                      \
	    SC_US,                                                            \
	    sc_fxcast_switch(),                                               \
	    observer_ )                                                       \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix::sc_ufix( tp a,                                                       \
		  const sc_fxtype_params& type_params,                        \
		  const sc_fxcast_switch& cast_sw,                            \
		  sc_fxnum_observer* observer_ )                              \
: sc_fxnum( a,                                                                \
	    type_params,                                                      \
	    SC_US,                                                            \
	    cast_sw,                                                          \
	    observer_ )                                                       \
{}

DEFN_CTORS_T(short)
DEFN_CTORS_T(unsigned short)
DEFN_CTORS_T(int)
DEFN_CTORS_T(unsigned int)
DEFN_CTORS_T(long)
DEFN_CTORS_T(unsigned long)
DEFN_CTORS_T(float)
DEFN_CTORS_T(double)
DEFN_CTORS_T(const char*)
DEFN_CTORS_T(int64)
DEFN_CTORS_T(uint64)
DEFN_CTORS_T(const sc_int_base&)
DEFN_CTORS_T(const sc_uint_base&)
DEFN_CTORS_T(const sc_signed&)
DEFN_CTORS_T(const sc_unsigned&)
DEFN_CTORS_T(const sc_fxval&)
DEFN_CTORS_T(const sc_fxval_fast&)
DEFN_CTORS_T(const sc_fxnum&)
DEFN_CTORS_T(const sc_fxnum_fast&)

#undef DEFN_CTORS_T

// copy constructor

inline
sc_ufix::sc_ufix( const sc_ufix& a )
: sc_fxnum( a,
	    sc_fxtype_params(),
	    SC_US,
	    sc_fxcast_switch(),
	    0 )
{}


// assignment operators

#define DEFN_ASN_OP_T(op,tp)                                                  \
inline                                                                        \
sc_ufix&                                                                      \
sc_ufix::operator op ( tp a )                                                 \
{                                                                             \
    sc_fxnum::operator op( a );                                               \
    return *this;                                                             \
}

#define DEFN_ASN_OP(op)                                                       \
DEFN_ASN_OP_T(op,short)                                                       \
DEFN_ASN_OP_T(op,unsigned short)                                              \
DEFN_ASN_OP_T(op,int)                                                         \
DEFN_ASN_OP_T(op,unsigned int)                                                \
DEFN_ASN_OP_T(op,long)                                                        \
DEFN_ASN_OP_T(op,unsigned long)                                               \
DEFN_ASN_OP_T(op,float)                                                       \
DEFN_ASN_OP_T(op,double)                                                      \
DEFN_ASN_OP_T(op,const char*)                                                 \
DEFN_ASN_OP_T(op,int64)                                                       \
DEFN_ASN_OP_T(op,uint64)                                                      \
DEFN_ASN_OP_T(op,const sc_int_base&)                                          \
DEFN_ASN_OP_T(op,const sc_uint_base&)                                         \
DEFN_ASN_OP_T(op,const sc_signed&)                                            \
DEFN_ASN_OP_T(op,const sc_unsigned&)                                          \
DEFN_ASN_OP_T(op,const sc_fxval&)                                             \
DEFN_ASN_OP_T(op,const sc_fxval_fast&)                                        \
DEFN_ASN_OP_T(op,const sc_fxnum&)                                             \
DEFN_ASN_OP_T(op,const sc_fxnum_fast&)

DEFN_ASN_OP(=)

DEFN_ASN_OP(*=)
DEFN_ASN_OP(/=)
DEFN_ASN_OP(+=)
DEFN_ASN_OP(-=)

DEFN_ASN_OP_T(<<=,int)
DEFN_ASN_OP_T(>>=,int)

#undef DEFN_ASN_OP_T
#undef DEFN_ASN_OP


// auto-increment and auto-decrement

inline
const sc_fxval
sc_ufix::operator ++ ( int )
{
    return sc_fxval( sc_fxnum::operator ++ ( 0 ) );
}

inline
const sc_fxval
sc_ufix::operator -- ( int )
{
    return sc_fxval( sc_fxnum::operator -- ( 0 ) );
}

inline
sc_ufix&
sc_ufix::operator ++ ()
{
    sc_fxnum::operator ++ ();
    return *this;
}

inline
sc_ufix&
sc_ufix::operator -- ()
{
    sc_fxnum::operator -- ();
    return *this;
}


// ----------------------------------------------------------------------------
//  CLASS : sc_ufix_fast
//
//  "Unconstrained" unsigned fixed-point class; limited precision.
// ----------------------------------------------------------------------------

// constructors

inline
sc_ufix_fast::sc_ufix_fast( sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params(),
		 SC_US,
		 sc_fxcast_switch(),
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( int wl_, int iwl_,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( wl_, iwl_ ),
		 SC_US,
		 sc_fxcast_switch(),
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( sc_q_mode qm, sc_o_mode om,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( qm, om ),
		 SC_US,
		 sc_fxcast_switch(),
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( sc_q_mode qm, sc_o_mode om, int nb,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( qm, om, nb ),
		 SC_US,
		 sc_fxcast_switch(),
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( wl_, iwl_, qm, om ),
		 SC_US,
		 sc_fxcast_switch(),
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( int wl_, int iwl_,
			    sc_q_mode qm, sc_o_mode om, int nb,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( wl_, iwl_, qm, om, nb ),
		 SC_US,
		 sc_fxcast_switch(),
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( const sc_fxcast_switch& cast_sw,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params(),
		 SC_US,
		 cast_sw,
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( int wl_, int iwl_,
			    const sc_fxcast_switch& cast_sw,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( wl_, iwl_ ),
		 SC_US,
		 cast_sw,
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( sc_q_mode qm, sc_o_mode om,
			    const sc_fxcast_switch& cast_sw,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( qm, om ),
		 SC_US,
		 cast_sw,
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( sc_q_mode qm, sc_o_mode om, int nb,
			    const sc_fxcast_switch& cast_sw,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( qm, om, nb ),
		 SC_US,
		 cast_sw,
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,
			    const sc_fxcast_switch& cast_sw,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( wl_, iwl_, qm, om ),
		 SC_US,
		 cast_sw,
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( int wl_, int iwl_,
			    sc_q_mode qm, sc_o_mode om, int nb,
			    const sc_fxcast_switch& cast_sw,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( sc_fxtype_params( wl_, iwl_, qm, om, nb ),
		 SC_US,
		 cast_sw,
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( const sc_fxtype_params& type_params,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( type_params,
		 SC_US,
		 sc_fxcast_switch(),
		 observer_ )
{}

inline
sc_ufix_fast::sc_ufix_fast( const sc_fxtype_params& type_params,
			    const sc_fxcast_switch& cast_sw,
			    sc_fxnum_fast_observer* observer_ )
: sc_fxnum_fast( type_params,
		 SC_US,
		 cast_sw,
		 observer_ )
{}

#define DEFN_CTORS_T(tp)                                                      \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params(),                                          \
		 SC_US,                                                       \
		 sc_fxcast_switch(),                                          \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    int wl_, int iwl_,                                \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( wl_, iwl_ ),                               \
		 SC_US,                                                       \
		 sc_fxcast_switch(),                                          \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    sc_q_mode qm, sc_o_mode om,                       \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( qm, om ),                                  \
		 SC_US,                                                       \
		 sc_fxcast_switch(),                                          \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    sc_q_mode qm, sc_o_mode om, int nb,               \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( qm, om, nb ),                              \
		 SC_US,                                                       \
		 sc_fxcast_switch(),                                          \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,    \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( wl_, iwl_, qm, om ),                       \
		 SC_US,                                                       \
		 sc_fxcast_switch(),                                          \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    int wl_, int iwl_,                                \
			    sc_q_mode qm, sc_o_mode om, int nb,               \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( wl_, iwl_, qm, om, nb ),                   \
		 SC_US,                                                       \
		 sc_fxcast_switch(),                                          \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    const sc_fxcast_switch& cast_sw,                  \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params(),                                          \
		 SC_US,                                                       \
		 cast_sw,                                                     \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    int wl_, int iwl_,                                \
			    const sc_fxcast_switch& cast_sw,                  \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( wl_, iwl_ ),                               \
		 SC_US,                                                       \
		 cast_sw,                                                     \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    sc_q_mode qm, sc_o_mode om,                       \
			    const sc_fxcast_switch& cast_sw,                  \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( qm, om ),                                  \
		 SC_US,                                                       \
		 cast_sw,                                                     \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    sc_q_mode qm, sc_o_mode om, int nb,               \
			    const sc_fxcast_switch& cast_sw,                  \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( qm, om, nb ),                              \
		 SC_US,                                                       \
		 cast_sw,                                                     \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    int wl_, int iwl_, sc_q_mode qm, sc_o_mode om,    \
			    const sc_fxcast_switch& cast_sw,                  \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( wl_, iwl_, qm, om ),                       \
		 SC_US,                                                       \
		 cast_sw,                                                     \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    int wl_, int iwl_,                                \
			    sc_q_mode qm, sc_o_mode om, int nb,               \
			    const sc_fxcast_switch& cast_sw,                  \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 sc_fxtype_params( wl_, iwl_, qm, om, nb ),                   \
		 SC_US,                                                       \
		 cast_sw,                                                     \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    const sc_fxtype_params& type_params,              \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 type_params,                                                 \
		 SC_US,                                                       \
		 sc_fxcast_switch(),                                          \
		 observer_ )                                                  \
{}                                                                            \
                                                                              \
inline                                                                        \
sc_ufix_fast::sc_ufix_fast( tp a,                                             \
			    const sc_fxtype_params& type_params,              \
			    const sc_fxcast_switch& cast_sw,                  \
			    sc_fxnum_fast_observer* observer_ )               \
: sc_fxnum_fast( a,                                                           \
		 type_params,                                                 \
		 SC_US,                                                       \
		 cast_sw,                                                     \
		 observer_ )                                                  \
{}

DEFN_CTORS_T(short)
DEFN_CTORS_T(unsigned short)
DEFN_CTORS_T(int)
DEFN_CTORS_T(unsigned int)
DEFN_CTORS_T(long)
DEFN_CTORS_T(unsigned long)
DEFN_CTORS_T(float)
DEFN_CTORS_T(double)
DEFN_CTORS_T(const char*)
DEFN_CTORS_T(int64)
DEFN_CTORS_T(uint64)
DEFN_CTORS_T(const sc_int_base&)
DEFN_CTORS_T(const sc_uint_base&)
DEFN_CTORS_T(const sc_signed&)
DEFN_CTORS_T(const sc_unsigned&)
DEFN_CTORS_T(const sc_fxval&)
DEFN_CTORS_T(const sc_fxval_fast&)
DEFN_CTORS_T(const sc_fxnum&)
DEFN_CTORS_T(const sc_fxnum_fast&)

#undef DEFN_CTORS_T

// copy constructor

inline
sc_ufix_fast::sc_ufix_fast( const sc_ufix_fast& a )
: sc_fxnum_fast( a,
		 sc_fxtype_params(),
		 SC_US,
		 sc_fxcast_switch(),
		 0 )
{}


// assignment operators

#define DEFN_ASN_OP_T(op,tp)                                                  \
inline                                                                        \
sc_ufix_fast&                                                                 \
sc_ufix_fast::operator op ( tp a )                                            \
{                                                                             \
    sc_fxnum_fast::operator op( a );                                          \
    return *this;                                                             \
}

#define DEFN_ASN_OP(op)                                                       \
DEFN_ASN_OP_T(op,short)                                                       \
DEFN_ASN_OP_T(op,unsigned short)                                              \
DEFN_ASN_OP_T(op,int)                                                         \
DEFN_ASN_OP_T(op,unsigned int)                                                \
DEFN_ASN_OP_T(op,long)                                                        \
DEFN_ASN_OP_T(op,unsigned long)                                               \
DEFN_ASN_OP_T(op,float)                                                       \
DEFN_ASN_OP_T(op,double)                                                      \
DEFN_ASN_OP_T(op,const char*)                                                 \
DEFN_ASN_OP_T(op,int64)                                                       \
DEFN_ASN_OP_T(op,uint64)                                                      \
DEFN_ASN_OP_T(op,const sc_int_base&)                                          \
DEFN_ASN_OP_T(op,const sc_uint_base&)                                         \
DEFN_ASN_OP_T(op,const sc_signed&)                                            \
DEFN_ASN_OP_T(op,const sc_unsigned&)                                          \
DEFN_ASN_OP_T(op,const sc_fxval&)                                             \
DEFN_ASN_OP_T(op,const sc_fxval_fast&)                                        \
DEFN_ASN_OP_T(op,const sc_fxnum&)                                             \
DEFN_ASN_OP_T(op,const sc_fxnum_fast&)

DEFN_ASN_OP(=)

DEFN_ASN_OP(*=)
DEFN_ASN_OP(/=)
DEFN_ASN_OP(+=)
DEFN_ASN_OP(-=)

DEFN_ASN_OP_T(<<=,int)
DEFN_ASN_OP_T(>>=,int)

#undef DEFN_ASN_OP_T
#undef DEFN_ASN_OP


// auto-increment and auto-decrement

inline
const sc_fxval_fast
sc_ufix_fast::operator ++ ( int )
{
    return sc_fxval_fast( sc_fxnum_fast::operator ++ ( 0 ) );
}

inline
const sc_fxval_fast
sc_ufix_fast::operator -- ( int )
{
    return sc_fxval_fast( sc_fxnum_fast::operator -- ( 0 ) );
}

inline
sc_ufix_fast&
sc_ufix_fast::operator ++ ()
{
    sc_fxnum_fast::operator ++ ();
    return *this;
}

inline
sc_ufix_fast&
sc_ufix_fast::operator -- ()
{
    sc_fxnum_fast::operator -- ();
    return *this;
}


#endif

// Taf!
