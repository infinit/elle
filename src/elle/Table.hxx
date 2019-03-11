#include <boost/range/irange.hpp>

namespace elle
{
  namespace _details
  {
    namespace table
    {
      template <int index = 0, typename ... T>
      constexpr
      int
      size(std::tuple<T...> const& t)
      {
        if constexpr(index == sizeof...(T))
                      return 1;
        else
          return std::get<index>(t) * size<index + 1>(t);
      }

      template <int dimension, typename I, typename T>
      auto
      _dimensions(I const& init, T& res)
      {
        if constexpr(dimension < std::tuple_size<T>::value)
                    {
                      std::get<dimension>(res) = init.size();
                      _dimensions<dimension + 1>(*begin(init), res);
                    }
      }

      template <int dimension, typename T>
      auto
      dimensions(T init)
      {
        typename elle::meta::repeat<int, dimension>::
          template apply<std::tuple> res;
        _dimensions<0>(init, res);
        return res;
      }

      template <typename T>
      using aligned =
        typename std::aligned_storage<sizeof(T), alignof(T)>::type;

      template <typename T, typename Index, int index>
      struct Initializer
      {
        static constexpr auto dimension = std::tuple_size<Index>::value;

        using type = std::initializer_list<
          typename Initializer<T, Index, index - 1>::type>;
        static
        void
        distribute(Index const& dimensions, type const& init, aligned<T>*& p)
        {
          if (init.size() != unsigned(std::get<dimension - index>(dimensions)))
            elle::err("wrong row size in Table initializer");
          for (auto& e: init)
            Initializer<T, Index, index - 1>::distribute(dimensions, e, p);
        }
      };

      template <typename T, typename Index>
      struct Initializer<T, Index, 0>
      {
        using type = T;
        static
        void
        distribute(Index const& dimensions, T const& e, aligned<T>*& p)
        {
          new (p) T(e);
          p += 1;
        }
      };
    }
  }

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>::TableImpl(Indexes ... dimensions)
    : TableImpl(std::make_tuple(std::forward<Indexes>(dimensions)...))
  {}

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>::TableImpl(std::tuple<Indexes...> dimensions)
    : TableImpl(std::move(dimensions), true)
  {
    for (auto i : boost::irange(0, this->size()))
      new (&this->_table[i]) T();
  }

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>::TableImpl(
    elle::meta::fold1<dimension, std::initializer_list, T> init)
    : TableImpl(_details::table::dimensions<dimension>(init), true)
  {
    Storage* p = &this->_table[0];
    try
    {
      _details::table::Initializer<T, Index, dimension>::distribute(
        this->_dimensions, std::move(init), p);
    }
    catch (...)
    {
      for (auto& e : elle::as_range(&this->_table[0], p))
      {
        try
        {
          reinterpret_cast<T&>(e).~T();
        }
        catch (...)
        {}
      }
      this->_table.reset();
      throw;
    }
  }

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>::~TableImpl()
  {
    if (this->_table)
      for (auto& e : elle::as_range(&this->_table[0],
                                    &this->_table[this->size()]))
        reinterpret_cast<T&>(e).~T();
  }

  template <typename T, typename ... Indexes>
  T&
  TableImpl<T, Indexes...>::at(Indexes const& ... indexes)
  {
    return this->at(Index(indexes...));
  }

  template <typename T, typename ... Indexes>
  T&
  TableImpl<T, Indexes...>::at(std::tuple<Indexes...> const& index)
  {
    if (!this->_check_boundaries(index, std::make_index_sequence<sizeof...(Indexes)>()))
      elle::err("{} is out on bounds ({})", index, this->_dimensions);
    // Don't just check index is in bound, because {2, 2} fits in {1, 100} but
    // is still out of bounds.
    auto const i = this->_index(index, std::make_index_sequence<sizeof...(Indexes)>());
    ELLE_ASSERT_LT(i, this->_size);
    return reinterpret_cast<T&>(this->_table[i]);
  }

  template <typename T, typename ... Indexes>
  T const&
  TableImpl<T, Indexes...>::at(Indexes const& ... indexes) const
  {
    return unconst(*this).at(indexes...);
  }

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>::TableImpl(std::tuple<Indexes...> dimensions, bool)
    : _size(_details::table::size(dimensions))
    , _dimensions(std::move(dimensions))
    , _table(new Storage[this->_size])
  {}

  template <typename T, typename ... Indexes>
  template <std::size_t ... S>
  bool
  TableImpl<T, Indexes...>::_check_boundaries(Index const& index, std::index_sequence<S...>)
  {
    return ((std::get<S>(index) < std::get<S>(this->_dimensions) &&
             std::get<S>(index) >= 0) && ...);
  }

  template <typename T, typename ... Indexes>
  template <std::size_t ... S>
  int
  TableImpl<T, Indexes...>::_index(Index const& index, std::index_sequence<S...>)
  {
    return sum((std::get<S>(index) * this->_index_offset(std::make_index_sequence<sizeof...(Indexes) - S - 1>()))...);
  }

  template <typename T, typename ... Indexes>
  template <std::size_t ... S>
  int
  TableImpl<T, Indexes...>::_index_offset(std::index_sequence<S...>)
  {
    return product(
      std::get<sizeof...(Indexes) - S - 1>(this->_dimensions)...);
  }
}
