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

    scfx_rep.cpp - 

    Original Author: Robert Graulich. Synopsys, Inc. (graulich@synopsys.com)
                     Martin Janssen.  Synopsys, Inc. (mjanssen@synopsys.com)

******************************************************************************/

/******************************************************************************

    MODIFICATION LOG - modifiers, enter your name, affliation and
    changes you are making here:

    Modifier Name & Affiliation:
    Description of Modification:
    

******************************************************************************/


#include "scfx_rep.h"

#include "scfx_ieee.h"
#include "scfx_pow10.h"
#include "scfx_utils.h"

#include "sc_bv.h"

#include <ctype.h>
#include <math.h>
#include <string.h>

#if defined(__BCPLUSPLUS__)
#pragma warn -ccc
#endif

// ----------------------------------------------------------------------------
//  some utilities
// ----------------------------------------------------------------------------

static scfx_pow10 pow10_fx;

static const int mantissa0_size = SCFX_IEEE_DOUBLE_M_SIZE - bits_in_int;

static inline
int
n_word( int x )
{
    return ( x + bits_in_word - 1 ) / bits_in_word;
}


// ----------------------------------------------------------------------------
//  construct scfx_rep from a float
// ----------------------------------------------------------------------------

scfx_rep::scfx_rep( float f )
: _mant( min_mant ), _wp( 0 ), _state( normal ), _msw( 0 ), _lsw( 0 ),
  _r_flag( false )
{
    _mant.clear();

    scfx_ieee_float id( f );

    _sign = id.negative() ? -1 : 1;

    if( id.is_nan() )
        _state = not_a_number;
    else if( id.is_inf() )
        _state = infinity;
    else if( id.is_subnormal() )
    {
	_mant[1] = id.mantissa();
	normalize( id.exponent() + 1 - SCFX_IEEE_FLOAT_M_SIZE - bits_in_word );
    }
    else if( id.is_normal() )
    {
	_mant[1] = id.mantissa() | ( 1 << SCFX_IEEE_FLOAT_M_SIZE );
	normalize( id.exponent() - SCFX_IEEE_FLOAT_M_SIZE - bits_in_word );
    }
}


// ----------------------------------------------------------------------------
//  construct scfx_rep from a double
// ----------------------------------------------------------------------------

scfx_rep::scfx_rep( double d )
: _mant( min_mant ), _wp( 0 ), _state( normal ), _msw( 0 ), _lsw( 0 ),
  _r_flag( false )
{
    _mant.clear();

    scfx_ieee_double id( d );

    _sign = id.negative() ? -1 : 1;

    if( id.is_nan() )
        _state = not_a_number;
    else if( id.is_inf() )
        _state = infinity;
    else if( id.is_subnormal() )
    {
	_mant[0] = id.mantissa1();
	_mant[1] = id.mantissa0();
	normalize( id.exponent() + 1 - SCFX_IEEE_DOUBLE_M_SIZE );
    }
    else if( id.is_normal() )
    {
	_mant[0] = id.mantissa1();
	_mant[1] = id.mantissa0() | ( 1 << mantissa0_size );
	normalize( id.exponent() - SCFX_IEEE_DOUBLE_M_SIZE );
    }
}


// ----------------------------------------------------------------------------
//  construct scfx_rep from a character string
// ----------------------------------------------------------------------------

#define _SCFX_FAIL_IF(cnd)                                                    \
{                                                                             \
    if( ( cnd ) )                                                             \
    {                                                                         \
        _state = not_a_number;                                                \
        return;                                                               \
    }                                                                         \
}


void
scfx_rep::from_string( const char* s, int cte_wl )
{
    _SCFX_FAIL_IF( s == 0 || *s == 0 );

    scfx_string s2;
    s2 += s;
    s2 += '\0';

    bool sign_char;
    _sign = scfx_parse_sign( s, sign_char );

    sc_numrep numrep = scfx_parse_prefix( s );

    int base = 0;

    switch( numrep )
    {
	case SC_DEC:
	{
	    base = 10;
	    if( scfx_is_nan( s ) )
	    {   // special case: NaN
		_state = not_a_number;
		return;
	    }
	    if( scfx_is_inf( s ) )
	    {   // special case: Infinity
		_state = infinity;
		return;
	    }
	    break;
	}
	case SC_BIN:
	case SC_BIN_US:
	{
	    _SCFX_FAIL_IF( sign_char );
	    base = 2;
	    break;
	}
	
	case SC_BIN_SM:
	{
	    base = 2;
	    break;
	}
	case SC_OCT:
	case SC_OCT_US:
	{
	    _SCFX_FAIL_IF( sign_char );
	    base = 8;
	    break;
	}
	case SC_OCT_SM:
	{
	    base = 8;
	    break;
	}
	case SC_HEX:
	case SC_HEX_US:
	{
	    _SCFX_FAIL_IF( sign_char );
	    base = 16;
	    break;
	}
	case SC_HEX_SM:
	{
	    base = 16;
	    break;
	}
	case SC_CSD:
	{
	    _SCFX_FAIL_IF( sign_char );
	    base = 2;
	    scfx_csd2tc( s2 );
	    s = (const char*) s2 + 4;
	    numrep = SC_BIN;
	    break;
	}
        default:;
    }

    //
    // find end of mantissa and count the digits and points
    //

    const char *end = s;
    bool based_point = false;
    int int_digits = 0;
    int frac_digits = 0;

    while( *end )
    {
	if( scfx_exp_start( end ) )
	    break;
	
	if( *end == '.' )
	{
	    _SCFX_FAIL_IF( based_point );
	    based_point = true;
	}
	else
	{
	    _SCFX_FAIL_IF( ! scfx_is_digit( *end, numrep ) );
	    if( based_point )
		frac_digits ++;
	    else
		int_digits ++;
	}

	++ end;
    }

    _SCFX_FAIL_IF( int_digits == 0 && frac_digits == 0 );

    // [ exponent ]
    
    int exponent = 0;

    if( *end )
    {
	for( const char *e = end + 2; *e; ++ e )
	    _SCFX_FAIL_IF( ! scfx_is_digit( *e, SC_DEC ) );
	exponent = atoi( end + 1 );
    }

    //
    // check if the mantissa is negative
    //

    bool mant_is_neg = false;

    switch( numrep )
    {
	case SC_BIN:
	case SC_OCT:
	case SC_HEX:
	{
	    const char* p = s;
	    if( *p == '.' )
		++ p;

	    mant_is_neg = ( scfx_to_digit( *p, numrep ) >= ( base >> 1 ) );

	    break;
	}
	default:
	    ;
    }

    //
    // convert the mantissa
    //

    switch( base )
    {
        case 2:
	{
	    int bit_offset = exponent % bits_in_word;
	    int word_offset = exponent / bits_in_word;

	    int_digits += bit_offset;
	    frac_digits -= bit_offset;

	    int words = n_word( int_digits ) + n_word( frac_digits );
	    if( words > size() )
		resize_to( words );
	    _mant.clear();

	    int j = n_word( frac_digits ) * bits_in_word + int_digits - 1;
	    
	    for( ; s < end; s ++ )
	    {
		switch( *s )
		{
		    case '1':
		        set_bin( j );
		    case '0':
			j --;
		    case '.':
			break;
		    default:
			_SCFX_FAIL_IF( true );  // should not happen
		}
	    }

	    _wp = n_word( frac_digits ) - word_offset;
	    break;
	}
        case 8:
	{
	    exponent *= 3;
	    int_digits *= 3;
	    frac_digits *= 3;

	    int bit_offset = exponent % bits_in_word;
	    int word_offset = exponent / bits_in_word;

	    int_digits += bit_offset;
	    frac_digits -= bit_offset;

	    int words = n_word( int_digits ) + n_word( frac_digits );
	    if( words > size() )
		resize_to( words );
	    _mant.clear();

	    int j = n_word( frac_digits ) * bits_in_word + int_digits - 3;
	    
	    for( ; s < end; s ++ )
	    {
		switch( *s )
		{
		    case '7': case '6': case '5': case '4':
		    case '3': case '2': case '1':
		        set_oct( j, *s - '0' );
		    case '0':
			j -= 3;
		    case '.':
			break;
		    default:
			_SCFX_FAIL_IF( true );  // should not happen
		}
	    }

	    _wp = n_word( frac_digits ) - word_offset;
	    break;
	}
        case 10:
	{
	    int length = int_digits + frac_digits;
	    resize_to( MAXT( min_mant, n_word( 4 * length ) ) );

	    _mant.clear();
	    _msw = _lsw = 0;
	    
	    for( ; s < end; s ++ )
	    {
		switch( *s )
		{
		    case '9': case '8': case '7': case '6': case '5':
		    case '4': case '3': case '2': case '1': case '0':
		        multiply_by_ten();
			_mant[0] += *s - '0';
		    case '.':
			break;
		    default:
			_SCFX_FAIL_IF( true );  // should not happen
		}
	    }
	    
	    _wp = 0;
	    find_sw();

	    int denominator = frac_digits - exponent;
	    
	    if( denominator )
	    {
		scfx_rep frac_num = pow10_fx( denominator );
		scfx_rep* temp_num =
		    divide( const_cast<const scfx_rep&>( *this ),
			    frac_num, cte_wl );
		*this = *temp_num;
		delete temp_num;
	    }

	    break;
	}
        case 16:
	{
	    exponent *= 4;
	    int_digits *= 4;
	    frac_digits *= 4;

	    int bit_offset = exponent % bits_in_word;
	    int word_offset = exponent / bits_in_word;

	    int_digits += bit_offset;
	    frac_digits -= bit_offset;

	    int words = n_word( int_digits ) + n_word( frac_digits );
	    if( words > size() )
		resize_to( words );
	    _mant.clear();

	    int j = n_word( frac_digits ) * bits_in_word + int_digits - 4;
	    
	    for( ; s < end; s ++ )
	    {
		switch( *s )
		{
		    case 'f': case 'e': case 'd': case 'c': case 'b': case 'a':
		       set_hex( j, *s - 'a' + 10 );
		       j -= 4;
		       break;
		    case 'F': case 'E': case 'D': case 'C': case 'B': case 'A':
		       set_hex( j, *s - 'A' + 10 );
		       j -= 4;
		       break;
		    case '9': case '8': case '7': case '6': case '5':
		    case '4': case '3': case '2': case '1':
		       set_hex( j, *s - '0' );
		    case '0':
		       j -= 4;
		    case '.':
		       break;
		   default:
		       _SCFX_FAIL_IF( true );  // should not happen
		}
	    }

	    _wp = n_word( frac_digits ) - word_offset;
	    break;
	}
    }

    _state = normal;
    find_sw();

    //
    // two's complement of mantissa if it is negative
    //

    if( mant_is_neg )
    {
	_mant[_msw] |=  -1 << scfx_find_msb( _mant[_msw] );
	for( int i = _msw + 1; i < _mant.size(); ++ i )
	    _mant[i] = static_cast<word>( -1 );
	complement( _mant, _mant, _mant.size() );
	inc( _mant );
	_sign *= -1;
    }
}


