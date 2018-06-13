#ifndef JSON_SPIRIT_WRITER
#define JSON_SPIRIT_WRITER

//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "platform.h"
#include "value.h"
#include "writer_options.h"
#include <iostream>

namespace json_spirit
{
    // these functions to convert JSON Values to text

#ifdef JSON_SPIRIT_VALUE_ENABLED
void JSON_SPIRIT_FUNCTION_EXPORT write(const Value& value, std::ostream&  os, unsigned int options = 0);
std::string JSON_SPIRIT_FUNCTION_EXPORT write(const Value& value, unsigned int options = 0);
#endif

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
void JSON_SPIRIT_FUNCTION_EXPORT write(const wValue& value, std::wostream& os, unsigned int options = 0);
std::wstring JSON_SPIRIT_FUNCTION_EXPORT write(const wValue& value, unsigned int options = 0);
#endif

    // these "formatted" versions of the "write" functions are the equivalent of the above functions
    // with option "pretty_print"

#ifdef JSON_SPIRIT_VALUE_ENABLED
void JSON_SPIRIT_FUNCTION_EXPORT write_formatted(const Value& value, std::ostream&  os);
std::string JSON_SPIRIT_FUNCTION_EXPORT write_formatted(const Value& value);
#endif

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
void JSON_SPIRIT_FUNCTION_EXPORT write_formatted(const wValue& value, std::wostream& os);
std::wstring JSON_SPIRIT_FUNCTION_EXPORT write_formatted(const wValue& value);
#endif
}

#endif
