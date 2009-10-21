/*
 * $Log$
 * Revision 1.4  2009/08/26 09:27:58  wamas
 * GCC 4 Port
 *
 * Revision 1.3  2009/02/02 10:58:01  wamas
 * WIN32 portage
 *
 * Revision 1.2  2009/01/26 12:31:46  wamas
 * Tour Fertigbuchen f�r SAP Tests.
 *
 * Revision 1.1  2009/01/16 13:12:55  wamas
 * Added Backtrace Helping Module
 *
 * Revision 1.3  2007/02/09 19:37:32  wamas
 * bugfix
 *
 * Revision 1.2  2007/02/09 17:25:35  wamas
 * GCC 2.95 port
 *
 * Revision 1.1.1.1  2006/03/17 19:49:17  wamas
 * own tools reponsitority
 *
 * Revision 1.2  2006/03/09 00:48:26  wamas
 * Added CVS Log Info
 *
 */

#ifndef TOOLS_BACKTRACE_H
#define TOOLS_BACKTRACE_H

#include <list>
#include <string>

#include <ostream>

#include "format.h"

namespace Tools {

class BackTrace
{
private:
    struct Ticket
    {
	int ticket;
	std::string message;
	std::string info;

	Ticket( int t, const std::string &m ) : ticket( t ), message( m ), info("") {}
    };

    std::list<Ticket> stack;
    typedef std::list<Ticket>::iterator it_t;
    typedef std::list<Ticket>::reverse_iterator rit_t;
    int count;
    
public:
    BackTrace() : stack(std::list<Ticket>()), count(0) {}


    int push( const std::string & message )
	{
	    count++;
	    stack.push_back( Ticket( count, message ) );
	    return count;
	}
    
    void pop( int ticket )
	{
	    if( stack.empty() )
		return;

	    it_t it = stack.end();
	    it--;

	    while( it != stack.end() )
	    {
		if( it->ticket == ticket )
		{
		    stack.erase( it );
		    break;
		} else {		   
		    stack.erase( it );
		    it = stack.end();

		    if( stack.empty() )
			return;

		    it--;
		}
	    }
	}

    void bt( std::ostream &out )
	{
	    for( it_t it = stack.begin(); it != stack.end(); it++ )
	    {
		out << it->ticket << ':' << it->message << ": " << it->info << '\n';
	    }
	}

    std::string bt( std::string prefix = " Backtrace: " )
	{
	    std::stringstream out;
	    bt( out );

		if( !out.str().empty() )
		  return prefix + out.str();	   
		else
		  return std::string();
	}

    void add_info( int ticket, const std::string &info )
	{
	    if( stack.empty() )
		return;

	    for( rit_t it = stack.rbegin(); it != stack.rend(); it++ )
	    {
		if( it->ticket == ticket )
		{
		    it->info += info;
		    return;
		}
	    }
	}
};

class BackTraceHelper
{
private:
    int ticket;

public:
    static BackTrace bt;

public:
    BackTraceHelper( const std::string &func_name )
	  : ticket(bt.push( func_name ))
	{

	}

    ~BackTraceHelper()
	{
	    bt.pop( ticket );
	}

    void add_info( const std::string &info )
	{
	    bt.add_info( ticket, info );
	}
};

#ifndef NDEBUG
#define BT \
  BackTraceHelper bt( format( "%s:%d:%s", __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )
#else
#define BT
#endif

}

#endif

