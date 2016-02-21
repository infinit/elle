//  Copyright (c) 2001-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_PHOENIX_ATTRIBUTES_OCT_01_2009_1128AM)
#define BOOST_SPIRIT_KARMA_PHOENIX_ATTRIBUTES_OCT_01_2009_1128AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/include/version.hpp>

// we support Phoenix attributes only starting with V2.2
#if SPIRIT_VERSION >= 0x2020

#include <boost/spirit/home/karma/detail/attributes.hpp>
#include <boost/spirit/home/karma/detail/indirect_iterator.hpp>
#include <boost/spirit/home/support/container.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/utility/result_of.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace network_boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // Provide customization points allowing the use of phoenix expressions as 
    // generator functions in the context of generators expecting a container 
    // attribute (Kleene, plus, list, repeat, etc.)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Eval>
    struct is_container<network_boostphoenix::actor<Eval> const>
      : is_container<typename network_boost::result_of<network_boostphoenix::actor<Eval>()>::type>
    {};

    template <typename Eval>
    struct container_iterator<network_boostphoenix::actor<Eval> const>
    {
        typedef network_boostphoenix::actor<Eval> const& type;
    };

    template <typename Eval>
    struct begin_container<network_boostphoenix::actor<Eval> const>
    {
        typedef network_boostphoenix::actor<Eval> const& type;
        static type call(network_boostphoenix::actor<Eval> const& f)
        {
            return f;
        }
    };

    template <typename Eval>
    struct end_container<network_boostphoenix::actor<Eval> const>
    {
        typedef network_boostphoenix::actor<Eval> const& type;
        static type call(network_boostphoenix::actor<Eval> const& f)
        {
            return f;
        }
    };

    template <typename Eval>
    struct deref_iterator<network_boostphoenix::actor<Eval> const>
    {
        typedef typename network_boost::result_of<network_boostphoenix::actor<Eval>()>::type type;
        static type call(network_boostphoenix::actor<Eval> const& f)
        {
            return f();
        }
    };

    template <typename Eval>
    struct next_iterator<network_boostphoenix::actor<Eval> const>
    {
        typedef network_boostphoenix::actor<Eval> const& type;
        static type call(network_boostphoenix::actor<Eval> const& f)
        {
            return f;
        }
    };

    template <typename Eval>
    struct compare_iterators<network_boostphoenix::actor<Eval> const>
    {
        static bool 
        call(network_boostphoenix::actor<Eval> const&, network_boostphoenix::actor<Eval> const&)
        {
            return false;
        }
    };

    template <typename Eval>
    struct container_value<network_boostphoenix::actor<Eval> >
    {
        typedef network_boostphoenix::actor<Eval> const& type;
    };

    template <typename Eval>
    struct make_indirect_iterator<network_boostphoenix::actor<Eval> const>
    {
        typedef network_boostphoenix::actor<Eval> const& type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Handle Phoenix actors as attributes, just invoke the function object
    // and deal with the result as the attribute.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Eval, typename Exposed>
    struct extract_from_attribute<network_boostphoenix::actor<Eval>, Exposed>
    {
        typedef typename network_boost::result_of<network_boostphoenix::actor<Eval>()>::type type;

        template <typename Context>
        static type call(network_boostphoenix::actor<Eval> const& f, Context& context)
        {
            return f(unused, context);
        }
    };
}}}

#endif
#endif