#undef _SCFX_FAIL_IF


// ----------------------------------------------------------------------------
//  construct scfx_rep from sc_signed
// ----------------------------------------------------------------------------

scfx_rep::scfx_rep( const sc_signed& a )
: _mant( min_mant ), _r_flag( false )
{
    if( a.iszero() )
	set_zero();
    else
    {
	int words = n_word( a.length() );
	if( words > size() )
	    resize_to( words );
	_mant.clear();
	_wp = 0;
	_state = normal;
	if( a.sign() )
	{
	    sc_signed a2 = -a;
	    for( int i = 0; i < a2.length(); ++ i )
	    {
		if( a2[i] )
		{
		    scfx_index x = calc_indices( i );
		    _mant[x.wi()] |= 1 << x.bi();
		}
	    }
	    _sign = -1;
	}
	else
	{
	    for( int i = 0; i < a.length(); ++ i )
	    {
		if( a[i] )
		{
		    scfx_index x = calc_indices( i );
		    _mant[x.wi()] |= 1 << x.bi();
		}
	    }
	    _sign = 1;
	}
	find_sw();
    }
}


// ----------------------------------------------------------------------------
//  construct scfx_rep from sc_unsigned
// ----------------------------------------------------------------------------

scfx_rep::scfx_rep( const sc_unsigned& a )
: _mant( min_mant ), _r_flag( false )
{
    if( a.iszero() )
	set_zero();
    else
    {
	int words = n_word( a.length() );
	if( words > size() )
	    resize_to( words );
	_mant.clear();
	_wp = 0;
	_state = normal;
	for( int i = 0; i < a.length(); ++ i )
	{
	    if( a[i] )
	    {
		scfx_index x = calc_indices( i );
		_mant[x.wi()] |= 1 << x.bi();
	    }
	}
	_sign = 1;
	find_sw();
    }
}


// ----------------------------------------------------------------------------
//  memory management of scfx_rep
// ----------------------------------------------------------------------------

union scfx_rep_node
{
    char           data[sizeof( scfx_rep )];
    scfx_rep_node* next;
};


static scfx_rep_node* list = 0;


void*
scfx_rep::operator new( size_t size )
{
    const int ALLOC_SIZE = 1024;

    if( size != sizeof( scfx_rep ) )
	return ::operator new( size );

    if( ! list )
    {
	list = new scfx_rep_node[ALLOC_SIZE];
	for( int i = 0; i < ALLOC_SIZE - 1; i ++ )
	    list[i].next = list + i + 1;
	list[ALLOC_SIZE - 1].next = 0;
    }

    scfx_rep* ptr = reinterpret_cast<scfx_rep*>( list->data );
    list = list->next;

    return ptr;
}


void scfx_rep::operator delete( void* ptr, size_t size )
{
    if( size != sizeof( scfx_rep ) )
    {
	::operator delete( ptr );
	return;
    }

    scfx_rep_node* node = static_cast<scfx_rep_node*>( ptr );
    node->next = list;
    list = node;
}


// ----------------------------------------------------------------------------
//  convert to double
// ----------------------------------------------------------------------------

double
scfx_rep::to_double() const 
{
    scfx_ieee_double id;

    // handle special cases

    if( is_nan() )
    {
        id.set_nan();
	return id;
    }

    if( is_inf() )
    {
        id.set_inf();
	id.negative( _sign < 0 );
	return id;
    }

    if( is_zero() )
    {
	id = 0.;
	id.negative( _sign < 0 );
	return id;
    }

    int msb = scfx_find_msb( _mant[_msw] );

    int exp = (_msw - _wp) * bits_in_word + msb;

    if( exp > SCFX_IEEE_DOUBLE_E_MAX )
    {
	id.set_inf();
	id.negative( _sign < 0 );
	return id;
    }

    if( exp < SCFX_IEEE_DOUBLE_E_MIN
	- static_cast<int>( SCFX_IEEE_DOUBLE_M_SIZE ) )
    {
	id = 0.;
	return id;
    }
 
    int shift = mantissa0_size - msb;

    unsigned int m0;
    unsigned int m1 = 0;
    unsigned int guard = 0;

    if( shift == 0 )
    {
        m0 = _mant[_msw] & ~( 1 << mantissa0_size );
	if( _msw > _lsw )
	{
	    m1 = _mant[_msw - 1];
	    if( _msw - 1 > _lsw )
	        guard = _mant[_msw - 2] >> ( bits_in_word - 1 );
	}
    }
    else if( shift < 0 )
    {
	m0 = ( _mant[_msw] >> -shift ) & ~( 1 << mantissa0_size );
	m1 = _mant[_msw] << ( bits_in_word + shift );
	if( _msw > _lsw )
	{
	    m1 |= _mant[_msw - 1] >> -shift;
	    guard = ( _mant[_msw - 1] >> ( -shift - 1 ) ) & 1;
	}
    }
    else
    {
	m0 = ( _mant[_msw] << shift ) & ~( 1 << mantissa0_size );
	if( _msw > _lsw )
	{
	    m0 |= _mant[_msw - 1] >> ( bits_in_word - shift );
	    m1 = _mant[_msw - 1] << shift;
	    if( _msw - 1 > _lsw )
	    {
	        m1 |= _mant[_msw - 2] >> ( bits_in_word - shift );
		guard = ( _mant[_msw - 2] >> (bits_in_word - shift - 1) ) & 1;
	    }      
	}
    }

    if( exp < SCFX_IEEE_DOUBLE_E_MIN )
    {
	m0 |= ( 1 << mantissa0_size );

	int subnormal_shift = SCFX_IEEE_DOUBLE_E_MIN - exp;

	if( subnormal_shift < bits_in_word )
	{
	    m1 = m1 >> subnormal_shift
	       | m0 << ( bits_in_word - subnormal_shift );
	    m0 = m0 >> subnormal_shift;
	}
	else
	{
	    m1 = m0 >> ( subnormal_shift - bits_in_word );
	    m0 = 0;
	}

	guard = 0;

	exp = SCFX_IEEE_DOUBLE_E_MIN - 1;
    }

    id.mantissa0( m0 );
    id.mantissa1( m1 );
    id.exponent( exp );
    id.negative( _sign < 0 );

    double result = id;

    if( guard != 0 )
        result += _sign * scfx_pow2( exp - SCFX_IEEE_DOUBLE_M_SIZE );

    return result;
}


// ----------------------------------------------------------------------------
//  convert to string
// ----------------------------------------------------------------------------

