///////////////////////////////////////////////////////////////////////////////
/// \file size.hpp
/// Proto callables for network_boost::size()
//
//  Copyright 2012 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_FUNCTIONAL_RANGE_SIZE_HPP_EAN_27_08_2012
#define BOOST_PROTO_FUNCTIONAL_RANGE_SIZE_HPP_EAN_27_08_2012

#include <boost/range/size.hpp>
#include <boost/proto/proto_fwd.hpp>

namespace network_boost { namespace proto { namespace functional
{

    // A PolymorphicFunctionObject that wraps network_boost::size()
    struct size
    {
        BOOST_PROTO_CALLABLE()

        template<typename Sig>
        struct result;

        template<typename This, typename Rng>
        struct result<This(Rng)>
          : network_boost::range_size<Rng>
        {};

        template<typename This, typename Rng>
        struct result<This(Rng &)>
          : network_boost::range_size<Rng>
        {};

        template<typename Rng>
        typename network_boost::range_size<Rng>::type operator()(Rng const &rng) const
        {
            return network_boost::size(rng);
        }
    };

}}}

#endif
