#include <iostream>
#include <fstream>
#include <xml.h>
#include <string_utils.h>

using namespace Tools;


std::string gen_decl( int i )
{
  return "Arg" + x2s(i);
}

std::string gen_var( int i )
{
  return "var" + x2s(i);
}

std::string gen_declerations( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += gen_decl(i);
	  res += " ";
	  res += gen_var(i);
	  res += ";\n";
	}
  return res;
}

std::string gen_templatelist( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  if( i > 0 )
		res += ", ";

	  res += gen_decl(i);
	  res += " ";
	  res += gen_var(i) + "_";
	}
  return res;
}

std::string gen_consttemplatelist( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  if( i > 0 )
		res += ", ";

	  res += "const " + gen_decl(i);
	  res += "& ";
	  res += gen_var(i) + "_";
	}
  return res;
}

std::string gen_templateclasses( int maxargs, const std::string cl_str = "class")
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  if( i > 0 )
		res += ", ";
	  res += cl_str + " " + gen_decl(i);		
	}
  return res;
}

std::string gen_templatedecl( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  if( i > 0 )
		res += ", ";
	  res += gen_decl(i);		
	}
  return res;
}


std::string gen_varinit( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  if( i > 0 )
		res += ", ";
	  res += gen_var(i) + "(" + gen_var(i) + "_)";
	}
  return res;
}


std::string gen_getint( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += "case " + x2s(i) + ": return get_int(" + gen_var(i) + ");\n";
	}
  return res;
}

std::string gen_arglist( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += "case " + x2s(i) + ":\n";
	  res += "args[i].is_int = is_int( " + gen_var(i) + ");\n";
	  res += "args[i].is_string = is_string( " + gen_var(i) + ");\n";
	  res += "break;\n";
	}
  return res;
}

std::string gen_usearg( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += "case " + x2s(i) + ": return x2s( " + gen_var(i) + ",cf);\n";
	}
  return res;
}

std::string gen_varlist( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  if( i > 0 )
		res += ", ";

	  res += gen_var(i) + "_";
	}

  return res;
}

std::string gen_format( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += "template<" + gen_templateclasses(i+1) + ">\n";
	  res += "inline std::string format( const std::string & fs," +  gen_templatelist(i+1) + ") {\n";
	  res += "  return Format::Format<" + gen_templatedecl(i+1);

	  for( int j = i+1; j < maxargs; j++ )
		{
		  if( j > 0 )
			{
			  res += ",";
			}
		res += "char";
		}

	  res += ">( fs, " + gen_varlist( i+1 );

	  for( int j = i+1; j < maxargs; j++ )
		{
		  if( j > 0 )
			{
			  res += ",";
			}
		res += "0";
		}
	  
	  res +=", " + x2s(i+1) + ").get_string();\n}\n\n";
	}
  return res;
}

std::string gen_printf( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += "template<" + gen_templateclasses(i+1, "typename") + ">\n";
	  res += "PrintF& operator()( const std::string & fs," +  gen_consttemplatelist(i+1) + ") {\n";
	  res += " if( check() ) out << format( fs," + gen_varlist( i+1 ) + ");\n";
	  res += " return *this;\n}\n";
	}
  return res;
}


std::string gen_logprintf( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += "template<" + gen_templateclasses(i+1, "typename") + ">\n";
	  res += "int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs," +  gen_consttemplatelist(i+1) + ") {\n";
	  res += "  return _LogPrintf( fac.c_str(), level, \"%s\", Tools::format( fs," + gen_varlist( i+1 ) + ").c_str());\n";
	  res += "}\n\n";
	}
  return res;
}

std::string gen_fspdb( int maxargs )
{
  std::string res;

  for( int i = 0; i < maxargs; i++ )
	{
	  res += "template<" + gen_templateclasses(i+1) + ">\n";
	  res += "int _CPP_FS_PDB(  dccclient_stm *scl, int level, const std::string & fs," +  gen_consttemplatelist(i+1) + ") {\n";
	  res += "  return fs_PDB_int( scl, level, \"%s\", Tools::format( fs," + gen_varlist( i+1 ) + ").c_str());\n";
	  res += "}\n\n";
	}
  return res;
}


int main( int argc, char **argv )
{
  if( argc <= 3 ) 
	{
	  std::cerr << "usage: " << argv[0] << " Anzah Inputdatei Outputdatei " << std::endl;
	  return 1;
	}

  int maxargs = s2x<int>( argv[1] );

  std::cerr << "maximale Anzahl der Argumente beträgt: " << maxargs << std::endl;

  std::string file;

  if( !XML::read_file( argv[2], file ) )
	{
	  std::cerr << "kann " << argv[2] << " nicht öffnen" << std::endl;
	  return 2;
	}

  file = substitude( file, "DECLARATIONS", gen_declerations( maxargs ) );
  file = substitude( file, "TEMPLATECLASSES", gen_templateclasses( maxargs ) );
  file = substitude( file, "TEMPLATELIST", gen_templatelist( maxargs ) );
  file = substitude( file, "TEMPLATEDECL", gen_templatedecl( maxargs ) );
  file = substitude( file, "VARINIT", gen_varinit( maxargs ) );
  file = substitude( file, "MAXARGS", x2s(maxargs) );
  file = substitude( file, "GETINT", gen_getint(maxargs) );
  file = substitude( file, "GENARGLIST", gen_arglist(maxargs) );
  file = substitude( file, "GENUSEARG", gen_usearg(maxargs) );
  file = substitude( file, "GENFORMAT", gen_format(maxargs) );
  file = substitude( file, "GENPRINTF", gen_printf(maxargs) );
  file = substitude( file, "GENLOGPRINTF", gen_logprintf(maxargs) );
  file = substitude( file, "GENFSPDB", gen_fspdb(maxargs) );
  
  std::ofstream out( argv[3], std::ios_base::out | std::ios_base::trunc );

  if( !out )
	{
	  std::cerr << "Datei '" << argv[3] << "' kann nicht zum schreiben geoeffnet werden\n";
	}

  out << file << std::endl;

  return 0;
}