void
print_dec( scfx_string& s, const scfx_rep& num, sc_fmt fmt )
{
    if( num.is_neg() )
	s += '-';

    if( num.is_zero() )
    {
	s += '0';
	return;
    }

    // split 'num' into its integer and fractional part

    scfx_rep int_part  = num;
    scfx_rep frac_part = num;

    int i;
    
    for( i = int_part._lsw; i <= int_part._msw && i < int_part._wp; i ++ )
	int_part._mant[i] = 0;
    int_part.find_sw();
    if( int_part._wp < int_part._lsw )
	int_part.resize_to( int_part.size() - int_part._wp, -1 );
    
    for( i = frac_part._msw; i >= frac_part._lsw && i >= frac_part._wp; i -- )
	frac_part._mant[i] = 0;
    frac_part.find_sw();
    if( frac_part._msw == frac_part.size() - 1 )
	frac_part.resize_to( frac_part.size() + 1, 1 );

    // print integer part

    int int_digits = 0;
    int int_zeros  = 0;
    
    if( ! int_part.is_zero() )
    {
	double int_wl = ( int_part._msw - int_part._wp ) * bits_in_word
	              + scfx_find_msb( int_part._mant[int_part._msw] ) + 1;
	int_digits = (int) ceil( int_wl * log10( 2. ) );

	int len = s.length();
	s.append( int_digits );

	bool zero_digits = ( frac_part.is_zero() && fmt != SC_F );

	for( i = int_digits + len - 1; i >= len; i-- )
	{
	    unsigned int remainder = int_part.divide_by_ten();
	    s[i] = '0' + remainder;
	    
	    if( zero_digits )
	    {
		if( remainder == 0 )
		    int_zeros ++;
		else
		    zero_digits = false;
	    }
	}

	// discard trailing zeros from int_part
	s.discard( int_zeros );

	if( s[len] == '0' )
	{
	    // int_digits was overestimated by one
	    s.remove( len );
	    -- int_digits;
	}
    }

    // print fractional part

    int frac_digits = 0;
    int frac_zeros  = 0;

    if( ! frac_part.is_zero() )
    {
	s += '.';

	bool zero_digits = ( int_digits == 0 && fmt != SC_F );

	double frac_wl = ( frac_part._wp - frac_part._msw ) * bits_in_word
	               - scfx_find_msb( frac_part._mant[frac_part._msw] ) - 1;
	frac_zeros = (int) floor( frac_wl * log10( 2. ) );

	scfx_rep temp;
	::multiply( temp, frac_part, pow10_fx( frac_zeros ) );
	frac_part = temp;
	if( frac_part._msw == frac_part.size() - 1 )
	    frac_part.resize_to( frac_part.size() + 1, 1 );
	
	frac_digits = frac_zeros;
	if( ! zero_digits )
	{
	    for( i = 0; i < frac_zeros; i ++ )
		s += '0';
	    frac_zeros = 0;
	}

	while( ! frac_part.is_zero() )
	{
	    frac_part.multiply_by_ten();
	    int n = frac_part._mant[frac_part._msw + 1];
	
	    if( zero_digits )
	    {
		if( n == 0 )
		    frac_zeros ++;
		else
		    zero_digits = false;
	    }
	
	    if( ! zero_digits )
		s += '0' + n;

	    frac_part._mant[frac_part._msw + 1] = 0;
	    frac_digits ++;
	}
    }

    // print exponent
    
    if( fmt != SC_F )
    {
        if( frac_digits == 0 )
	    scfx_print_exp( s, int_zeros );
	else if( int_digits == 0 )
	    scfx_print_exp( s, - frac_zeros );
    }
}


void
print_other( scfx_string& s, const scfx_rep& a, sc_numrep numrep, sc_fmt fmt,
	     const scfx_params* params )
{
    scfx_rep b = a;

    sc_numrep numrep2 = numrep;

    if( numrep == SC_BIN_SM ||
	numrep == SC_OCT_SM ||
	numrep == SC_HEX_SM )
    {
	if( b.is_neg() )
	{
	    s += '-';
	    b = *negate( a );
	}
	switch( numrep )
	{
	    case SC_BIN_SM:
		numrep2 = SC_BIN_US;
		break;
	    case SC_OCT_SM:
		numrep2 = SC_OCT_US;
		break;
	    case SC_HEX_SM:
		numrep2 = SC_HEX_US;
		break;
	    default:
		;
	}
    }
    
    scfx_print_prefix( s, numrep );

    numrep = numrep2;

    int msb, lsb;

    if( params != 0 )
    {
	msb = params->iwl() - 1;
	lsb = params->iwl() - params->wl();

	if( params->enc() == SC_TC &&
	    ( numrep == SC_BIN_US ||
	      numrep == SC_OCT_US ||
	      numrep == SC_HEX_US ) &&
	    params->wl() > 1 )
	    -- msb;
    }
    else
    {
	if( b.is_zero() )
	{
	    msb = 0;
	    lsb = 0;
	}
	else
	{
	    msb = ( b._msw - b._wp ) * bits_in_word
		+ scfx_find_msb( b._mant[ b._msw ] ) + 1;
	    while( b.get_bit( msb ) == b.get_bit( msb - 1 ) )
		-- msb;

	    if( numrep == SC_BIN_US ||
		numrep == SC_OCT_US ||
		numrep == SC_HEX_US )
		-- msb;

	    lsb = ( b._lsw - b._wp ) * bits_in_word
		+ scfx_find_lsb( b._mant[ b._lsw ] );
	}
    }

    int step;

    switch( numrep )
    {
	case SC_BIN:
	case SC_BIN_US:
	case SC_CSD:
	    step = 1;
	   break;
	case SC_OCT:
	case SC_OCT_US:
	    step = 3;
	    break;
	case SC_HEX:
	case SC_HEX_US:
	    step = 4;
	    break;
	default:
	    step = 0;
    }

    msb = (int) ceil( double( msb + 1 ) / step ) * step - 1;

    lsb = (int) floor( double( lsb ) / step ) * step;

    if( msb < 0 )
    {
	s += '.';
	if( fmt == SC_F )
	{
	    int sign = ( b.is_neg() ) ? ( 1 << step ) - 1 : 0;
	    for( int i = ( msb + 1 ) / step; i < 0; i ++ )
	    {
		if( sign < 10 )
		    s += sign + '0';
		else
		    s += sign + 'a' - 10;
	    }
	}
    }

    int i = msb;
    while( i >= lsb )
    {
        int value = 0;
        for( int j = step - 1; j >= 0; -- j )
	{
            value += static_cast<int>( b.get_bit( i ) ) << j;
            -- i;
        }
        if( value < 10 )
            s += value + '0';
	else
            s += value + 'a' - 10;
	if( i == -1 )
	    s += '.';
    }

    if( lsb > 0 && fmt == SC_F )
    {
	for( int i = lsb / step; i > 0; i -- )
	    s += '0';
    }

    if( s[s.length() - 1] == '.' )
	s.discard( 1 );

    if( fmt != SC_F )
    {
	if( msb < 0 )
	    scfx_print_exp( s, ( msb + 1 ) / step );
	else if( lsb > 0 )
	    scfx_print_exp( s, lsb / step );
    }

    if( numrep == SC_CSD )
	scfx_tc2csd( s );
}


const char*
scfx_rep::to_string( sc_numrep numrep, sc_fmt fmt,
		     const scfx_params* params ) const
{
    static scfx_string s;

    s.clear();

    if( is_nan() )
        scfx_print_nan( s );
    else if( is_inf() )
        scfx_print_inf( s, is_neg() );
    else if( is_neg() && ! is_zero() &&
	     ( numrep == SC_BIN_US ||
	       numrep == SC_OCT_US ||
	       numrep == SC_HEX_US ) )
        s += "negative";
    else if( numrep == SC_DEC )
        ::print_dec( s, *this, fmt );
    else
        ::print_other( s, *this, numrep, fmt, params );

    return s;
}


// ----------------------------------------------------------------------------
// non-destructive binary not
// ----------------------------------------------------------------------------

scfx_rep*
b_not( const scfx_rep& num, const scfx_params& params )
{
    scfx_rep& result = *new scfx_rep;

    //
    // check for special cases
    //

    if( num.is_nan() || num.is_inf() )
    {
	result.set_nan();
	return &result;
    }

    //
    // do it
    //

    int msb = params.iwl() - 1;
    int lsb = params.iwl() - params.wl();

    for( int i = msb; i >= lsb; -- i )
    {
        if( ! num.get_bit( i ) )
	{
	    result.set( i, params );
	}
    }

    result.find_sw();

    return &result;
}


// ----------------------------------------------------------------------------
//  ADD
//
//  add two mantissas of the same size
//  result has the same size
//  returns carry of operation
// ----------------------------------------------------------------------------

