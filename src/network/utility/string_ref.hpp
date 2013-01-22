// Copyright 2012, 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_STRING_REF_INC
#define NETWORK_STRING_REF_INC

#include <network/uri/config.hpp>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <string>
#include <boost/optional.hpp>
#include <boost/range/as_literal.hpp>

namespace network {
  template <class CharT,
            class CharTraits = std::char_traits<CharT>,
            class Allocator = std::allocator<CharT> >
  class basic_string_ref {

  public:

    typedef std::basic_string<CharT, CharTraits, Allocator> string_type;
    typedef typename string_type::const_iterator const_iterator;
    typedef const_iterator iterator;
    typedef typename std::iterator_traits<iterator>::value_type value_type;

#if defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    typedef bool (basic_string_ref::*unspecified_bool_type)() const;
#endif // defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)

    basic_string_ref() {

    }

    basic_string_ref(const_iterator first, const_iterator last)
      : first_(first), last_(last) {

    }

    const_iterator begin() const {
      return first_;
    }

    const_iterator end() const {
      return last_;
    }

    bool empty() const {
      return first_ == last_;
    }

#if defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    operator unspecified_bool_type () const {
      return !empty()? &basic_string_ref::empty : nullptr;
    }
#else
    explicit operator bool () const {
      return !empty();
    }
#endif // defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)

#if defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    operator string_type () const {
      return string_type(first_, last_);
    }
#else
    template <class AllocL>
    explicit operator std::basic_string<CharT, CharTraits, AllocL> () const {
      return std::basic_string<CharT, CharTraits, AllocL>(first_, last_);
    }
#endif // !defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)

  private:

    const_iterator first_, last_;

  };

  template <typename CharT, class CharTraits, class Allocator>
  inline
  bool operator == (const basic_string_ref<CharT, CharTraits, Allocator> &lhs, const basic_string_ref<CharT, CharTraits, Allocator> &rhs) {
    return boost::equal(lhs, rhs);
  }

  template <typename CharT, class CharTraits, class Allocator>
  inline
  bool operator == (const basic_string_ref<CharT, CharTraits, Allocator> &lhs, const CharT *rhs) {
    return boost::equal(lhs, boost::as_literal(rhs));
  }

  template <typename CharT, class CharTraits, class Allocator>
  inline
  bool operator == (const CharT *lhs, const basic_string_ref<CharT, CharTraits, Allocator> &rhs) {
    return rhs == lhs;
  }

  typedef basic_string_ref<char> string_ref;
  typedef basic_string_ref<wchar_t> wstring_ref;
} // namespace network


#endif // NETWORK_STRING_REF_INC
