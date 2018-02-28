///////////////////////////////////////////////////////////////////////////////
//
// Builder.h
//
// defines the pja::CBuilder class
// used for positional string formatting
//
// Written by: PJ Arends  April 2012
//
// License: The CodeProject Open License (CPOL) - (http://www.codeproject.com/info/cpol10.aspx)
//
// Usage:
//
// std::tcout << pja::CBuilder<>(_T("{{0} = {0}")) % 23.5;
// std::tcout << pja::CBuilder<>(_T("{{0} = {{{0}}"), 45.6);
// std::tcout << pja::CBuilder< _T('%'), _T('%') > (_T("%%0% = %0%"), 18.3);
//
// Outputs:
//
// {0} = 23.5
// {0} = {45.6}
// %0% = 18.3
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>       // for: std::vector
#include <map>          // for: std::map, std::pair
#include <string>       // for: std::string, std::wstring
#include <ostream>      // for: std::ostream, std::wostream
#include <sstream>      // for: std::ostringstream, std::wostringstream
#include "tstl.h"       // for: std::tstring, std::tostream, std::tostringstream
#include <math.h>       // for: log10()

#ifndef _T
#   ifdef _UNICODE
#       define _T(x) L##x
#   else
#       define _T(x) x
#   endif
#endif

#ifdef _UNICODE
    typedef wchar_t TCHAR;
#else
    typedef char TCHAR;
#endif


namespace pja
{
    template <TCHAR LeftInsertionMarker = _T('{'), TCHAR RightInsertionMarker = _T('}')>
    class CBuilder
    {
        class CParameter
        {
        private:
            std::tstring Text;

        public:
            operator const std::tstring()
            {
                return Text;
            }

            // Will take any class that has a
            // friend std::tostream& operator <<(std::tostream &, const T&)
            // defined
            //
            // example:
            //
            // class Test
            // {
            //     public:
            //         friend std::tostream& operator <<(std::tostream &os, const Test&)
            //         {
            //             os << _T("Hello from Test");
            //             return os;
            //         }
            //     }
            // };

            // Default constructor
            // This constructor is called for all types unless
            // a specialized constructor is supplied.
            template <typename T>
            CParameter(const T& t)
            {
                std::tostringstream ss;
                ss << t;
                Text = ss.str();
            }

            // Specialized constructors

            // boolean
            CParameter(const bool &t)
            {
                std::tostringstream ss;
                ss << std::boolalpha << t;
                Text = ss.str();
            }

            // std::tstring
            CParameter(const std::tstring &t)
            {
                Text = t;
            }

            // TCHAR* pointer
            CParameter(const TCHAR *t)
            {
                Text = t;
            }

#ifdef __CSTRINGT_H__
            //MFC/ATL CString
            CParameter(const CString &t)
            {
                Text = t;
            }
#endif // __CSTRINGT_H__

        }; // class CParameter

    private:
        std::vector<std::tstring> ParameterVector;
        std::tstring UnformattedText;
        std::tstring FormattedText;
        bool IsBuilt;

    protected:

        void Build(void)                              
        {
            FormattedText = UnformattedText;
            const std::vector<std::tstring>::size_type RemoveLeft = ~(static_cast<std::vector<std::tstring>::size_type>(0));

            // replacement map: first is the position in the string; second is the index in the ParameterVector vector, ~0 to erase a character
            std::map<std::tstring::size_type, std::vector<std::tstring>::size_type> PositionMap;

            // loop counters
            std::tstring::size_type position;
            std::vector<std::tstring>::size_type index;

            // first we build our replacement map
            for (index = 0; index < ParameterVector.size(); ++index)
            {
                std::tostringstream ss;
                ss << LeftInsertionMarker << index << RightInsertionMarker;

                for (position = 0; position != std::tstring::npos;)
                {
                    position = UnformattedText.find(ss.str(), position);
                    if (position != std::tstring::npos)
                    {
                        if (position > 1 && UnformattedText[position - 2] == LeftInsertionMarker && UnformattedText[position - 1] == LeftInsertionMarker)
                        {   // {{{x}  ==>  {newtext
                            PositionMap.insert(std::pair<std::tstring::size_type, std::vector<std::tstring>::size_type>(position - 1, RemoveLeft));
                            PositionMap.insert(std::pair<std::tstring::size_type, std::vector<std::tstring>::size_type>(position, index));
                        }
                        else if (position > 0 && UnformattedText[position - 1] == LeftInsertionMarker)
                        {   // {{x}  ==>  {x}
                            PositionMap.insert(std::pair<std::tstring::size_type, std::vector<std::tstring>::size_type>(position, RemoveLeft));
                        }
                        else
                        {   // {x}  ==>  newtext
                            PositionMap.insert(std::pair<std::tstring::size_type, std::vector<std::tstring>::size_type>(position, index));
                        }

                        ++position;
                    }
                }
            }

            // now we do our replacements from back to front
            for (std::map<std::tstring::size_type, std::vector<std::tstring>::size_type>::reverse_iterator mapIterator = PositionMap.rbegin(); mapIterator != PositionMap.rend(); mapIterator++)
            {
                FormattedText.reserve(UnformattedText.length() + PositionMap.size() * 10); // assume an average of 10 characters per insertion
                if (mapIterator->second == RemoveLeft)
                {
                    FormattedText.erase(mapIterator->first, 1);
                }
                else
                {
                    // calculate the number of characters that need to be replaced
                    std::tstring::size_type chars = 3;
                    chars += static_cast<std::tstring::size_type>(log10(static_cast<double>(mapIterator->second)));

                    FormattedText.replace(mapIterator->first, chars, ParameterVector[mapIterator->second]);
                }
            }

            IsBuilt = true;
        }

