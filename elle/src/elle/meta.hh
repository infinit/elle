#ifndef ELLE_META_HH
# define ELLE_META_HH

namespace elle
{
  namespace meta
  {
    template <typename ... Elts>
    struct List
    {
      /// T<Elts...>
      template <template <typename ...> class T>
      struct
      apply;

      /// A list containing elements that match P
      template <template <typename> class P>
      struct filter;

      /// The position of T in List
      template <typename T>
      struct
      index_of;

      /// List<T, Elts...>
      template <typename T>
      struct
      prepend;

      /// List<F<Elts>...>
      template <int = 0>
      struct
      tail;
    };
  }
}

# include <elle/meta.hxx>

#endif
