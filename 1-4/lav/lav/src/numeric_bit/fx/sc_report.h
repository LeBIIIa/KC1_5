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

    sc_report.h - 

    Original Author: Martin Janssen. Synopsys, Inc. (mjanssen@synopsys.com)

******************************************************************************/

/******************************************************************************

    MODIFICATION LOG - modifiers, enter your name, affliation and
    changes you are making here:

    Modifier Name & Affiliation:
    Description of Modification:
    

******************************************************************************/


#ifndef SC_REPORT_H
#define SC_REPORT_H


// ----------------------------------------------------------------------------
//  ENUM : sc_severity
//
//  Enumeration of report severities.
// ----------------------------------------------------------------------------

enum sc_severity
{
    SC_INFO,
    SC_WARNING,
    SC_ERROR,
    SC_FATAL
};


// ----------------------------------------------------------------------------
//  CLASS : sc_report
//
//  Report class.
// ----------------------------------------------------------------------------

// ADD USAGE MODEL HERE (INCLUDING EXAMPLE)

class sc_report
{
    
public:

    static void    info( const char* /* report id */,
			 const char* /* additional message */ = 0 );

    static void warning( const char* /* report id */,
			 const char* /* additional message */ = 0 );

    static void   error( const char* /* report id */,
			 const char* /* additional message */ = 0 );

    static void   fatal( const char* /* report id */,
			 const char* /* additional message */ = 0 );

    static void  report( sc_severity /* report severity */,
			 const char* /* report id */,
			 const char* /* additional message */ = 0 );

    static void register_id( const char* /* report id */,
			     const char* /* report message */ );

    static const char* message( const char* /* report id */ );

private:

    sc_report() {}

};


// ----------------------------------------------------------------------------
//  CLASS : sc_report_handler_base
//
//  Report handler pure virtual base class.
// ----------------------------------------------------------------------------

// ADD USAGE MODEL HERE

class sc_report_handler_base
{

    friend class sc_report;

protected:

    sc_report_handler_base();
    virtual ~sc_report_handler_base();

    virtual void report( sc_severity /* report severity */,
			 const char* /* report id */,
			 const char* /* additional message */ ) const = 0;

    void install_();
    void deinstall_();

private:

    const sc_report_handler_base* _old_handler;

};


// ----------------------------------------------------------------------------
//  FUNCTION : sc_stop_here
//
//  Debugging aid for warning, error, and fatal reports.
// ----------------------------------------------------------------------------

void
sc_stop_here();


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

// ----------------------------------------------------------------------------
//  CLASS : sc_report
//
//  Report class.
// ----------------------------------------------------------------------------

inline
void
sc_report::info( const char* id, const char* add_msg )
{
    report( SC_INFO, id, add_msg );
}

inline
void
sc_report::warning( const char* id, const char* add_msg )
{
    report( SC_WARNING, id, add_msg );
}

inline
void
sc_report::error( const char* id, const char* add_msg )
{
    report( SC_ERROR, id, add_msg );
}

inline
void
sc_report::fatal( const char* id, const char* add_msg )
{
    report( SC_FATAL, id, add_msg );
}


#endif

// Taf!
