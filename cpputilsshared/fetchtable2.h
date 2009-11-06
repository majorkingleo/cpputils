/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2009 Salomon Automation GmbH
 */

#ifndef _Tools_FETCHTABLE2_H
#define _Tools_FETCHTABLE2_H

#include <map>
#include <report_exception.h>

#ifndef NOWAMAS
# include <dbsql.h>
# include <hist_util.h>
#endif

#include "jvector.h"

namespace Tools {

#ifndef NOWAMAS

#ifdef FIX_BUG_20809
#  define WORKAROUND_BUG_20809_TExecSqlX _TExecSqlX
#  define WORKAROUND_BUG_20809_TExecSqlV _TExecSqlV
#else
#  define WORKAROUND_BUG_20809_TExecSqlX TExecSqlX
#  define WORKAROUND_BUG_20809_TExecSqlV TExecSqlV
#endif

class FetchTableBase
{
 protected:
  void *tid;
  SqlContext *sc;
  std::string where;
  const int BLOCKSIZE;
  bool failed;
  std::string sql;

 protected:
  FetchTableBase()
	: 
	tid(0),
	sc(0),
	where(""),
	BLOCKSIZE(100),
	failed(false),
	sql("")
	{

	}

  FetchTableBase( void *tid_, SqlContext *sc_, const std::string where_ , const unsigned int BLOCKSIZE_ )
	:
	tid(tid_), 
	sc(sc_), 
	where( where_ ),
	BLOCKSIZE( BLOCKSIZE_ ),
    failed(false),
	sql( "" )
	{}

  FetchTableBase( const FetchTableBase & table )
	:
	tid(table.tid),
	sc(table.sc),
	where(table.where),
	BLOCKSIZE(table.BLOCKSIZE),
	failed(table.failed),
	sql(table.sql)
	{}


  void check_bug_20809()
	{
	  if( std::string( __FILE__ ).find(":") != std::string::npos &&
		  std::string( TSqlErrTxt(tid) ).find( "too few bind variables" ) != std::string::npos )  
		{
		  throw REPORT_EXCEPTION( Tools::format("WARNING YOU TRIGGERT BUG http://bugzilla.salomon.at/show_bug.cgi?id=20809\n"
												"Possible Workaround: recompiling your file with defined FIX_BUG_20809 before\n"
												"including this '" __FILE__ "' Header file.\n" 
												"SqlError: %d SqlError is: %s", TSqlError, TSqlErrTxt(tid) )
								  );
		}
	}

  FetchTableBase & operator=( const FetchTableBase & table )
	{
	  tid = table.tid;
	  sc = table.sc;
	  where = table.where;
	  failed = table.failed;
	  sql = table.sql;

	  return *this;
	}

public:
    bool operator!() const { return failed; }

	const std::string & get_sql() const { return sql; }

};

template <class Table> class FetchTable : public JVector<Table>, public FetchTableBase
{
 protected:
  std::string table_name;

 private:
  FetchTable() : FetchTableBase(), table_name("") {};

 public:
  FetchTable(const FetchTable<Table> & table )
	: JVector<Table>(table.BLOCKSIZE),
	FetchTableBase( table ),
	table_name(table.table_name)
	{
		insert( table.begin(), table.end() );
	}

  FetchTable & operator=( const FetchTable<Table> & table )
	{
	  FetchTableBase::operator=( table );

	  JVector<Table>::clear();
	  JVector<Table>::insert( JVector<Table>::end(), table.begin(), table.end() );	
	  table_name = table.table_name;	  

	  return *this;
	}

 public:
  FetchTable( void *tid_, SqlContext *sc_, const std::string & table_name_, const std::string where_ = "", const unsigned int BLOCKSIZE_ = 100 )
    : JVector<Table>(BLOCKSIZE_),
	FetchTableBase( tid_, sc_, where_, BLOCKSIZE_ ),
	table_name( table_name_ )
	{
		sql = Tools::format( "select %%%s from %s %s", table_name, table_name, where );
		exec();
      }

 protected:  

  void exec()
    {
      // fprintf( stderr, "exec()===========\n" );
      JVector<Table> vec(BLOCKSIZE);
     
	  JVector<Table>::resize(0);
	  JVector<Table>::reserve(BLOCKSIZE);
 
      int rv = 0;	  
	  
      do
		{
		  rv = ( rv == 0 ) ?  WORKAROUND_BUG_20809_TExecSqlX( tid, sc, 
										 const_cast<char*>(sql.c_str()),
										 BLOCKSIZE, 0,
										 SELSTRUCT( const_cast<char*>(table_name.c_str()), vec[0] ),
										 NULL )
			: WORKAROUND_BUG_20809_TExecSqlV( tid, sc, NULL, NULL, NULL, NULL );
		  
		  
		  if( rv < BLOCKSIZE  && TSqlError(tid) != SqlNotFound )	    
			{
			  failed = true;

			  check_bug_20809();
			  return;
			}
		  
			if( rv > 0 )
			   insert( JVector<Table>::end(), vec.begin(), vec.begin() + rv );

		} while( rv == BLOCKSIZE );      
    }

 public:
    Table & operator[]( unsigned int i ) { return JVector<Table>::operator[](i); }
};


template <class TableA, class TableB> class FetchTablePair : public JVector<std::pair<TableA,TableB> >, public FetchTableBase
{
 protected:
  std::string table1_name;
  std::string table2_name;

 private:
  FetchTablePair() : FetchTableBase(), table1_name(""), table2_name("") {};

 public:
  FetchTablePair(const FetchTablePair<TableA,TableB> & table )
	: JVector<std::pair<TableA,TableB> >(),
	FetchTableBase( table ),
	table1_name(table.table1_name),
	table2_name(table.table2_name)
	{
		insert( table.begin(), table.end() );
	}

  FetchTablePair & operator=( const FetchTablePair<TableA, TableB> & table )
	{
	  FetchTableBase::operator=( table );
	  
	  insert( table.begin(), table.end() );	
	  table1_name = table.table1_name;
	  table2_name = table.table2_name;
	}

 public:
  FetchTablePair( void *tid_, 
			  SqlContext *sc_, 
			  const std::string & table1_name_, 
			  const std::string & table2_name_,
			  const std::string & where_, 
			  const unsigned int BLOCKSIZE_ = 100 )
    : JVector<std::pair<TableA,TableB> >(),
	FetchTableBase(tid_, sc_, where_, BLOCKSIZE_ ),
	table1_name( table1_name_ ), 
	table2_name( table2_name_ )
      {
		sql = Tools::format( "select %%%s, %%%s from %s, %s %s", table1_name, table2_name, table1_name, table2_name, where );
		exec();
      }

 protected:  

  void exec()
    {
      // fprintf( stderr, "exec()===========\n" );
      JVector<TableA> vec1(BLOCKSIZE);
      JVector<TableB> vec2(BLOCKSIZE);
     
	  JVector<std::pair<TableA,TableB> >::resize(0);
	  JVector<std::pair<TableA,TableB> >::reserve(BLOCKSIZE);
 
      int rv = 0;
	  
      do
		{
		  rv = ( rv == 0 ) ? WORKAROUND_BUG_20809_TExecSqlX( tid, sc, 
										 const_cast<char*>(sql.c_str()),
										 BLOCKSIZE, 0,
										 SELSTRUCT( const_cast<char*>(table1_name.c_str()), vec1[0] ),
										 SELSTRUCT( const_cast<char*>(table2_name.c_str()), vec2[0] ),
										 NULL )
			:  WORKAROUND_BUG_20809_TExecSqlV( tid, sc, NULL, NULL, NULL, NULL );
		  
		  
		  if( rv < BLOCKSIZE  && TSqlError(tid) != SqlNotFound )	    
			{
			  failed = true;

			  check_bug_20809();
			  return;
			}
		  
			if( rv > 0 )
			  {
				for( int i = 0; i < rv; i++ )
				  {
					JVector<std::pair<TableA,TableB> >::push_back( std::pair<TableA,TableB>( vec1[i], vec2[i] ) );
				  }
			  }

		} while( rv == BLOCKSIZE );      
    }

 public:
    std::pair<TableA,TableB> & operator[]( unsigned int i ) { return JVector<std::pair<TableA,TableB> >::operator[](i); }
    
};

#endif


} // /namespace wamas

#endif  /* _wamas_FETCHTABLE2_H */