static inline
int
add_mants( int size, scfx_mant& result,
	   const scfx_mant& a, const scfx_mant& b )
{
    unsigned int carry = 0;

    int index = 0;

    do
    {
        word x = a[index];
	word y = b[index];

	y += carry;
	carry = y < carry;
	y += x;
	carry += y < x;
	result[index] = y;
    }
    while( ++ index < size );

    return ( carry ? 1 : 0 );
}


static inline
int
sub_mants( int size, scfx_mant& result,
	   const scfx_mant& a, const scfx_mant& b )
{
    unsigned carry = 0;

    int index = 0;

    do
    {
	word x = a[index];
	word y = b[index];

	y += carry;
	carry = y < carry;
	y = x - y;
	carry += y > x;
	result[index] = y;
    }
    while( ++ index < size );

    return ( carry ? 1 : 0 );
}


scfx_rep*
add( const scfx_rep& lhs, const scfx_rep& rhs, int max_wl )
{
    scfx_rep& result = *new scfx_rep;

    //
    // check for special cases
    //

    if( lhs.is_nan() || rhs.is_nan()
    ||  ( lhs.is_inf() && rhs.is_inf() && lhs._sign != rhs._sign ) )
    {
	result.set_nan();
	return &result;
    }

    if( lhs.is_inf() )
    {
	result.set_inf( lhs._sign );
	return &result;
    }

    if( rhs.is_inf() )
    {
	result.set_inf( rhs._sign );
	return &result;
    }

    //
    // align operands if needed
    //

    scfx_mant_ref lhs_mant;
    scfx_mant_ref rhs_mant;

    int len_mant = lhs.size();
    int new_wp = lhs._wp;

    align( lhs, rhs, new_wp, len_mant, lhs_mant, rhs_mant );

    //
    // size the result mantissa
    //

    result.resize_to( len_mant );
    result._wp = new_wp;

    //
    // do it
    //

    if( lhs._sign == rhs._sign )
    {
	add_mants( len_mant, result._mant, lhs_mant, rhs_mant );
	result._sign = lhs._sign;
    }
    else
    {
	int cmp = compare_abs( lhs, rhs );

	if( cmp == 1 )
	{
	    sub_mants( len_mant, result._mant, lhs_mant, rhs_mant );
	    result._sign = lhs._sign;
	}
	else if ( cmp == -1 )
	{
	    sub_mants( len_mant, result._mant, rhs_mant, lhs_mant );
	    result._sign = rhs._sign;
	}
	else
	{
	    result._mant.clear();
	    result._sign = 1;
	}
    }

    result.find_sw();
    result.round( max_wl );

    return &result;
}


// ----------------------------------------------------------------------------
//  SUB
//
//  sub two word's of the same size
//  result has the same size
//  returns carry of operation
// ----------------------------------------------------------------------------

static inline
int
sub_with_index(       scfx_mant& a, int a_msw, int a_lsw,
		const scfx_mant& b, int b_msw, int b_lsw )
{
    unsigned carry = 0;

    int size    = b_msw - b_lsw;
    int a_index = a_msw - size;
    int b_index = b_msw - size;

    do
    {
	word x = a[a_index];
	word y = b[b_index];

	y += carry;
	carry = y < carry;
	y = x - y;
	carry += y > x;
	a[a_index] = y;

	a_index ++;
	b_index ++;
    }
    while( size -- );

    if( carry )
    {
        // special case: a[a_msw + 1 ] == 1
        a[a_msw + 1] = 0;
    }

    return ( carry ? 1 : 0 );
}


scfx_rep*
subtract( const scfx_rep& lhs, const scfx_rep& rhs, int max_wl )
{
    scfx_rep& result = *new scfx_rep;

    //
    // check for special cases
    //

    if( lhs.is_nan() || rhs.is_nan()
    ||  ( lhs.is_inf() && rhs.is_inf() && lhs._sign != rhs._sign ) )
    {
	result.set_nan();
	return &result;
    }

    if( lhs.is_inf() )
    {
	result.set_inf( lhs._sign );
	return &result;
    }

    if( rhs.is_inf() )
    {
	result.set_inf( rhs._sign );
	return &result;
    }

    //
    // align operands if needed
    //

    scfx_mant_ref lhs_mant;
    scfx_mant_ref rhs_mant;

    int len_mant = lhs.size();
    int new_wp = lhs._wp;

    align( lhs, rhs, new_wp, len_mant, lhs_mant, rhs_mant );

    //
    // size the result mantissa
    //

    result.resize_to( len_mant );
    result._wp = new_wp;

    //
    // do it
    //

    if( lhs._sign != rhs._sign )
    {
	add_mants( len_mant, result._mant, lhs_mant, rhs_mant );
	result._sign = lhs._sign;
    }
    else
    {
	int cmp = compare_abs( lhs, rhs );

	if( cmp == 1 )
	{
	    sub_mants( len_mant, result._mant, lhs_mant, rhs_mant );
	    result._sign = lhs._sign;
	}
	else if ( cmp == -1 )
	{
	    sub_mants( len_mant, result._mant, rhs_mant, lhs_mant );
	    result._sign = -rhs._sign;
	} else {
	    result._mant.clear();
	    result._sign = 1;
	}
    }

    result.find_sw();
    result.round( max_wl );

    return &result;
}


// ----------------------------------------------------------------------------
//  MUL
// ----------------------------------------------------------------------------

union long_short
{
    word l;
    struct
    {
#if defined( SCFX_BIG_ENDIAN )
        half_word u;
        half_word l;
#elif defined( SCFX_LITTLE_ENDIAN )
        half_word l;
        half_word u;
#endif
    } s;
};


#if defined( SCFX_BIG_ENDIAN )
static const int half_word_incr = -1;
#elif defined( SCFX_LITTLE_ENDIAN )
static const int half_word_incr = 1;
#endif


void
multiply( scfx_rep& result, const scfx_rep& lhs, const scfx_rep& rhs,
	  int max_wl )
{
    //
    // check for special cases
    //

    if( lhs.is_nan() || rhs.is_nan()
    ||  lhs.is_inf() && rhs.is_zero()
    ||  lhs.is_zero() && rhs.is_inf() )
    {
	result.set_nan();
	return;
    }

    if( lhs.is_inf() || rhs.is_inf() )
    {
	result.set_inf( lhs._sign * rhs._sign );
	return;
    }

    //
    // do it
    //

    int len_lhs = lhs._msw - lhs._lsw + 1;
    int len_rhs = rhs._msw - rhs._lsw + 1;

    int new_size = MAXT( min_mant, len_lhs + len_rhs );
    int new_wp   = ( lhs._wp - lhs._lsw ) + ( rhs._wp - rhs._lsw );
    int new_sign = lhs._sign * rhs._sign;

    result.resize_to( new_size );
    result._mant.clear();
    result._wp    = new_wp;
    result._sign  = new_sign;
    result._state = normal;

    half_word *s1 = lhs._mant.half_addr( lhs._lsw );
    half_word *s2 = rhs._mant.half_addr( rhs._lsw );

    half_word *t = result._mant.half_addr();

    len_lhs <<= 1;
    len_rhs <<= 1;

    int i1, i2;

    for( i1 = 0; i1 * half_word_incr < len_lhs; i1 += half_word_incr )
    {
	register long_short ls;
	ls.l = 0;

	half_word v1 = s1[i1];

	for( i2  = 0; i2 * half_word_incr < len_rhs; i2 += half_word_incr )
	{
	    ls.l  += v1 * s2[i2];
	    ls.s.l = ls.s.u + ( ( t[i2] += ls.s.l ) < ls.s.l );
	    ls.s.u = 0;
	}

	t[i2] = ls.s.l;
	t += half_word_incr;
    }

    result.find_sw();
    result.round( max_wl );
}


// ----------------------------------------------------------------------------
//  DIV
// ----------------------------------------------------------------------------

