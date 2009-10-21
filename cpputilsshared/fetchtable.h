/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2009 Salomon Automation GmbH
 */

#ifndef _Tools_FETCHTABLE_H
#define _Tools_FETCHTABLE_H

#include <map>

#ifndef NOWAMAS
# include <dbsql.h>
# include <hist_util.h>
#endif

#include "zvector.h"

namespace Tools {

#ifndef NOWAMAS

template <class Table> class FetchTable : public zvector<Table>
{
 protected:
  void *tid;
  SqlContext *sc;
  std::string table_name;
  std::string where;
  const int BLOCKSIZE;

  bool failed;

  std::string sql;

 private:
  FetchTable() : BLOCKSIZE(0) {};

 public:
  FetchTable(const FetchTable<Table> & table )
	: zvector<Table>(table.BLOCKSIZE),
	  tid(table.tid),
	  table_name(table.table_name),
	  where(table.where),
	  BLOCKSIZE(table.BLOCKSIZE),
	  failed(table.failed),
	  sql(table.sql)
	{
		insert( table.begin(), table.end() );
	}

  FetchTable & operator=( const FetchTable<Table> & table )
	{
	    std::vector<Table>::clear();
	    std::vector<Table>::insert( std::vector<Table>::end(), table.begin(), table.end() );	
		tid = table.tid;
		sc = table.sc;
		table_name = table.table_name;
		where = table.where;
		failed = table.failed;
		sql = table.sql;

		return *this;
	}

 public:
  FetchTable( void *tid_, SqlContext *sc_, const std::string & table_name_, const std::string where_ = "", const unsigned int BLOCKSIZE_ = 100 )
    : zvector<Table>(BLOCKSIZE_),
    tid(tid_), 
	sc(sc_), 
	table_name( table_name_ ), 
	where( where_ ), 
	BLOCKSIZE( BLOCKSIZE_ ),
    failed(false),
	sql("")
      {
		sql = Tools::format( "select %%%s from %s %s", table_name, table_name, where );
		exec();
      }

 protected:  

  void exec()
    {
      // fprintf( stderr, "exec()===========\n" );
      zvector<Table> vec(BLOCKSIZE);
     
	  zvector<Table>::resize(0);
	  zvector<Table>::reserve(BLOCKSIZE);
 
      int rv = 0;	  
	  
      do
		{
		  rv = ( rv == 0 ) ?  TExecSqlX( tid, sc, 
										 const_cast<char*>(sql.c_str()),
										 BLOCKSIZE, 0,
										 SELSTRUCT( const_cast<char*>(table_name.c_str()), vec[0] ),
										 NULL )
			: TExecSqlV( tid, sc, NULL, NULL, NULL, NULL );
		  
		  
		  if( rv < BLOCKSIZE  && TSqlError(tid) != SqlNotFound )	    
			{
			  failed = true;
			  return;
			}
		  
			if( rv > 0 )
				insert( std::vector<Table>::end(), vec.begin(), vec.begin() + rv );

		} while( rv == BLOCKSIZE );      
    }

 public:
    Table & operator[]( unsigned int i ) { return std::vector<Table>::operator[](i); }
    
    bool operator!() const { return failed; }

	const std::string & get_sql() const { return sql; }
};


template <class TableA, class TableB> class FetchTablePair : public std::vector<std::pair<TableA,TableB> >
{
 protected:
  void *tid;
  SqlContext *sc;
  std::string table1_name;
  std::string table2_name;
  std::string where;
  const int BLOCKSIZE;

  bool failed;

  std::string sql;

 private:
  FetchTablePair() : BLOCKSIZE(0) {};

 public:
  FetchTablePair(const FetchTablePair<TableA,TableB> & table )
	: std::vector<std::pair<TableA,TableB> >(),
	  tid(table.tid),
	  table1_name(table.table1_name),
	  table2_name(table.table2_name),
	  where(table.where),
	  BLOCKSIZE(table.BLOCKSIZE),
	  failed(table.failed),
	  sql(table.sql)
	{
		insert( table.begin(), table.end() );
	}

  FetchTablePair & operator=( const FetchTablePair<TableA, TableB> & table )
	{
		insert( table.begin(), table.end() );	
		tid = table.tid;
		sc = table.sc;
		table1_name = table.table1_name;
		table2_name = table.table2_name;
		where = table.where;
		failed = table.failed;
		sql = table.sql;
	}

 public:
  FetchTablePair( void *tid_, 
			  SqlContext *sc_, 
			  const std::string & table1_name_, 
			  const std::string & table2_name_,
			  const std::string & where_, 
			  const unsigned int BLOCKSIZE_ = 100 )
    : std::vector<std::pair<TableA,TableB> >(),
    tid(tid_), 
	sc(sc_), 
	table1_name( table1_name_ ), 
	table2_name( table2_name_ ), 
	where( where_ ), 
	BLOCKSIZE( BLOCKSIZE_ ),
    failed(false),
	sql("")
      {
		sql = Tools::format( "select %%%s, %%%s from %s, %s %s", table1_name, table2_name, table1_name, table2_name, where );
		exec();
      }

 protected:  

  void exec()
    {
      // fprintf( stderr, "exec()===========\n" );
      zvector<TableA> vec1(BLOCKSIZE);
      zvector<TableB> vec2(BLOCKSIZE);
     
	  std::vector<std::pair<TableA,TableB> >::resize(0);
	  std::vector<std::pair<TableA,TableB> >::reserve(BLOCKSIZE);
 
      int rv = 0;
	  
      do
		{
		  rv = ( rv == 0 ) ?  TExecSqlX( tid, sc, 
										 const_cast<char*>(sql.c_str()),
										 BLOCKSIZE, 0,
										 SELSTRUCT( const_cast<char*>(table1_name.c_str()), vec1[0] ),
										 SELSTRUCT( const_cast<char*>(table2_name.c_str()), vec2[0] ),
										 NULL )
			: TExecSqlV( tid, sc, NULL, NULL, NULL, NULL );
		  
		  
		  if( rv < BLOCKSIZE  && TSqlError(tid) != SqlNotFound )	    
			{
			  failed = true;
			  return;
			}
		  
			if( rv > 0 )
			  {
				for( int i = 0; i < rv; i++ )
				  {
					std::vector<std::pair<TableA,TableB> >::push_back( std::pair<TableA,TableB>( vec1[i], vec2[i] ) );
				  }
			  }

		} while( rv == BLOCKSIZE );      
    }

 public:
    std::pair<TableA,TableB> & operator[]( unsigned int i ) { return std::vector<std::pair<TableA,TableB> >::operator[](i); }
    
    bool operator!() const { return failed; }

	const std::string & get_sql() const { return sql; }
};

#endif


} // /namespace wamas

#endif  /* _wamas_FETCHTABLE_H */
