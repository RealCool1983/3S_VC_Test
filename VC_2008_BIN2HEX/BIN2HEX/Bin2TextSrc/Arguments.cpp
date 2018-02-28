// Arguments.cpp: implementation of the Arguments class.
//
// (C) 2001 NOVACOM GmbH, Berlin, www.novacom.net
// Author: Patrick Hoffmann, 03/21/2001
// http://www.codeproject.com/Articles/1242/Managing-argc-and-argv-with-STL-in-a-simple-way
//////////////////////////////////////////////////////////////////////

// updated December 2015 by PJ Arends
// - fixed UNICODE compliance issues
// - fixed 'Help' option issue
//   - Added AddHelpOption() function
//   - Added m_chHelpOption variable

#include "stdafx.h"
#include "Arguments.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

Arguments::Option	Arguments::Option::Empty(_T('\0'));
std::tstring		Arguments::UnknownArgument(_T("<UnKnOwN>"));

Arguments::Arguments(std::tstring strCommandName, std::tstring strDescription, std::tstring strOptionmarkers)
: m_strCommandName( strCommandName ) 
, m_strDescription( strDescription )
, m_strOptionmarkers( strOptionmarkers )
, m_chHelpOption( NULL )
{
}

Arguments::~Arguments()
{
}

bool Arguments::Parse(int argc, TCHAR *argv[])
{
	if( m_strCommandName.empty() )
		m_strCommandName = argv[0];

	unsigned int nArg = 0;

	for( int i=1; i<argc; i++ )
	{
		std::tstring strArgument = argv[i];

		// Option...?
		if( m_strOptionmarkers.find(strArgument.substr(0,1)) != std::tstring::npos )
		{
			TCHAR chOptionName = strArgument[1];

			OptionMap::iterator it = m_mOptions.find(chOptionName);

			if( it == m_mOptions.end() )
			{
                std::tcerr << m_strCommandName << _T(" error: Unknown option ") << strArgument << _T(".") << std::endl;
				Usage();
				return false;
			}
			else
			{
				it->second.m_bSet = true;

				i++;
				{ 
					size_t nNonOptionalArgs = 0;
					
					{
						for( ArgVector::iterator itOptArg = it->second.m_vArguments.begin(); itOptArg != it->second.m_vArguments.end(); itOptArg++ ) 
						{
							if( !itOptArg->m_bOptional )
								nNonOptionalArgs++;
						}
					}
					
					for(size_t nOptArg=0; nOptArg < it->second.m_vArguments.size(); i++, nOptArg++ )
					{
						if( i >= argc || m_strOptionmarkers.find(tstring(argv[i]).substr(0,1)) != tstring::npos )
						{
							if( nOptArg < nNonOptionalArgs )
							{
								std::tcerr << m_strCommandName << _T(" error: Too few arguments for option ") << strArgument << _T(".") << std::endl;
								Usage();
								return false;
							}
							else
							{
								break;
							}
						}
						
						it->second.m_vArguments[nOptArg].m_strValue = argv[i];
					}
				}
				i--;
			}
		}
		else	// ...oder Argument
		{
			if( nArg >= m_vArguments.size() )
			{
				std::tcerr << m_strCommandName << _T(" error: Too much arguments. ") << std::endl;
				Usage();
				return false;
			}

			m_vArguments[nArg++].m_strValue = strArgument;
		}
	}

    if (NULL != m_chHelpOption && operator[](m_chHelpOption))
    {
        // Help option specified, show help screen and exit
        std::tcerr << m_strCommandName << _T(" Help Screen. ") << std::endl;
        Usage();
        return false;
    }

	{
		size_t nNonOptionalArgs = 0;
	
		{
			for( ArgVector::iterator it = m_vArguments.begin(); it != m_vArguments.end(); it++ ) 
			{
				if( !it->m_bOptional )
					nNonOptionalArgs++;
			}
		}
		
		if( nNonOptionalArgs > nArg )
		{
			std::tcerr << m_strCommandName << _T(" error: Too few arguments.") << std::endl;
			Usage();
			return false;
		}
	}
	
	return true;
}

bool Arguments::AddHelpOption(TCHAR chHelpOption, tstring strDescription)
{
    if (NULL != m_chHelpOption)
    {
        return false;
    }

    m_chHelpOption = chHelpOption;
    return AddOption(chHelpOption, strDescription);
}

bool Arguments::AddOption(TCHAR chOption, tstring strDescription)
{
	m_mOptions.insert( pair<TCHAR,Option>(chOption,Option(chOption,strDescription)) );

	return true;
}

bool Arguments::AddOption( Option &option )
{
	m_mOptions.insert( pair<TCHAR,Option>(option.m_chName,option) );
	
	return true;
}