scfx_rep*
divide( const scfx_rep& lhs, const scfx_rep& rhs, int div_wl )
{
    scfx_rep& result = *new scfx_rep;

    //
    // check for special cases
    //

    if( lhs.is_nan() || rhs.is_nan() || lhs.is_inf() && rhs.is_inf() ||
	lhs.is_zero() && rhs.is_zero() )
    {
	result.set_nan();
	return &result;
    }

    if( lhs.is_inf() || rhs.is_zero() )
    {
	result.set_inf( lhs._sign * rhs._sign );
	return &result;
    }

    if( rhs.is_inf() )
    {
	result.set_zero( lhs._sign * rhs._sign );
	return &result;
    }

    //
    // do it
    //

    // compute one bit more for rounding
    div_wl ++;

    result.resize_to( MAXT( n_word( div_wl ) + 1, min_mant ) );
    result._mant.clear();
    result._sign = lhs._sign * rhs._sign;

    int msb_lhs = scfx_find_msb( lhs._mant[lhs._msw] )
	        + ( lhs._msw - lhs._wp ) * bits_in_word;
    int msb_rhs = scfx_find_msb( rhs._mant[rhs._msw] )
	        + ( rhs._msw - rhs._wp ) * bits_in_word;

    int msb_res = msb_lhs - msb_rhs;
    int to_shift = -msb_res % bits_in_word;
    int result_index;

    int c = ( msb_res % bits_in_word >= 0 ) ? 1 : 0;

    result_index = (result.size() - c) * bits_in_word + msb_res % bits_in_word;
    result._wp   = (result.size() - c) - msb_res / bits_in_word;

    scfx_rep remainder = lhs;

    // align msb from remainder to msb from rhs
    remainder.lshift( to_shift );

    // make sure msw( remainder ) < size - 1
    if( remainder._msw == remainder.size() - 1 )
	remainder.resize_to( remainder.size() + 1, 1 );

    // make sure msw( remainder ) >= msw( rhs )!
    int msw_diff = rhs._msw - remainder._msw;
    if (msw_diff > 0)
	remainder.resize_to( remainder.size() + msw_diff, -1 );

    int counter;

    for( counter = div_wl; counter && ! remainder.is_zero(); counter -- )
    {
	if( compare_msw_ff( rhs, remainder ) <= 0 )
	{
	    result.set_bin( result_index );
	    sub_with_index( remainder._mant, remainder._msw, remainder._lsw,
			    rhs._mant, rhs._msw, rhs._lsw );
	}
	result_index --;
	remainder.shift_left( 1 );
    }

    // perform convergent rounding, if needed
    if( counter == 0 )
    {
	int index = result_index + 1 - result._wp * bits_in_word;

	scfx_index x = result.calc_indices( index );
	scfx_index x1 = result.calc_indices( index + 1 );

	if( result.o_bit_at( x ) && result.o_bit_at( x1 ) )
	    result.q_incr( x );

	result._r_flag = true;
    }

    result.find_sw();

    return &result;
}


// ----------------------------------------------------------------------------
//  non-destructive binary and
// ----------------------------------------------------------------------------

scfx_rep*
b_and( const scfx_rep& lhs, const scfx_rep& rhs, const scfx_params& )
{
    scfx_rep& result = *new scfx_rep;

    //
    // check for special cases
    //

    if( lhs.is_nan() || rhs.is_nan() ||
	lhs.is_inf() || rhs.is_inf() )
    {
	result.set_nan();
	return &result;
    }

    //
    // align operands if needed
    //

    scfx_mant_ref lhs_mant;
    scfx_mant_ref rhs_mant;

    int len_mant = lhs.size();
    int new_wp = lhs._wp;

    align( lhs, rhs, new_wp, len_mant, lhs_mant, rhs_mant );

    //
    // go to two's complement
    //

    if( lhs.is_neg() )
    {
	lhs_mant = lhs.resize( len_mant, new_wp );
	complement( lhs_mant, lhs_mant, len_mant );
	inc( lhs_mant );
    }

    if( rhs.is_neg() )
    {
	rhs_mant = rhs.resize( len_mant, new_wp );
	complement( rhs_mant, rhs_mant, len_mant );
	inc( rhs_mant );
    }

    //
    // do it
    //

    result.resize_to( len_mant );

    for( int i = 0; i < result.size(); i ++ )
	result._mant[i] = lhs_mant[i] & rhs_mant[i];

    result._wp = new_wp;
    result._sign = ( lhs._sign == -1  &&  rhs._sign == -1 ) ? -1 : 1;

    //
    // go back to sign and magnitude
    //

    if( result.is_neg() )
    {
	complement( result._mant, result._mant, result.size() );
	inc( result._mant );
    }

    result.find_sw();

    return &result;
}


// ----------------------------------------------------------------------------
//  non-destructive binary xor
// ----------------------------------------------------------------------------

scfx_rep*
b_xor( const scfx_rep& lhs, const scfx_rep& rhs, const scfx_params& )
{
    scfx_rep& result = *new scfx_rep;

    //
    // check for special cases
    //

    if( lhs.is_nan() || rhs.is_nan() ||
	lhs.is_inf() || rhs.is_inf() )
    {
	result.set_nan();
	return &result;
    }

    //
    // align operands if needed
    //

    scfx_mant_ref lhs_mant;
    scfx_mant_ref rhs_mant;

    int len_mant = lhs.size();
    int new_wp = lhs._wp;

    align( lhs, rhs, new_wp, len_mant, lhs_mant, rhs_mant );

    //
    // go to two's complement
    //

    if( lhs.is_neg() )
    {
	lhs_mant = lhs.resize( len_mant, new_wp );
	complement( lhs_mant, lhs_mant, len_mant );
	inc( lhs_mant );
    }

    if( rhs.is_neg() )
    {
	rhs_mant = rhs.resize( len_mant, new_wp );
	complement( rhs_mant, rhs_mant, len_mant );
	inc( rhs_mant );
    }

    //
    // do it
    //

    result.resize_to( len_mant );

    for( int i = 0; i < result.size(); i ++ )
	result._mant[i] = lhs_mant[i] ^ rhs_mant[i];

    result._wp = new_wp;
    result._sign = ( lhs._sign !=  rhs._sign ) ? -1 : 1;

    //
    // go back to sign and magnitude
    //

    if( result.is_neg() )
    {
	complement( result._mant, result._mant, result.size() );
	inc( result._mant );
    }

    result.find_sw();

    return &result;
}


// ----------------------------------------------------------------------------
//  non-destructive binary or
// ----------------------------------------------------------------------------

scfx_rep*
b_or( const scfx_rep& lhs, const scfx_rep& rhs, const scfx_params& )
{
    scfx_rep& result = *new scfx_rep;

    //
    // check for special cases
    //

    if( lhs.is_nan() || rhs.is_nan() ||
	lhs.is_inf() || rhs.is_inf() )
    {
	result.set_nan();
	return &result;
    }

    //
    // align operands if needed
    //

    scfx_mant_ref lhs_mant;
    scfx_mant_ref rhs_mant;

    int len_mant = lhs.size();
    int new_wp = lhs._wp;

    align( lhs, rhs, new_wp, len_mant, lhs_mant, rhs_mant );

    //
    // go to two's complement
    //

    if( lhs.is_neg() )
    {
	lhs_mant = lhs.resize( len_mant, new_wp );
	complement( lhs_mant, lhs_mant, len_mant );
	inc( lhs_mant );
    }

    if( rhs.is_neg() )
    {
	rhs_mant = rhs.resize( len_mant, new_wp );
	complement( rhs_mant, rhs_mant, len_mant );
	inc( rhs_mant );
    }

    //
    // do it
    //

    result.resize_to( len_mant );

    for( int i = 0; i < result.size(); i ++ )
	result._mant[i] = lhs_mant[i] | rhs_mant[i];

    result._wp = new_wp;
    result._sign = ( lhs._sign == -1 || rhs._sign == -1 ) ? -1 : 1;

    //
    // go back to sign and magnitude
    //

    if( result.is_neg() )
    {
	complement( result._mant, result._mant, result.size() );
	inc( result._mant );
    }

    result.find_sw();

    return &result;
}


// ----------------------------------------------------------------------------
//  destructive shift mantissa to the left
// ----------------------------------------------------------------------------

void
scfx_rep::lshift( int n )
{
    if( n == 0 )
        return;

    if( n < 0 )
    {
        rshift( -n );
	return;
    }

    if( is_normal() )
    {
        int shift_bits  = n % bits_in_word;
	int shift_words = n / bits_in_word;

	// resize if needed
	if( _msw == size() - 1 &&
	    scfx_find_msb( _mant[_msw] ) >= bits_in_word - shift_bits )
	    resize_to( size() + 1, 1 );

	// do it
	_wp -= shift_words;
	shift_left( shift_bits );
	find_sw();
    }
}


// ----------------------------------------------------------------------------
//  destructive shift mantissa to the right
// ----------------------------------------------------------------------------

void
scfx_rep::rshift( int n )
{
    if( n == 0 )
        return;

    if( n < 0 )
    {
        lshift( -n );
	return;
    }

    if( is_normal() )
    {
        int shift_bits  = n % bits_in_word;
	int shift_words = n / bits_in_word;

	// resize if needed
	if( _lsw == 0 && scfx_find_lsb( _mant[_lsw] ) < shift_bits )
	    resize_to( size() + 1, -1 );

	// do it
	_wp += shift_words;
	shift_right( shift_bits );
	find_sw();
    }
}


// ----------------------------------------------------------------------------
//  FRIEND FUNCTION : compare_abs
//
//  Compares the absolute values of two scfx_reps, excluding the special cases.
// ----------------------------------------------------------------------------

