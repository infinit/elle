//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Trait.hh
//
// created       julien quintard   [tue jun  7 13:43:54 2011]
// updated       julien quintard   [sat sep  3 20:50:34 2011]
//

#ifndef ELLE_RADIX_TRAIT_HH
#define ELLE_RADIX_TRAIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Parameters.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace radix
  {

//
// ---------- structures ------------------------------------------------------
//

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
	typedef T				Type;
      };

      // constant
      template <typename T>
      struct Constant
      {
	typedef const T				Type;
      };

      // pointer
      template <typename T>
      struct Pointer
      {
	typedef T*				Type;
      };

      // reference
      template <typename T>
      struct Reference
      {
	typedef T&				Type;
      };

      // iterator
      template <typename T>
      struct Iterator
      {
	typedef typename T::iterator		Type;
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
	typedef typename T::const_iterator	Type;
      };

    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Trait.hxx>

#endif