bool Arguments::Usage()
{
	std::tcerr << "Usage: " << m_strCommandName;
	OptionMap::iterator it;
    ArgVector::iterator itArg;

	for(it = m_mOptions.begin(); it != m_mOptions.end(); it++ )
	{
		std::tcerr << " [" << m_strOptionmarkers[0] << it->second.GetName();
		
		for( ArgVector::iterator itArg = it->second.m_vArguments.begin(); itArg != it->second.m_vArguments.end(); itArg++ )
		{
			if( itArg->m_bOptional )
				std::tcerr << " [" << itArg->m_strName << "]";
			else
				std::tcerr << " " << itArg->m_strName;
		}
		std::tcerr << "]";
	}

	for(itArg = m_vArguments.begin(); itArg != m_vArguments.end(); itArg++ )
	{
		if( itArg->m_bOptional )
			std::tcerr << " [" << itArg->m_strName << "]";
		else
			std::tcerr << " " << itArg->m_strName;
	}
	
	std::tcerr << endl;

	if( !m_mOptions.empty() )
		std::tcerr << endl << _T("Options:") << std::endl;
	
	for( it = m_mOptions.begin(); it != m_mOptions.end(); it++ )
	{
		std::tcerr << _T("\t-") << it->second.GetName() << _T("\t  ") << it->second.m_strDescription << endl;
		
		for( ArgVector::iterator itArg = it->second.m_vArguments.begin(); itArg != it->second.m_vArguments.end(); itArg++ )
		{
			std::tcerr << _T("\t ") << itArg->m_strName << _T("\t= ") << itArg->m_strDescription << std::endl;

			if( itArg->m_bOptional )
				std::tcerr << _T("\t\t  optional argument (default='") << itArg->m_strDefault << _T("')") << std::endl;
		}
	}
	
	if( !m_vArguments.empty() )
		std::tcerr << endl << _T("Arguments:") << std::endl;

	for( itArg = m_vArguments.begin(); itArg != m_vArguments.end(); itArg++ )
	{
		std::tcerr << _T("\t") << itArg->m_strName << _T("\t= ") << itArg->m_strDescription << std::endl;

		if( itArg->m_bOptional )
			std::tcerr << _T("\t\t  optional argument (default='") << itArg->m_strDefault << _T("')") << std::endl;
	}
	
	std::tcerr << std::endl;
	
	std::tcerr << m_strDescription << std::endl;

	return true;
}

Arguments::Option::Option( TCHAR chName, std::tstring strDescription )
: m_chName( chName )
, m_strDescription( strDescription )
, m_bSet( false )
{
}

bool Arguments::AddArgument(std::tstring strName, std::tstring strDescription, std::tstring strDefault )
{
	m_vArguments.push_back( Argument( strName, strDescription, strDefault ) );
	return true;
}

bool Arguments::Option::AddArgument(std::tstring strName, std::tstring strDescription, std::tstring strDefault )
{
	m_vArguments.push_back( Argument( strName, strDescription, strDefault ) );
	return true;
}

Arguments::Argument::Argument( std::tstring strName, std::tstring strDescription, std::tstring strDefault )
: m_strName( strName )
, m_strDescription( strDescription )
, m_strValue( strDefault )
, m_strDefault( strDefault )
, m_bOptional( !strDefault.empty() )
{

}

bool Arguments::IsOption(TCHAR chOptionName)
{
	OptionMap::iterator it = m_mOptions.find(chOptionName);
	
	if( it == m_mOptions.end() )
		return false;
	else 
		return it->second.m_bSet;
}

Arguments::Option::operator bool()
{
	return m_bSet;
}

void Arguments::Option::Set( bool bSet )
{
	m_bSet = bSet;
}

tstring &Arguments::operator[]( int n )
{
	return m_vArguments[n].m_strValue;
}

tstring &Arguments::operator[]( std::tstring strArgumentName )
{
	for( ArgVector::iterator it = m_vArguments.begin(); it != m_vArguments.end(); it++ ) 
	{
		if( it->m_strName == strArgumentName )
			return it->m_strValue;
	}

	return UnknownArgument;
}

tstring &Arguments::Option::operator[]( int n )
{
	return m_vArguments[n].m_strValue;
}

tstring &Arguments::Option::operator[]( const TCHAR *pszArgumentName )
{
	return operator[]( (std::tstring)pszArgumentName );
}

tstring &Arguments::Option::operator[]( std::tstring strArgumentName )
{
	for( ArgVector::iterator it = m_vArguments.begin(); it != m_vArguments.end(); it++ ) 
	{
		if( it->m_strName == strArgumentName )
			return it->m_strValue;
	}

	return UnknownArgument;
}

Arguments::Option &Arguments::operator[]( TCHAR chOptionName )
{
	OptionMap::iterator it = m_mOptions.find(chOptionName);
	
	if( it == m_mOptions.end() )
		return Option::Empty;
	else 
		return it->second;
}

tstring Arguments::Option::GetName()
{
	std::tstring str = _T(" ");

	str[0] = m_chName;

	return str;
}
