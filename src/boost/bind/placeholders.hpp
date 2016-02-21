#ifndef BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
#define BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  bind/placeholders.hpp - _N definitions
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//  Copyright 2015 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

#include <boost/bind/arg.hpp>
#include <boost/config.hpp>

namespace network_boost
{

namespace placeholders
{

#if defined(__BORLANDC__) || defined(__GNUC__) && (__GNUC__ < 4)

inline network_boost::arg<1> _1() { return network_boost::arg<1>(); }
inline network_boost::arg<2> _2() { return network_boost::arg<2>(); }
inline network_boost::arg<3> _3() { return network_boost::arg<3>(); }
inline network_boost::arg<4> _4() { return network_boost::arg<4>(); }
inline network_boost::arg<5> _5() { return network_boost::arg<5>(); }
inline network_boost::arg<6> _6() { return network_boost::arg<6>(); }
inline network_boost::arg<7> _7() { return network_boost::arg<7>(); }
inline network_boost::arg<8> _8() { return network_boost::arg<8>(); }
inline network_boost::arg<9> _9() { return network_boost::arg<9>(); }

#else

BOOST_STATIC_CONSTEXPR network_boost::arg<1> _1;
BOOST_STATIC_CONSTEXPR network_boost::arg<2> _2;
BOOST_STATIC_CONSTEXPR network_boost::arg<3> _3;
BOOST_STATIC_CONSTEXPR network_boost::arg<4> _4;
BOOST_STATIC_CONSTEXPR network_boost::arg<5> _5;
BOOST_STATIC_CONSTEXPR network_boost::arg<6> _6;
BOOST_STATIC_CONSTEXPR network_boost::arg<7> _7;
BOOST_STATIC_CONSTEXPR network_boost::arg<8> _8;
BOOST_STATIC_CONSTEXPR network_boost::arg<9> _9;

#endif

} // namespace placeholders

} // namespace network_boost

#endif // #ifndef BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
