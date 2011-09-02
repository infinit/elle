//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Trait.hxx
//
// created       julien quintard   [fri jul 15 10:45:39 2011]
// updated       julien quintard   [thu sep  1 17:30:33 2011]
//

#ifndef ELLE_RADIX_TRAIT_HXX
#define ELLE_RADIX_TRAIT_HXX

namespace elle
{
  namespace radix
  {

//
// ---------- bare ------------------------------------------------------------
//

    template <typename T>
    struct Trait::Bare<T&>
    {
      typedef T					Type;
    };

    template <typename T>
    struct Trait::Bare<T*>
    {
      typedef T					Type;
    };

    template <typename T>
    struct Trait::Bare<const T>
    {
      typedef T					Type;
    };

    template <typename T>
    struct Trait::Bare<const T&>
    {
      typedef T					Type;
    };

    template <typename T>
    struct Trait::Bare<const T*>
    {
      typedef T					Type;
    };

    template <typename... T>
    struct Trait::Bare< Parameters<T...> >
    {
      typedef Parameters<T...>			Type;
    };

    template <typename... T>
    struct Trait::Bare< Parameters<T&...> >
    {
      typedef Parameters<T...>			Type;
    };

    template <typename... T>
    struct Trait::Bare< Parameters<T*...> >
    {
      typedef Parameters<T...>			Type;
    };

    template <typename... T>
    struct Trait::Bare< Parameters<const T...> >
    {
      typedef Parameters<T...>			Type;
    };

    template <typename... T>
    struct Trait::Bare< Parameters<const T&...> >
    {
      typedef Parameters<T...>			Type;
    };

    template <typename... T>
    struct Trait::Bare< Parameters<const T*...> >
    {
      typedef Parameters<T...>			Type;
    };

    template <>
    struct Trait::Bare< Parameters<> >
    {
      typedef Parameters<>			Type;
    };

//
// ---------- constants -------------------------------------------------------
//

    template <typename T>
    struct Trait::Constant<const T>
    {
      typedef const T				Type;
    };

    template <typename T>
    struct Trait::Constant<T&>
    {
      typedef const T&				Type;
    };

    template <typename T>
    struct Trait::Constant<T*>
    {
      typedef const T*				Type;
    };

    template <typename... T>
    struct Trait::Constant< Parameters<T...> >
    {
      typedef Parameters<const T...>		Type;
    };

    template <typename... T>
    struct Trait::Constant< Parameters<T&...> >
    {
      typedef Parameters<const T&...>		Type;
    };

    template <typename... T>
    struct Trait::Constant< Parameters<T*...> >
    {
      typedef Parameters<const T*...>		Type;
    };

    template <typename... T>
    struct Trait::Constant< Parameters<const T...> >
    {
      typedef Parameters<const T...>		Type;
    };

    template <>
    struct Trait::Constant< Parameters<> >
    {
      typedef Parameters<>			Type;
    };

//
// ---------- pointer ---------------------------------------------------------
//

    template <typename T>
    struct Trait::Pointer<T*>
    {
      typedef T*				Type;
    };

    template <typename... T>
    struct Trait::Pointer< Parameters<T...> >
    {
      typedef Parameters<T*...>			Type;
    };

    template <typename... T>
    struct Trait::Pointer< Parameters<T*...> >
    {
      typedef Parameters<T*...>			Type;
    };

//
// ---------- reference -------------------------------------------------------
//

    template <typename T>
    struct Trait::Reference<T&>
    {
      typedef T&				Type;
    };

    template <typename... T>
    struct Trait::Reference< Parameters<T...> >
    {
      typedef Parameters<T&...>			Type;
    };

    template <typename... T>
    struct Trait::Reference< Parameters<T&...> >
    {
      typedef Parameters<T&...>			Type;
    };

  }
}

#endif