    public:
    
        // Formatted text extraction 
/*
        operator const TCHAR*()
        {
            // cannot use operator const TCHAR* as it seems to confuse the compiler
            // when a CBuilder object is output to a stream via a << operator.
            // Sometimes our friend std::tostream& operator << is called and sometimes
            // the operator const TCHAR* is called with the output being the address
            // of our formatted string instead of the string itself.
            
            if (!IsBuilt)
            {
                Build();
            }
            return FormattedText.c_str();
        }
*/

        // Retrieve a TCHAR* pointer to the formatted text
        const TCHAR *c_str()
        {
            if (!IsBuilt)
            {
                Build();
            }
            return FormattedText.c_str();
        }

        // std::tstring operator
        // Retrieve a std::tstring containing the formatted text
        operator const std::tstring()
        {
            if (!IsBuilt)
            {
                Build();
            }
            
            return FormattedText;
        }

        // stream output operator
        // outputs the formatted text to the ostream
        friend std::tostream& operator << (std::tostream &os, pja::CBuilder<LeftInsertionMarker, RightInsertionMarker> &Builder)
        {
            if (!Builder.IsBuilt)
            {
                Builder.Build();
            }
            
            os << Builder.FormattedText.c_str();
            return os;
        }

        // parameter adding operator
        // used the modulus operator because that is what is used in boost::format
        template <class T>
        CBuilder<LeftInsertionMarker, RightInsertionMarker>& operator % (const T& Parameter)
        {
            IsBuilt = false;
            ParameterVector.push_back(CParameter(Parameter));
            return *this;
        }

        // parameter clearing function
        // call this function to reset all the parameters
        void ClearParameters(void)
        {
            ParameterVector.clear();
            IsBuilt = false;
        }

        unsigned int GetParameterCount()
        {
            return const_cast<unsigned int>(ParameterVector.size());
        }

        // assignment operator
        // purposely disabled as it should not be used
        // throws an exception if called
        template <class T>
        CBuilder<LeftInsertionMarker, RightInsertionMarker>& operator = (const T&)
        {
            throw "pja::CBuilder operator=(): Not Implemented";
            return *this;
        }

        // constructors
#ifdef _WINDOWS_

    protected:
        void LoadStringFromResource(HMODULE hModule, unsigned long FormatStringID)
        {
            if (hModule == NULL)
            {
                hModule = ::GetModuleHandle(NULL);
            }

            // following code based on CString::LoadString
            int size = 0;
            int len = 256;
            TCHAR *buffer = NULL;

            while (len && len > size - 2)
            {
                size += 256;
                len = 0;
                delete[] buffer;
                buffer = new TCHAR[size];
                if (hModule && FormatStringID && buffer)
                {
                    len = ::LoadString(hModule, FormatStringID, buffer, size);
                }
            }

            if (len)
            {
                UnformattedText = buffer;
            }

            delete[] buffer;
        }

    public:
        // format string that is loaded from a string resource
        CBuilder(HMODULE hModule, unsigned long FormatStringID) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.push_back(p0);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(2);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(3);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2, CParameter p3) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(4);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(5);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(6);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(7);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6, CParameter p7) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(8);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
            ParameterVector.push_back(p7);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6, CParameter p7, CParameter p8) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(9);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
            ParameterVector.push_back(p7);
            ParameterVector.push_back(p8);
        }

        CBuilder(HMODULE hModule, unsigned long FormatStringID, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6, CParameter p7, CParameter p8, CParameter p9) : IsBuilt(false)
        {
            LoadStringFromResource(hModule, FormatStringID)
            ParameterVector.reserve(10);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
            ParameterVector.push_back(p7);
            ParameterVector.push_back(p8);
            ParameterVector.push_back(p9);
        }

#endif //_WINDOWS_

        CBuilder(const std::tstring &Format) : UnformattedText(Format), IsBuilt(false)
        {
        }

        CBuilder(const std::tstring &Format, CParameter p0) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.push_back(p0);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(2);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(3);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2, CParameter p3) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(4);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(5);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(6);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(7);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6, CParameter p7) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(8);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
            ParameterVector.push_back(p7);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6, CParameter p7, CParameter p8) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(9);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
            ParameterVector.push_back(p7);
            ParameterVector.push_back(p8);
        }

        CBuilder(const std::tstring &Format, CParameter p0, CParameter p1, CParameter p2, CParameter p3, CParameter p4, CParameter p5, CParameter p6, CParameter p7, CParameter p8, CParameter p9) : UnformattedText(Format), IsBuilt(false)
        {
            ParameterVector.reserve(10);
            ParameterVector.push_back(p0);
            ParameterVector.push_back(p1);
            ParameterVector.push_back(p2);
            ParameterVector.push_back(p3);
            ParameterVector.push_back(p4);
            ParameterVector.push_back(p5);
            ParameterVector.push_back(p6);
            ParameterVector.push_back(p7);
            ParameterVector.push_back(p8);
            ParameterVector.push_back(p9);
        }
    }; // class CBuilder
} // namespace pja
