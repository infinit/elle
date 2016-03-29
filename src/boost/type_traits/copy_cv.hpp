#ifndef BOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED
#define BOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED

//
//  Copyright 2015 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_volatile.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_volatile.hpp>
#include <boost/type_traits/conditional.hpp>

namespace network_boost
{

template<class T, class U> struct copy_cv
{
private:

    typedef typename network_boost::conditional<network_boost::is_const<U>::value, typename network_boost::add_const<T>::type, T>::type CT;

public:

    typedef typename network_boost::conditional<network_boost::is_volatile<U>::value, typename network_boost::add_volatile<CT>::type, CT>::type type;
};

} // namespace network_boost

#endif // #ifndef BOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED
