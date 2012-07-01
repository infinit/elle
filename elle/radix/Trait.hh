#ifndef ELLE_RADIX_TRAIT_HH
# define ELLE_RADIX_TRAIT_HH

// XXX[class to delete]

#include <elle/radix/Parameters.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace radix
  {

    ///
    /// this set of template structures enables the programmer to add
    /// traits to a given type such as constness, reference etc.
    ///
    struct Trait
    {
      //
      // structures
      //

      // bare
      template <typename T>
      struct Bare
      {
        typedef T                               Type;
      };

      // constant
      template <typename T>
      struct Constant
      {
        typedef const T                         Type;
      };

      // pointer
      template <typename T>
      struct Pointer
      {
        typedef T*                              Type;
      };

      // reference
      template <typename T>
      struct Reference
      {
        typedef T&                              Type;
      };

      // iterator
      template <typename T>
      struct Iterator
      {
        typedef typename T::iterator            Type;
      };

      template <typename T>
      struct Iterator<const T>
      {
        // invalid use: no Type defined.
      };

      // scoutor
      template <typename T>
      struct Scoutor
      {
        // invalid use: no Type defined.
      };

      template <typename T>
      struct Scoutor<const T>
      {
        typedef typename T::const_iterator      Type;
      };

    };

  }
}

#include <elle/radix/Trait.hxx>

#endif
