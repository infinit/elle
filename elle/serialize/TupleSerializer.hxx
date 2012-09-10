#ifndef  ELLE_SERIALIZE_TUPLE_SERIALIZER_HXX
# define ELLE_SERIALIZE_TUPLE_SERIALIZER_HXX

# include <cassert>
# include <tuple>

# include <elle/serialize/Serializer.hh>

namespace elle
{
  namespace serialize
  {

    namespace detail
    {

      template<size_t size_left>
      struct TupleAccess
      {
        template<typename Archive, typename T>
        static void Load(Archive& ar, T& value)
        {
          ar >> std::get<std::tuple_size<T>::value - size_left>(value);
          TupleAccess<size_left - 1>::Load(ar, value);
        }
        template<typename Archive, typename T>
        static void Save(Archive& ar, T const& value)
        {
          ar << std::get<std::tuple_size<T>::value - size_left>(value);
          TupleAccess<size_left - 1>::Save(ar, value);
        }

      };

      template<>
      struct TupleAccess<0>
      {
        template<typename Archive, typename T>
        static void Load(Archive&, T&) {}

        template<typename Archive, typename T>
        static void Save(Archive&, T const&) {}
      };

    }

    // XXX could be done without variadic templates
    template<typename... T>
      struct Serializer<std::tuple<T...>>
        : public SplitSerializer<std::tuple<T...>>
      {
        template<typename Archive>
          static void Load(Archive& ar,
                           std::tuple<T...>& value,
                           unsigned int version)
          {
            detail::TupleAccess<sizeof...(T)>::Load(ar, value);
          }

        template<typename Archive>
          static void Save(Archive& ar,
                           std::tuple<T...> const& value,
                           unsigned int version)
          {
            detail::TupleAccess<sizeof...(T)>::Save(ar, value);
          }
      };

    template<typename... T>
      struct StoreFormat<std::tuple<T...>>
      {
        static bool const value = false;
      };



    /// Helper to create tuples of references
    template<typename... T>
      std::tuple<T&...> make_tuple(T&... values)
      {
        return std::tuple<T&...>(values...);
      }
  }
}

#endif
