#pragma once

#include <type_traits>

#include <das/model.hh>

namespace das
{
  namespace
  {
    template <typename O, template <typename> class M>
    struct flatten_object;

    template <typename Model, typename T, template <typename> class M>
    typename Model::Fields::template map<
      flatten_object<T, M>::template flatten>
      ::type::template apply<std::tuple>::type
    _flatten(typename M<T>::object_type o);

    template <typename T>
    struct FlattenByValue
    {
      using object_type = T const&;
      using type = T;
      template <typename V>
      static
      V const&
      value(V const& v)
      {
        return v;
      }
    };

    template <typename T>
    struct FlattenByRef
    {
      using object_type = T&;
      using type = typename std::remove_reference<T>::type&;
      static
      type
      value(T& v)
      {
        return v;
      }
    };

    template <typename T>
    struct FlattenByRefWrapper
    {
      using object_type = T&;
      using type =
        std::reference_wrapper<typename std::remove_reference<T>::type>;
       static
       type
       value(T& v)
       {
        return std::ref(v);
       }
     };

    template <typename T>
    struct FlattenByConstRef
    {
      using object_type = T const&;
      using type = T const&;
      static
      type
      value(T const& v)
      {
        return v;
      }
    };

    template <typename T>
    struct FlattenByConstRefWrapper
    {
      using object_type = T const&;
      using type =
        std::reference_wrapper<typename std::remove_reference<T>::type const>;
      static
      type
      value(T const& v)
      {
        return std::ref(v);
      }
    };

    template <typename T, template <typename> class M>
    struct FlattenRecurse
    {
      using Model = typename DefaultModel<T>::type;
      using type =
        typename Model::Fields::template map<
        flatten_object<T, M>::template flatten>
        ::type::template apply<std::tuple>::type;
      static
      type
      value(typename M<T>::object_type o)
      {
        return _flatten<Model, T, M>(o);
      }
    };

    template <typename T, template <typename> class M>
    struct FlattenCompose
      : public std::conditional<model_has<T>(),
                                FlattenRecurse<T, M>,
                                M<T> >::type
    {};

    template <typename Model, typename T, template <typename> class M>
    typename Model::Fields::template map<
      flatten_object<T, M>::template flatten>
      ::type::template apply<std::tuple>::type
    _flatten(typename M<T>::object_type o)
    {
      return Model::Fields::template map<
        flatten_object<T, M>::template flatten>::value(o);
    }

    template <typename O, template <typename> class M>
    struct flatten_object
    {
      template <typename S>
      struct flatten
      {
        using Method = FlattenCompose<
          typename S::template attr_type<O>::type, M>;
        using type = typename Method::type;
        static
        type
        value(typename M<O>::object_type o)
        {
          return Method::value(S::attr_get(o));
        }
      };
    };
  }

  /// Flatten a structure: return it as a tuple of values.
  template <typename Model, typename T>
  typename Model::Fields::template map<
    flatten_object<T, FlattenByValue>::template flatten>
    ::type::template apply<std::tuple>::type
  flatten(T const& o)
  {
    return _flatten<Model, T, FlattenByValue>(o);
  }

  /// Flatten a structure: return it as a tuple of values.
  template <typename T>
  auto
  flatten(T const& o)
    -> decltype(flatten<typename DefaultModel<T>::type, T>(o))
  {
    return flatten<typename DefaultModel<T>::type, T>(o);
  }

  // Flatten by ref
  template <typename Model, typename T>
  typename Model::Fields::template map<
    flatten_object<T, FlattenByRef>::template flatten>
    ::type::template apply<std::tuple>::type
  flatten_ref(T& o)
  {
    return _flatten<Model, T, FlattenByRef>(o);
  }

  template <typename T>
  auto
  flatten_ref(T& o)
    -> decltype(flatten_ref<typename DefaultModel<T>::type, T>(o))
  {
    return flatten_ref<typename DefaultModel<T>::type, T>(o);
  }

  // Flatten by const ref
  template <typename Model, typename T>
  typename Model::Fields::template map<
    flatten_object<T, FlattenByConstRef>::template flatten>
    ::type::template apply<std::tuple>::type
  flatten_ref(T const& o)
  {
    return _flatten<Model, T, FlattenByConstRef>(o);
  }

  template <typename T>
  auto
  flatten_ref(T const& o)
    -> decltype(flatten_ref<typename DefaultModel<T>::type, T>(o))
  {
    return flatten_ref<typename DefaultModel<T>::type, T>(o);
  }
}

