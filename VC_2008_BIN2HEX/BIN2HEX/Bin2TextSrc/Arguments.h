// Arguments.h: interface for the Arguments class.
//
// (C) 2001 NOVACOM GmbH, Berlin, www.novacom.net
// Author: Patrick Hoffmann, 03/21/2001
// http://www.codeproject.com/Articles/1242/Managing-argc-and-argv-with-STL-in-a-simple-way
//////////////////////////////////////////////////////////////////////

#pragma warning (disable : 4786) 

#if !defined(AFX_ARGUMENTS_H__1F0E328D_C72D_4709_8A74_7654888217A6__INCLUDED_)
#define AFX_ARGUMENTS_H__1F0E328D_C72D_4709_8A74_7654888217A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <map>
#include "tstl.h"

//using namespace std;

inline std::tostream &operator<<(std::tostream& ostr, std::tstring& tstr )
{
	ostr << tstr.c_str();
	return ostr;
}

class Arguments  
{
	std::tstring	m_strOptionmarkers;
	std::tstring	m_strDescription;
	std::tstring	m_strCommandName;

public:
	static std::tstring	UnknownArgument;

	class Option;

	class Argument
	{
		friend class Arguments;
		friend class Option;
		
		std::tstring	m_strName;
		std::tstring	m_strDescription;
		std::tstring	m_strValue;
		std::tstring	m_strDefault;
		bool	m_bOptional;

	public:
		Argument(std::tstring strName, std::tstring strDescription=_T(""), std::tstring strDefault=_T("") );
	};

	typedef std::vector<Argument>	ArgVector;

	class Option
	{
		friend class Arguments;
		static Option	Empty;
		
		TCHAR			m_chName;
		ArgVector		m_vArguments;
		std::tstring    m_strDescription;
		bool			m_bSet;
		
	public:
		Option( TCHAR chName, std::tstring strDescription=_T("") );
		bool AddArgument(std::tstring strName, std::tstring strDescription=_T(""), std::tstring strDefault = _T("") );
		std::tstring &operator[]( int n );
		std::tstring &operator[]( std::tstring strArgumentName );
		std::tstring &operator[]( const TCHAR *pszArgumentName );
		operator bool();
		void Set( bool bSet = true );
		std::tstring GetName();
	};

private:
	typedef std::map<TCHAR,Option,std::less<TCHAR>,std::allocator<Option> > OptionMap;
	
	OptionMap			m_mOptions;
	ArgVector			m_vArguments;

    TCHAR m_chHelpOption;

public:
	bool IsOption( TCHAR chOptionName );
	bool Usage();
	bool AddOption( TCHAR chOptionName, std::tstring strDescription=_T("") );
	bool AddHelpOption( TCHAR chOptionName, std::tstring strDescription=_T("") );
	bool AddOption( Option &option );
	bool AddArgument(std::tstring strName, std::tstring strDescription=_T(""), std::tstring strDefault = _T("") );
	bool Parse(int argc, TCHAR* argv[]);
	bool Parse(TCHAR *pszCommandLine);
	bool Parse();
	
	std::tstring &operator[]( int n );
	std::tstring &operator[]( std::tstring strArgumentName );
	Option &operator[]( TCHAR chOptionName );
	
	Arguments(std::tstring strCommandName, std::tstring strDescription=_T(""), std::tstring strOptionmarkers=_T("-/") );
	virtual ~Arguments();
};

#pragma warning (disable : 4786) 

#endif // !defined(AFX_ARGUMENTS_H__1F0E328D_C72D_4709_8A74_7654888217A6__INCLUDED_)