int
compare_abs( const scfx_rep& a, const scfx_rep& b )
{
    // check for zero

    word a_word = a._mant[a._msw];
    word b_word = b._mant[b._msw];
  
    if( a_word == 0 || b_word == 0 )
    {
	if( a_word != 0 )
	    return 1;
	if( b_word != 0 )
	    return -1;
	return 0;
    }

    // compare msw index

    int a_msw = a._msw - a._wp;
    int b_msw = b._msw - b._wp;

    if( a_msw > b_msw )
	return 1;

    if( a_msw < b_msw )
	return -1;

    // compare content

    int a_i = a._msw;
    int b_i = b._msw;

    while( a_i >= a._lsw && b_i >= b._lsw )
    {
	a_word = a._mant[a_i];
	b_word = b._mant[b_i];
	if( a_word > b_word )
	    return 1;
	if( a_word < b_word )
	    return -1;
	-- a_i;
	-- b_i;
    }

    bool a_zero = true;
    while( a_i >= a._lsw )
    {
	a_zero &= ( a._mant[a_i] == 0 );
	-- a_i;
    }
  
    bool b_zero = true;
    while( b_i >= b._lsw )
    {
	b_zero &= ( b._mant[b_i] == 0 );
	-- b_i;
    }

    // assertion: a_zero || b_zero == true

    if( ! a_zero && b_zero )
	return 1;

    if( a_zero && ! b_zero )
	return -1;

    return 0;
}


// ----------------------------------------------------------------------------
//  FRIEND FUNCTION : compare
//
//  Compares the values of two scfx_reps, including the special cases.
// ----------------------------------------------------------------------------

int
compare( const scfx_rep& a, const scfx_rep& b )
{
    // handle special cases

    if( a.is_nan() || b.is_nan() )
    {
	if( a.is_nan() && b.is_nan() )
	{
	    return 0;
	}
	return 2;
    }

    if( a.is_inf() || b.is_inf() )
    {
	if( a.is_inf() )
	{
	    if( ! a.is_neg() )
	    {
		if( b.is_inf() && ! b.is_neg() )
		{
		    return 0;
		}
		else
		{
		    return 1;
		}
	    }
	    else
	    {
		if( b.is_inf() && b.is_neg() )
		{
		    return 0;
		}
		else
		{
		    return -1;
		}
	    }
	}
	if( b.is_inf() )
	{
	    if( ! b.is_neg() )
	    {
		return -1;
	    }
	    else
	    {
		return 1;
	    }
	}
    }
  
    if( a.is_zero() && b.is_zero() )
    {
	return 0;
    }

    // compare sign

    if( a._sign != b._sign )
    {
	return a._sign;
    }

    return ( a._sign * compare_abs( a, b ) );
}


// ----------------------------------------------------------------------------
//  PRIVATE METHOD : quantization
//
//  Performs destructive quantization.
// ----------------------------------------------------------------------------

void
scfx_rep::quantization( const scfx_params& params, bool& q_flag )
{
    scfx_index x = calc_indices( params.iwl() - params.wl() );

    if( x.wi() < 0 )
        return;

    if( x.wi() >= size() )
        resize_to( x.wi() + 1, 1 );

    bool qb = q_bit( x );
    bool qz = q_zero( x );

    q_flag = ( qb || ! qz );

    if( q_flag )
    {
        switch( params.q_mode() )
	{
            case SC_TRN:			// truncation
	    {
	        if( is_neg() )
		    q_incr( x );
		break;
	    }
            case SC_RND:			// rounding to plus infinity
	    {
	        if( ! is_neg() )
		{
		    if( qb )
			q_incr( x );
		}
		else
		{
		    if( qb && ! qz )
			q_incr( x );
		}
		break;
	    }
            case SC_TRN_ZERO:			// truncation to zero
	    {
	        break;
	    }
            case SC_RND_INF:			// rounding to infinity
	    {
	        if( qb )
		    q_incr( x );
		break;
	    }
            case SC_RND_CONV:			// convergent rounding
	    {
		if( qb && ! qz || qb && qz && q_odd( x ) )
		    q_incr( x );
		break;
	    }
            case SC_RND_ZERO:			// rounding to zero
	    {
		if( qb && ! qz )
		    q_incr( x );
		break;
	    }
            case SC_RND_MIN_INF:		// rounding to minus infinity
	    {
		if( ! is_neg() )
		{
		    if( qb && ! qz )
			q_incr( x );
		}
		else
		{
		    if( qb )
			q_incr( x );
		}
		break;
	    }
            default:
	        ;
	}
	q_clear( x );
	
	find_sw();
    }
}


// ----------------------------------------------------------------------------
//  PRIVATE METHOD : overflow
//
//  Performs destructive overflow handling.
// ----------------------------------------------------------------------------

void
scfx_rep::overflow( const scfx_params& params, bool& o_flag )
{
    scfx_index x = calc_indices( params.iwl() - 1 );

    if( x.wi() >= size() )
        resize_to( x.wi() + 1, 1 );

    if( x.wi() < 0 )
    {
        resize_to( size() - x.wi(), -1 );
        x.wi( 0 );
    }

    bool zero_left = o_zero_left( x );
    bool bit_at = o_bit_at( x );
    bool zero_right = o_zero_right( x );

    bool under = false;
    bool over = false;

    sc_enc enc = params.enc();

    if( enc == SC_TC )
    {
	if( is_neg() )
	{
	    if( params.o_mode() == SC_SAT_SYM )
		under = ( ! zero_left || bit_at );
	    else
		under = ( ! zero_left || zero_left && bit_at && ! zero_right );
	}
	else
	    over = ( ! zero_left || bit_at );
    }
    else
    {
	if( is_neg() )
	    under = ( ! is_zero() );
        else
	    over = ( ! zero_left );
    }

    o_flag = ( under || over );

    if( o_flag )
    {
	scfx_index x2 = calc_indices( params.iwl() - params.wl() );

	if( x2.wi() < 0 )
	{
	    resize_to( size() - x2.wi(), -1 );
	    x.wi( x.wi() - x2.wi() );
	    x2.wi( 0 );
	}

	switch( params.o_mode() )
	{
            case SC_WRAP:			// wrap-around
	    {
		int n_bits = params.n_bits();

		if( n_bits == 0 )
		{
		    // wrap-around all 'wl' bits
		    toggle_tc();
		    o_extend( x, enc );
		    toggle_tc();
		}
		else if( n_bits < params.wl() )
		{
		    scfx_index x3 = calc_indices( params.iwl() - 1 - n_bits );

		    // wrap-around least significant 'wl - n_bits' bits;
		    // saturate most significant 'n_bits' bits
		    toggle_tc();
		    o_set( x, x3, enc, under );
		    o_extend( x, enc );
		    toggle_tc();
		}
		else
		{
		    // saturate all 'wl' bits
		    if( under )
			o_set_low( x, enc );
		    else
			o_set_high( x, x2, enc );
		}
		break;
	    }
            case SC_SAT:			// saturation
	    {
		if( under )
		    o_set_low( x, enc );
		else
		    o_set_high( x, x2, enc );
		break;
	    }
            case SC_SAT_SYM:			// symmetrical saturation
	    {
		if( under )
		{
		    if( enc == SC_TC )
			o_set_high( x, x2, SC_TC, -1 );
		    else
			o_set_low( x, SC_US );
		}
		else
		    o_set_high( x, x2, enc );
		break;
	    }
            case SC_SAT_ZERO:			// saturation to zero
	    {
		set_zero();
		break;
	    }
            case SC_WRAP_SM:			// sign magnitude wrap-around
	    {
		_SC_ASSERT( enc == SC_TC,
			    "SC_WRAP_SM not defined for unsigned numbers" );

		int n_bits = params.n_bits();

		if( n_bits == 0 )
		{
		    scfx_index x4 = calc_indices( params.iwl() );

		    if( x4.wi() >= size() )
			resize_to( x4.wi() + 1, 1 );

		    toggle_tc();
		    if( o_bit_at( x4 ) != o_bit_at( x ) )
			o_invert( x2 );
		    o_extend( x, SC_TC );
		    toggle_tc();
		}
		else if( n_bits == 1 )
		{
		    toggle_tc();
		    if( is_neg() != o_bit_at( x ) )
			o_invert( x2 );
		    o_extend( x, SC_TC );
		    toggle_tc();
		}
		else if( n_bits < params.wl() )
		{
		    scfx_index x3 = calc_indices( params.iwl() - 1 - n_bits );
		    scfx_index x4 = calc_indices( params.iwl() - n_bits );

		    // wrap-around least significant 'wl - n_bits' bits;
		    // saturate most significant 'n_bits' bits
		    toggle_tc();
		    if( is_neg() == o_bit_at( x4 ) )
			o_invert( x2 );
		    o_set( x, x3, SC_TC, under );
		    o_extend( x, SC_TC );
		    toggle_tc();
		}
		else
		{
		    if( under )
			o_set_low( x, SC_TC );
		    else
			o_set_high( x, x2, SC_TC );
		}
		break;
	    }
            default:
	        ;
	}

	find_sw();
    }
}


