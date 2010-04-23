//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Core.hxx
//
// created       julien quintard   [fri jan 30 16:29:55 2009]
// updated       julien quintard   [wed mar 10 20:39:01 2010]
//

#ifndef ELLE_CORE_CORE_HXX
#define ELLE_CORE_CORE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <limits.h>
#include <elle/idiom/Open.hh>

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
#define CoreDeclare(_type_, _minimum_, _maximum_)			\
  template <>								\
  class Type<_type_>:							\
    public Limits<_type_, _minimum_, _maximum_>	\
  {									\
  };

    ///
    /// these macro-function calls define the limits of the following basic
    /// core: boolean, character, integer, natural, real.
    ///
    CoreDeclare(Null, Nil, Nil);
    CoreDeclare(Boolean, false, true);
    CoreDeclare(Character, 0, CHAR_MAX);
    CoreDeclare(Integer8, CHAR_MIN, CHAR_MAX);
    CoreDeclare(Integer16, SHRT_MIN, SHRT_MAX);
    CoreDeclare(Integer32, INT_MIN, INT_MAX);
    CoreDeclare(Integer64, LLONG_MIN, LLONG_MAX);
    CoreDeclare(Natural8, 0, UCHAR_MAX);
    CoreDeclare(Natural16, 0, USHRT_MAX);
    CoreDeclare(Natural32, 0, UINT_MAX);
    CoreDeclare(Natural64, 0, ULLONG_MAX);

//
// ---------- variable --------------------------------------------------------
//

    ///
    /// this template provides the caller to retrieve the minmum
    /// value a variable can hold.
    ///
    template <typename T>
    T			Variable::Minimum(const T&)
    {
      return Type<T>::Minimum;
    }

    ///
    /// this template provides the caller to retrieve the minmum
    /// value a variable can hold.
    ///
    template <typename T>
    T			Variable::Maximum(const T&)
    {
      return Type<T>::Maximum;
    }

  }
}

#endif
