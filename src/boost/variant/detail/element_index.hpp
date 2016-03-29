//-----------------------------------------------------------------------------
// boost variant/detail/element_index.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2014-2015 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_VARIANT_DETAIL_ELEMENT_INDEX_HPP
#define BOOST_VARIANT_DETAIL_ELEMENT_INDEX_HPP

#include "boost/config.hpp"
#include "boost/variant/recursive_wrapper_fwd.hpp"
#include "boost/variant/variant_fwd.hpp"

#include "boost/type_traits/remove_cv.hpp"
#include "boost/type_traits/remove_reference.hpp"
#include "boost/mpl/find_if.hpp"

namespace network_boost { namespace detail { namespace variant {

template <class VariantElement, class T>
struct variant_element_functor :
    network_boost::mpl::or_<
        network_boost::is_same<VariantElement, T>,
        network_boost::is_same<VariantElement, network_boost::recursive_wrapper<T> >,
        network_boost::is_same<VariantElement, T& >
    >
{};

template <class Types, class T>
struct element_iterator_impl :
    network_boost::mpl::find_if<
        Types,
        network_boost::mpl::or_<
            variant_element_functor<network_boost::mpl::_1, T>,
            variant_element_functor<network_boost::mpl::_1, typename network_boost::remove_cv<T>::type >
        >
    >
{};

template <class Variant, class T>
struct element_iterator :
    element_iterator_impl< typename Variant::types, typename network_boost::remove_reference<T>::type >
{};

template <class Variant, class T>
struct holds_element :
    network_boost::mpl::not_<
        network_boost::is_same<
            typename network_boost::mpl::end<typename Variant::types>::type,
            typename element_iterator<Variant, T>::type
        >
    >
{};


}}} // namespace network_boost::detail::variant

#endif // BOOST_VARIANT_DETAIL_ELEMENT_INDEX_HPP
