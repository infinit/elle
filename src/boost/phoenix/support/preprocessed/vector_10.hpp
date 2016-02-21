/*==============================================================================
    Copyright (c) 2005-2010 Joel de Guzman
    Copyright (c) 2010 Thomas Heller

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
namespace network_boost { namespace network_boostphoenix
{
    template <typename Dummy = void>
    struct vector0
    {
        typedef mpl::int_<0> size_type;
        static const int size_value = 0;
    };
}}
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0>
    struct vector1
    {
        typedef A0 member_type0; A0 a0;
        
        typedef mpl::int_<1> size_type;
        static const int size_value = 1;
        typedef
            vector0<>
            args_type;
        args_type args() const
        {
            args_type r = {};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0)
  , ( network_boost::network_boostphoenix::vector1 ) (A0)
  , (A0, a0)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1>
    struct vector2
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1;
        
        typedef mpl::int_<2> size_type;
        static const int size_value = 2;
        typedef
            vector1<A1>
            args_type;
        args_type args() const
        {
            args_type r = {a1};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1)
  , ( network_boost::network_boostphoenix::vector2 ) (A0) (A1)
  , (A0, a0) (A1, a1)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2>
    struct vector3
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2;
        
        typedef mpl::int_<3> size_type;
        static const int size_value = 3;
        typedef
            vector2<A1 , A2>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2)
  , ( network_boost::network_boostphoenix::vector3 ) (A0) (A1) (A2)
  , (A0, a0) (A1, a1) (A2, a2)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3>
    struct vector4
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3;
        
        typedef mpl::int_<4> size_type;
        static const int size_value = 4;
        typedef
            vector3<A1 , A2 , A3>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3)
  , ( network_boost::network_boostphoenix::vector4 ) (A0) (A1) (A2) (A3)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3 , typename A4>
    struct vector5
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3; typedef A4 member_type4; A4 a4;
        
        typedef mpl::int_<5> size_type;
        static const int size_value = 5;
        typedef
            vector4<A1 , A2 , A3 , A4>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3 , a4};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3) (A4)
  , ( network_boost::network_boostphoenix::vector5 ) (A0) (A1) (A2) (A3) (A4)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3) (A4, a4)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5>
    struct vector6
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3; typedef A4 member_type4; A4 a4; typedef A5 member_type5; A5 a5;
        
        typedef mpl::int_<6> size_type;
        static const int size_value = 6;
        typedef
            vector5<A1 , A2 , A3 , A4 , A5>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3 , a4 , a5};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3) (A4) (A5)
  , ( network_boost::network_boostphoenix::vector6 ) (A0) (A1) (A2) (A3) (A4) (A5)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3) (A4, a4) (A5, a5)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6>
    struct vector7
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3; typedef A4 member_type4; A4 a4; typedef A5 member_type5; A5 a5; typedef A6 member_type6; A6 a6;
        
        typedef mpl::int_<7> size_type;
        static const int size_value = 7;
        typedef
            vector6<A1 , A2 , A3 , A4 , A5 , A6>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3 , a4 , a5 , a6};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3) (A4) (A5) (A6)
  , ( network_boost::network_boostphoenix::vector7 ) (A0) (A1) (A2) (A3) (A4) (A5) (A6)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3) (A4, a4) (A5, a5) (A6, a6)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7>
    struct vector8
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3; typedef A4 member_type4; A4 a4; typedef A5 member_type5; A5 a5; typedef A6 member_type6; A6 a6; typedef A7 member_type7; A7 a7;
        
        typedef mpl::int_<8> size_type;
        static const int size_value = 8;
        typedef
            vector7<A1 , A2 , A3 , A4 , A5 , A6 , A7>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3 , a4 , a5 , a6 , a7};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7)
  , ( network_boost::network_boostphoenix::vector8 ) (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3) (A4, a4) (A5, a5) (A6, a6) (A7, a7)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8>
    struct vector9
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3; typedef A4 member_type4; A4 a4; typedef A5 member_type5; A5 a5; typedef A6 member_type6; A6 a6; typedef A7 member_type7; A7 a7; typedef A8 member_type8; A8 a8;
        
        typedef mpl::int_<9> size_type;
        static const int size_value = 9;
        typedef
            vector8<A1 , A2 , A3 , A4 , A5 , A6 , A7 , A8>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7) (A8)
  , ( network_boost::network_boostphoenix::vector9 ) (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7) (A8)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3) (A4, a4) (A5, a5) (A6, a6) (A7, a7) (A8, a8)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9>
    struct vector10
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3; typedef A4 member_type4; A4 a4; typedef A5 member_type5; A5 a5; typedef A6 member_type6; A6 a6; typedef A7 member_type7; A7 a7; typedef A8 member_type8; A8 a8; typedef A9 member_type9; A9 a9;
        
        typedef mpl::int_<10> size_type;
        static const int size_value = 10;
        typedef
            vector9<A1 , A2 , A3 , A4 , A5 , A6 , A7 , A8 , A9>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7) (A8) (A9)
  , ( network_boost::network_boostphoenix::vector10 ) (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7) (A8) (A9)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3) (A4, a4) (A5, a5) (A6, a6) (A7, a7) (A8, a8) (A9, a9)
)
    
    
    
    
    
    
    
namespace network_boost { namespace network_boostphoenix
{
    template <typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10>
    struct vector11
    {
        typedef A0 member_type0; A0 a0; typedef A1 member_type1; A1 a1; typedef A2 member_type2; A2 a2; typedef A3 member_type3; A3 a3; typedef A4 member_type4; A4 a4; typedef A5 member_type5; A5 a5; typedef A6 member_type6; A6 a6; typedef A7 member_type7; A7 a7; typedef A8 member_type8; A8 a8; typedef A9 member_type9; A9 a9; typedef A10 member_type10; A10 a10;
        
        typedef mpl::int_<11> size_type;
        static const int size_value = 11;
        typedef
            vector10<A1 , A2 , A3 , A4 , A5 , A6 , A7 , A8 , A9 , A10>
            args_type;
        args_type args() const
        {
            args_type r = {a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10};
            return r;
        }
    };
}}
BOOST_FUSION_ADAPT_TPL_STRUCT_NO_PARTIAL(
    (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7) (A8) (A9) (A10)
  , ( network_boost::network_boostphoenix::vector11 ) (A0) (A1) (A2) (A3) (A4) (A5) (A6) (A7) (A8) (A9) (A10)
  , (A0, a0) (A1, a1) (A2, a2) (A3, a3) (A4, a4) (A5, a5) (A6, a6) (A7, a7) (A8, a8) (A9, a9) (A10, a10)
)
