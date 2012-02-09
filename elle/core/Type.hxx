//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri jan 30 16:29:55 2009]
//

#ifndef ELLE_CORE_TYPE_HXX
#define ELLE_CORE_TYPE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <limits.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

  namespace core
  {

//
// ---------- type ------------------------------------------------------------
//

    ///
    /// this default template does nothing interesting.
    ///
    template <typename T>
    struct Type
    {
    };

    ///
    /// this template-generated structure maintains general information
    /// regarding a type.
    ///
#define TypeDeclare(_type_)                                             \
  template <>                                                           \
  struct Type<_type_>                                                   \
  {                                                                     \
    static const Character*     Name;                                   \
                                                                        \
    static const _type_         Minimum;                                \
    static const _type_         Maximum;                                \
                                                                        \
    static const _type_         Default;                                \
  };

    ///
    /// this macro-function specifies the information related to
    /// a type since these cannot be embedded inside at declaration time.
    ///
#define TypeDefine(_type_, _minimum_, _maximum_, _default_)             \
  const Character*      Type<_type_>::Name = #_type_;                   \
                                                                        \
  const _type_          Type<_type_>::Minimum = _minimum_;              \
                                                                        \
  const _type_          Type<_type_>::Maximum = _maximum_;              \
                                                                        \
  const _type_          Type<_type_>::Default = _default_;

    ///
    /// these macro-function calls define some basic types: boolean,
    /// character, integer, natural, real.
    ///
    /// note that byte is an alias of natural8, hence do not require
    /// a specific declaration.
    ///
    TypeDeclare(Null);
    TypeDeclare(Boolean);
    TypeDeclare(Character);
    TypeDeclare(Integer8);
    TypeDeclare(Integer16);
    TypeDeclare(Integer32);
    TypeDeclare(Integer64);
    TypeDeclare(Natural8);
    TypeDeclare(Natural16);
    TypeDeclare(Natural32);
    TypeDeclare(Natural64);
    TypeDeclare(String);
    TypeDeclare(Real);

    ///
    /// large-specific declaration which contains a name only
    /// since Large i.e ::BIGNUM cannot be declared statically i.e as
    /// constants.
    ///
    template <>
    struct Type<Large>
    {
      static const Character*   Name;
    };

//
// ---------- variable --------------------------------------------------------
//

    ///
    /// this template provides the caller to retrieve the minmum
    /// value a variable can hold.
    ///
    template <typename T>
    T                   Variable::Minimum(const T&)
    {
      return (Type<T>::Minimum);
    }

    ///
    /// this template provides the caller to retrieve the minmum
    /// value a variable can hold.
    ///
    template <typename T>
    T                   Variable::Maximum(const T&)
    {
      return (Type<T>::Maximum);
    }

    ///
    /// this method performs a conversion between same types.
    ///
    /// there is therefore no need for a so-called conversion.
    ///
    template <typename T>
    Status              Variable::Convert(const T&              input,
                                          T&                    output)
    {
      // copy.
      output = input;

      return elle::StatusOk;
    }

    ///
    /// this method tries to convert any type into any other type.
    ///
    template <typename T1,
              typename T2>
    Status              Variable::Convert(const T1&             input,
                                          T2&                   output)
    {
      String            string;

      // convert the input into a string-based format.
      if (Variable::Convert(input, string) == StatusError)
        escape("unable to convert the input into a string");

      // convert the string into the output.
      if (Variable::Convert(string, output) == StatusError)
        escape("unable to convert the string into the output");

      return elle::StatusOk;
    }

  }
}

#endif