// ----------------------------------------------------------------------------
//  PUBLIC METHOD : cast
//
//  Performs a destructive cast operation on a scfx_rep.
// ----------------------------------------------------------------------------

void
scfx_rep::cast( const scfx_params& params, bool& q_flag, bool& o_flag )
{
    q_flag = false;
    o_flag = false;

    // check for special cases
    
    if( is_zero() )
    {
	if( is_neg() )
	    _sign = 1;
	return;
    }

    // perform casting

    quantization( params, q_flag );
    overflow( params, o_flag );

    // check for special case: -0

    if( is_zero() && is_neg() )
	_sign = 1;
}


// ----------------------------------------------------------------------------
//  make sure, the two mantissas are aligned
// ----------------------------------------------------------------------------

void
align( const scfx_rep& lhs, const scfx_rep& rhs, int& new_wp,
       int& len_mant, scfx_mant_ref& lhs_mant, scfx_mant_ref& rhs_mant )
{
    bool need_lhs = true;
    bool need_rhs = true;

    if( lhs._wp != rhs._wp || lhs.size() != rhs.size() )
    {
	int lower_bound_lhs = lhs._lsw - lhs._wp;
	int upper_bound_lhs = lhs._msw - lhs._wp;
	int lower_bound_rhs = rhs._lsw - rhs._wp;
	int upper_bound_rhs = rhs._msw - rhs._wp;

	int lower_bound = MINT( lower_bound_lhs, lower_bound_rhs );
	int upper_bound = MAXT( upper_bound_lhs, upper_bound_rhs );

	new_wp   = -lower_bound;
	len_mant = MAXT( min_mant, upper_bound - lower_bound + 1 );

	if( new_wp != lhs._wp || len_mant != lhs.size() )
	{
	    lhs_mant = lhs.resize( len_mant, new_wp );
	    need_lhs = false;
	}

	if( new_wp != rhs._wp || len_mant != rhs.size() )
        {
	    rhs_mant = rhs.resize( len_mant, new_wp );
	    need_rhs = false;
	}
    }

    if( need_lhs )
    {
	lhs_mant = lhs._mant;
    }

    if( need_rhs )
    {
	rhs_mant = rhs._mant;
    }
}


// ----------------------------------------------------------------------------
//  compare two mantissas
// ----------------------------------------------------------------------------

int
compare_msw_ff( const scfx_rep& lhs, const scfx_rep& rhs )
{
    // special case: rhs._mant[rhs._msw + 1] == 1
    if( rhs._msw < rhs.size() - 1 && rhs._mant[rhs._msw + 1 ] != 0 )
    {
	return -1;
    }

    int lhs_size = lhs._msw - lhs._lsw + 1;
    int rhs_size = rhs._msw - rhs._lsw + 1;

    int size = MINT( lhs_size, rhs_size );

    int lhs_index = lhs._msw;
    int rhs_index = rhs._msw;

    int i;

    for( i = 0; i < size && lhs._mant[lhs_index] == rhs._mant[rhs_index]; i ++ )
    {
	lhs_index --;
	rhs_index --;
    }

    if( i == size )
    {
	if( lhs_size == rhs_size )
	{
	    return 0;
	}

	if( lhs_size < rhs_size )
	{
	    return -1;
	}
	else
	{
	    return 1;
	}
  }

  if( lhs._mant[lhs_index] < rhs._mant[rhs_index] )
  {
      return -1;
  } else {
      return 1;
  }
}


#if 0

// ----------------------------------------------------------------------------
//  compare the most significant words of two scfx_reps
// ----------------------------------------------------------------------------

int
compare_msw( const scfx_rep& a, const scfx_rep& b )
{
    word a_word = a._mant[a._msw];
    word b_word = b._mant[b._msw];
  
    if( a_word == 0 || b_word == 0 )
    {
	if( a_word != 0 )
	{
	    return 1;
	}
	if( b_word != 0 )
	{
	    return -1;
	}
	return 0;
    }

    int a_msw = a._msw - a._wp;
    int b_msw = b._msw - b._wp;

    if( a_msw < b_msw )
    {
	return -1;
    }

    if( a_msw > b_msw )
    {
	return 1;
    }

    if( a_word < b_word )
    {
	return -1;
    }

    if( a_word > b_word )
    {
	return 1;
    }

    return 0;
}

#endif


// ----------------------------------------------------------------------------
//  divide the mantissa by ten
// ----------------------------------------------------------------------------

unsigned int
scfx_rep::divide_by_ten()
{
#if defined( SCFX_BIG_ENDIAN )
    half_word* hw = (half_word*) &_mant[_msw];
#elif defined( SCFX_LITTLE_ENDIAN )
    half_word* hw = ( (half_word*) &_mant[_msw] ) + 1;
#endif

    unsigned int remainder = 0;

    long_short ls;
    ls.l = 0;

#if defined( SCFX_BIG_ENDIAN )
    for( int i = 0, end = ( _msw - _wp + 1 ) * 2; i < end; i ++ )
#elif defined( SCFX_LITTLE_ENDIAN )
    for( int i = 0, end = -( _msw - _wp + 1 ) * 2; i > end; i -- )
#endif
    {
	ls.s.u = remainder;
	ls.s.l = hw[i];
	remainder = ls.l % 10;
	ls.l /= 10;
	hw[i] = ls.s.l;
    }

    return remainder;
}


// ----------------------------------------------------------------------------
//  multiply the mantissa by ten
// ----------------------------------------------------------------------------

void
scfx_rep::multiply_by_ten()
{
    int size = _mant.size() + 1;

    scfx_mant mant8( size );
    scfx_mant mant2( size );

    size --;

    mant8[size] = (_mant[size - 1] >> (bits_in_word - 3));
    mant2[size] = (_mant[size - 1] >> (bits_in_word - 1));

    while( -- size )
    {
	mant8[size] = ( _mant[size] << 3 ) |
	              ( _mant[size - 1] >> ( bits_in_word - 3 ) );
	mant2[size] = ( _mant[size] << 1 ) |
	              ( _mant[size - 1] >> ( bits_in_word - 1 ) );
    }

    mant8[0] = ( _mant[0] << 3 );
    mant2[0] = ( _mant[0] << 1 );

    add_mants( _mant.size(), _mant, mant8, mant2 );

#if 0
    for( int i = size() - 1; i > 0; i -- )
    {
	_mant[i] = ( _mant[i] << 3 ) | ( _mant[i-1] >> ( bits_in_word - 3 ) )
	         + ( _mant[i] << 1 ) | ( _mant[i-1] >> ( bits_in_word - 1 ) );
    }
    _mant[0] = ( _mant[0] << 3 ) + ( _mant[0] << 1 );
#endif
}


// ----------------------------------------------------------------------------
//  normalize
// ----------------------------------------------------------------------------

void
scfx_rep::normalize( int exponent )
{
    int shift = exponent % bits_in_word;
    if( shift < 0 )
    {
	shift += bits_in_word;
    }

    if( shift )
    {
	shift_left( shift );
    }

    find_sw();

    _wp = (shift - exponent) / bits_in_word;
}


// ----------------------------------------------------------------------------
//  return a new mantissa that is aligned and resized
// ----------------------------------------------------------------------------

scfx_mant*
scfx_rep::resize( int new_size, int new_wp ) const
{
    scfx_mant *result = new scfx_mant( new_size );

    result->clear();

    int shift = new_wp - _wp;

    for( int j = _lsw; j <= _msw; j ++ )
    {
	(*result)[j+shift] = _mant[j];
    }

    return result;
}


// ----------------------------------------------------------------------------
//  set a single bit
// ----------------------------------------------------------------------------

void
scfx_rep::set_bin( int i )
{
    _mant[i >> 5] |= 1 << ( i & 31 );
}


// ----------------------------------------------------------------------------
//  set three bits
// ----------------------------------------------------------------------------

void
scfx_rep::set_oct( int i, int n )
{
    if( n & 1 )
    {
	_mant[i >> 5] |= 1 << ( i & 31 );
    }
    i ++;
    if( n & 2 )
    {
	_mant[i >> 5] |= 1 << ( i & 31 );
    }
    i ++;
    if( n & 4 )
    {
	_mant[i >> 5] |= 1 << ( i & 31 );
    }
}


// ----------------------------------------------------------------------------
//  set four bits
// ----------------------------------------------------------------------------

