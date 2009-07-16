//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Core.hxx
//
// created       julien quintard   [fri jan 30 16:29:55 2009]
// updated       julien quintard   [thu feb 19 01:48:01 2009]
//

#ifndef ELLE_CORE_CORE_HXX
#define ELLE_CORE_CORE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <limits.h>

namespace elle
{
  namespace core
  {

//
// ---------- type ------------------------------------------------------------
//

    ///
    /// this template records the limits for a given type.
    ///
    template <typename T, const T L, const T U>
    struct Limits
    {
      static const T	Minimum = L;
      static const T	Maximum = U;
    };

    ///
    /// this default template does nothing interesting.
    ///
    template <typename T>
    class Type
    {
    };

    ///
    /// this template-generated links a type to its limits, therefore
    /// enables code to access limits from the type.
    ///
#define TypeDeclare(_type_, _minimum_, _maximum_)			\
  template <>								\
  class Type<_type_>:							\
    public Limits<_type_, _minimum_, _maximum_>	\
  {									\
  };

    ///
    /// these macro-function calls define the limits of the following basic
    /// core: boolean, character, integer, natural, real.
    ///
    TypeDeclare(Null, Nil, Nil);
    TypeDeclare(Boolean, false, true);
    TypeDeclare(Character, 0, CHAR_MAX);
    TypeDeclare(Integer8, CHAR_MIN, CHAR_MAX);
    TypeDeclare(Integer16, SHRT_MIN, SHRT_MAX);
    TypeDeclare(Integer32, INT_MIN, INT_MAX);
    TypeDeclare(Integer64, LLONG_MIN, LLONG_MAX);
    TypeDeclare(Natural8, 0, UCHAR_MAX);
    TypeDeclare(Natural16, 0, USHRT_MAX);
    TypeDeclare(Natural32, 0, UINT_MAX);
    TypeDeclare(Natural64, 0, ULLONG_MAX);

//
// ---------- variable --------------------------------------------------------
//

    ///
    /// this template provides the caller to retrieve the minmum
    /// value a variable can hold.
    ///
    template <typename T>
    T			Variable::Minimum(T&		variable)
    {
      return Type<T>::Minimum;
    }

    ///
    /// this template provides the caller to retrieve the minmum
    /// value a variable can hold.
    ///
    template <typename T>
    T			Variable::Maximum(T&		variable)
    {
      return Type<T>::Maximum;
    }

  }
}

#endif
