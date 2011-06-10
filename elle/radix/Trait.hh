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
// updated       julien quintard   [fri jun 10 12:39:03 2011]
//

#ifndef ELLE_RADIX_TRAIT_HH
#define ELLE_RADIX_TRAIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Parameters.hh>

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

      template <typename T>
      struct Bare<const T>
      {
	typedef T				Type;
      };

      template <typename T>
      struct Bare<const T&>
      {
	typedef T				Type;
      };

      template <typename T>
      struct Bare<const T*>
      {
	typedef T				Type;
      };

      template <typename... T>
      struct Bare< Parameters<T...> >
      {
	typedef Parameters<T...>		Type;
      };

      template <typename... T>
      struct Bare< Parameters<const T...> >
      {
	typedef Parameters<T...>		Type;
      };

      template <typename... T>
      struct Bare< Parameters<T&...> >
      {
	typedef Parameters<T...>		Type;
      };

      template <typename... T>
      struct Bare< Parameters<T*...> >
      {
	typedef Parameters<T...>		Type;
      };

      // constant
      template <typename T>
      struct Constant
      {
	typedef const T				Type;
      };

      template <typename T>
      struct Constant<const T>
      {
	typedef const T				Type;
      };

      template <typename... T>
      struct Constant< Parameters<T...> >
      {
	typedef Parameters<const T...>		Type;
      };

      template <typename... T>
      struct Constant< Parameters<const T...> >
      {
	typedef Parameters<const T...>		Type;
      };

      // pointer
      template <typename T>
      struct Pointer
      {
	typedef T*				Type;
      };

      template <typename T>
      struct Pointer<T*>
      {
	typedef T*				Type;
      };

      template <typename... T>
      struct Pointer< Parameters<T...> >
      {
	typedef Parameters<T*...>		Type;
      };

      template <typename... T>
      struct Pointer< Parameters<T*...> >
      {
	typedef Parameters<T*...>		Type;
      };

      // reference
      template <typename T>
      struct Reference
      {
	typedef T&				Type;
      };

      template <typename T>
      struct Reference<T&>
      {
	typedef T&				Type;
      };

      template <typename... T>
      struct Reference< Parameters<T...> >
      {
	typedef Parameters<T&...>		Type;
      };

      template <typename... T>
      struct Reference< Parameters<T&...> >
      {
	typedef Parameters<T&...>		Type;
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
	// invalid use.
      };

      // scoutor
      template <typename T>
      struct Scoutor
      {
	// invalid use.
      };

      template <typename T>
      struct Scoutor<const T>
      {
	typedef typename T::const_iterator	Type;
      };
    };

  }
}

#endif