void
scfx_rep::set_hex( int i, int n )
{
    if( n & 1 )
    {
	_mant[i >> 5] |= 1 << ( i & 31 );
    }
    i ++;
    if( n & 2 )
    {
	_mant[i >> 5] |= 1 << ( i & 31 );
    }
    i ++;
    if( n & 4 )
    {
	_mant[i >> 5] |= 1 << ( i & 31 );
    }
    i ++;
    if( n & 8 )
    {
	_mant[i >> 5] |= 1 << ( i & 31 );
    }
}


// ----------------------------------------------------------------------------
//  PRIVATE METHOD : shift_left
//
//  Shifts a scfx_rep to the left by a MAXIMUM of bits_in_word - 1 bits.
// ----------------------------------------------------------------------------

void
scfx_rep::shift_left( int n )
{
    if( n != 0 )
    {
	int shift_left  = n;
	int shift_right = bits_in_word - n;

	_SC_ASSERT( !(_mant[size()-1] >> shift_right), "shift_left overflow" );

	for( int i = size() - 1; i > 0; i -- )
	{
	    _mant[i] = ( _mant[i] << shift_left ) |
		       ( _mant[i-1] >> shift_right );
	}
	_mant[0] <<= shift_left;
    }
}


// ----------------------------------------------------------------------------
//  PRIVATE METHOD : shift_right
//
//  Shifts a scfx_rep to the right by a MAXIMUM of bits_in_word - 1 bits.
// ----------------------------------------------------------------------------

void
scfx_rep::shift_right( int n )
{
    if( n != 0 )
    {
	int shift_left  = bits_in_word - n;
	int shift_right = n;

	_SC_ASSERT( !(_mant[0] << shift_left), "shift_right overflow" );

	for( int i = 0; i < size() - 1; i ++ )
	{
	    _mant[i] = ( _mant[i] >> shift_right ) |
		       ( _mant[i+1] << shift_left );
	}
	_mant[size()-1] >>= shift_right;
    }
}


// ----------------------------------------------------------------------------
//  METHOD : get_bit
//
//  Tests a bit, in two's complement.
// ----------------------------------------------------------------------------

bool
scfx_rep::get_bit( int i ) const
{
    if( ! is_normal() )
	return false;

    scfx_index x = calc_indices( i );

    if( x.wi() >= size() )
	return is_neg();

    if( x.wi() < 0 )
	return false;

    const_cast<scfx_rep*>( this )->toggle_tc();

    bool result = ( _mant[x.wi()] & ( 1 << x.bi() ) ) != 0;

    const_cast<scfx_rep*>( this )->toggle_tc();

    return result;
}


// ----------------------------------------------------------------------------
//  METHOD : set
//
//  Sets a bit, in two's complement, between iwl-1 and -fwl.
// ----------------------------------------------------------------------------

bool
scfx_rep::set( int i, const scfx_params& params )
{
    if( ! is_normal() )
	return false;

    scfx_index x = calc_indices( i );

    if( x.wi() >= size() )
    {
	if( is_neg() )
	    return true;
	else
	    resize_to( x.wi() + 1, 1 );
    }
    else if( x.wi() < 0 )
    {
	resize_to( size() - x.wi(), -1 );
	x.wi( 0 );
    }

    toggle_tc();

    _mant[x.wi()] |= 1 << x.bi();

    if( params.enc() == SC_TC && i == params.iwl() - 1 )
	_sign = -1;

    toggle_tc();

    return true;
}


// ----------------------------------------------------------------------------
//  METHOD : clear
//
//  Clears a bit, in two's complement, between iwl-1 and -fwl.
// ----------------------------------------------------------------------------

bool
scfx_rep::clear( int i, const scfx_params& params )
{
    if( ! is_normal() )
	return false;

    scfx_index x = calc_indices( i );

    if( x.wi() >= size() )
    {
	if( ! is_neg() )
	    return true;
	else
	    resize_to( x.wi() + 1, 1 );
    }
    else if( x.wi() < 0 )
	return true;

    toggle_tc();

    _mant[x.wi()] &= ~( 1 << x.bi() );

    if( params.enc() == SC_TC && i == params.iwl() - 1 )
	_sign = 1;

    toggle_tc();

    return true;
}


// ----------------------------------------------------------------------------
//  METHOD : get_slice
// ----------------------------------------------------------------------------

bool
scfx_rep::get_slice( int i, int j, const scfx_params&,
		     sc_bv_base& bv ) const
{
    if( is_nan() || is_inf() )
	return false;

    // get the bits

    int l = j;
    for( int k = 0; k < static_cast<int>( bv.length() ); ++ k )
    {
	bv[k] = get_bit( l );

	if( i >= j )
	    ++ l;
	else
	    -- l;
    }

    return true;
}

bool
scfx_rep::set_slice( int i, int j, const scfx_params& params,
		     const sc_bv_base& bv )
{
    if( is_nan() || is_inf() )
        return false;

    // set the bits

    int l = j;
    for( int k = 0; k < static_cast<int>( bv.length() ); ++ k )
    {
	if( bv[k] )
	    set( l, params );
	else
	    clear( l, params );

	if( i >= j )
	    ++ l;
	else
	    -- l;
    }

    return true;
}


// ----------------------------------------------------------------------------
//  METHOD : print
// ----------------------------------------------------------------------------

void
scfx_rep::print( ostream& os ) const
{
    os << to_string( SC_DEC, SC_E );
}


// ----------------------------------------------------------------------------
//  METHOD : dump
// ----------------------------------------------------------------------------

void
scfx_rep::dump( ostream& os ) const
{
    os << "scfx_rep" << endl;
    os << "(" << endl;

    os << "mant  =" << endl;
    for( int i = size() - 1; i >= 0; i -- )
    {
	char buf[BUFSIZ];
	sprintf( buf, " %d: %10u (%8x)", i, (int) _mant[i], (int) _mant[i] );
	os << buf << endl;
    }

    os << "wp    = " << _wp << endl;
    os << "sign  = " << _sign << endl;

    os << "state = ";
    switch( _state )
    {
        case normal:
	    os << "normal";
	    break;
        case infinity:
	    os << "infinity";
	    break;
        case not_a_number:
	    os << "not_a_number";
	    break;
        default:
	    os << "unknown";
    }
    os << endl;

    os << "msw   = " << _msw << endl;
    os << "lsw   = " << _lsw << endl;

    os << ")" << endl;
}


// ----------------------------------------------------------------------------
//  METHOD : get_type
// ----------------------------------------------------------------------------

void
scfx_rep::get_type( int& wl, int& iwl, sc_enc& enc ) const
{
    if( is_nan() || is_inf() )
    {
        wl  = 0;
        iwl = 0;
        enc = SC_TC;
        return;
    }

    if( is_zero() )
    {
        wl  = 1;
        iwl = 1;
        enc = SC_US;
        return;
    }

    int msb = ( _msw - _wp ) * bits_in_word
            + scfx_find_msb( _mant[ _msw ] ) + 1;
    while( get_bit( msb ) == get_bit( msb - 1 ) )
    {
        -- msb;
    }

    int lsb = ( _lsw - _wp ) * bits_in_word
            + scfx_find_lsb( _mant[ _lsw ] );

    if( is_neg() )
    {
        wl  = msb - lsb + 1;
        iwl = msb + 1;
        enc = SC_TC;
    }
    else
    {
        wl  = msb - lsb;
        iwl = msb;
        enc = SC_US;
    }
}


// ----------------------------------------------------------------------------
//  PRIVATE METHOD : round
//
//  Performs convergent rounding (rounding to even) as in floating-point.
// ----------------------------------------------------------------------------

void
scfx_rep::round( int wl )
{
    // check for special cases
    
    if( is_nan() || is_inf() || is_zero() )
	return;

    // estimate effective wordlength and compare

    int wl_effective;

    wl_effective = ( _msw - _lsw + 1 ) * bits_in_word;
    if( wl_effective <= (int) wl )
	return;

    // calculate effective wordlength and compare

    int msb = scfx_find_msb( _mant[_msw] );
    int lsb = scfx_find_lsb( _mant[_lsw] );

    wl_effective = ( _msw * bits_in_word + msb ) -
	           ( _lsw * bits_in_word + lsb ) + 1;
    if( wl_effective <= (int) wl )
	return;

    // perform rounding

    int wi = _msw - ( wl - 1 ) / bits_in_word;
    int bi =  msb - ( wl - 1 ) % bits_in_word;
    if( bi < 0 )
    {
	-- wi;
	bi += bits_in_word;
    }

    scfx_index x( wi, bi );

    if( q_bit( x ) && ! q_zero( x ) ||
	q_bit( x ) && q_zero( x ) && q_odd( x ) )
	q_incr( x );
    q_clear( x );

    find_sw();

    _r_flag = true;
}

#if defined(__BCPLUSPLUS__)
#pragma warn +ccc
#endif

// Taf!
