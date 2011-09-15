//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri jul 15 10:45:39 2011]
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

    ///
    /// these templates remove traits from the given type.
    ///

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

    ///
    /// these templates make the given type constant.
    ///

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

    ///
    /// these templates transform the given type into a pointer.
    ///

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

    ///
    /// these templates transform the given type into a reference.
    ///

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
