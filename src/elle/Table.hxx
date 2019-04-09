#include <type_traits>

#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/zip.hpp>

#include <boost/range/irange.hpp>

#include <elle/log.hh>
#include <elle/utils.hh>

namespace elle
{
  /*--------.
  | Helpers |
  `--------*/

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

  /*-------------.
  | Construction |
  `-------------*/

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
  TableImpl<T, Indexes...>::TableImpl(TableImpl&& src)
    : _size(src._size)
    , _dimensions(src._dimensions)
    , _table(std::move(src._table))
  {}

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>::TableImpl(TableImpl const& src)
    : TableImpl(src._dimensions, true)
  {
    Storage* p;
    try
    {
      for (p = &this->_table[0]; p < &this->_table[this->_size]; ++p)
        new (p) T(reinterpret_cast<T const&>(src._table[p - &this->_table[0]]));
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
  noexcept(noexcept(std::declval<T>().~T()))
  {
    ELLE_LOG_COMPONENT("elle.Table");
    std::exception_ptr ptr;
    if (this->_table)
      for (auto& e : this->_range())
        try
        {
          reinterpret_cast<T&>(e).~T();
        }
        catch (...)
        {
          if (!ptr)
            ptr = std::current_exception();
          else
            ELLE_WARN("ignoring additional exception destroying {}: {}",
                      this, elle::exception_string());
        }
    if (ptr)
      std::rethrow_exception(ptr);
  }

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>::TableImpl(std::tuple<Indexes...> dimensions, bool)
    : _size(_details::table::size(dimensions))
    , _dimensions(std::move(dimensions))
    , _table(new Storage[this->_size])
  {}

  /*-------.
  | Access |
  `-------*/

  template <typename T, typename ... Indexes>
  T&
  TableImpl<T, Indexes...>::at(Indexes const& ... indexes)
  {
    return this->at(Index(indexes...));
  }

  template <typename T, typename ... Indexes>
  T&
  TableImpl<T, Indexes...>::at(Index const& index)
  {
    if (!this->contains(index))
      elle::err("{} is out of bounds {}", index, this->_dimensions);
    // Don't just check index is in bound, because {2, 2} fits in {1, 100} but
    // is still out of bounds.
    auto const i = this->index(index);
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
  T const&
  TableImpl<T, Indexes...>::at(Index const& index) const
  {
    return unconst(*this).at(index);
  }

  template <typename T, typename ... Indexes>
  bool
  TableImpl<T, Indexes...>::contains(Index const& index) const
  {
    return this->_contains(
      index, std::make_index_sequence<sizeof...(Indexes)>());
  }


  template <typename T, typename ... Indexes>
  template <std::size_t ... S>
  bool
  TableImpl<T, Indexes...>::_contains(Index const& index,
                                              std::index_sequence<S...>) const
  {
    return ((std::get<S>(index) < std::get<S>(this->_dimensions) &&
             std::get<S>(index) >= 0) && ...);
  }

  /*---------.
  | Indexing |
  `---------*/

  template <typename T, typename ... Indexes>
  int
  TableImpl<T, Indexes...>::index(Index const& index) const
  {
    return this->_index(index, std::make_index_sequence<sizeof...(Indexes)>());
  }

  template <typename T, typename ... Indexes>
  template <std::size_t ... S>
  int
  TableImpl<T, Indexes...>::_index(
    Index const& index, std::index_sequence<S...>) const
  {
    return sum(
      (std::get<S>(index) *
       this->_index_offset(
         std::make_index_sequence<sizeof...(Indexes) - S - 1>()))...);
  }

  template <typename T, typename ... Indexes>
  template <std::size_t ... S>
  int
  TableImpl<T, Indexes...>::_index_offset(std::index_sequence<S...>) const
  {
    return product(
      std::get<sizeof...(Indexes) - S - 1>(this->_dimensions)...);
  }

  /*-------------.
  | Modification |
  `-------------*/

  template <typename T, typename ... Indexes>
  TableImpl<T, Indexes...>&
  TableImpl<T, Indexes...>::operator =(TableImpl const& table)
  {
    this->~TableImpl();
    new (this) TableImpl(table);
    return *this;
  }

  /*----------.
  | Iteration |
  `----------*/

  template <typename T, typename ... Indexes>
  typename TableImpl<T, Indexes...>::iterator
  TableImpl<T, Indexes...>::begin()
  {
    return iterator(*this, reinterpret_cast<T*>(&this->_table[0]));
  }

  template <typename T, typename ... Indexes>
  typename TableImpl<T, Indexes...>::iterator
  TableImpl<T, Indexes...>::end()
  {
    return this->begin() + this->size();
  }

  template <typename T, typename ... Indexes>
  typename TableImpl<T, Indexes...>::const_iterator
  TableImpl<T, Indexes...>::begin() const
  {
    return const_iterator(*this, reinterpret_cast<T*>(&this->_table[0]));
  }

  template <typename T, typename ... Indexes>
  typename TableImpl<T, Indexes...>::const_iterator
  TableImpl<T, Indexes...>::end() const
  {
    return this->begin() + this->size();
  }

  template <typename T, typename ... Indexes>
  template <template <typename> class Const>
  struct TableImpl<T, Indexes...>::iterator_base
  {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::pair<Index, T>;
    using difference_type = int;
    using pointer = std::pair<Index, Const<T>*>;
    using reference = std::pair<Index, Const<T>&>;

    friend class TableImpl;

    bool
    operator !=(iterator_base rhs)
    {
      return this->_iterator != rhs._iterator;
    }

    iterator_base
    operator ++()
    {
      ++this->_iterator;
      return *this;
    }

    iterator_base
    operator +(int o)
    {
      return {this->_table, this->_iterator + o};
    }

    reference
    operator *()
    {
      auto offset =
        this->_iterator - reinterpret_cast<Const<T>*>(&this->_table._table[0]);
      return {
        this->_index(offset, std::make_index_sequence<sizeof...(Indexes)>()),
        *this->_iterator,
      };
    }

  private:
    iterator_base(Const<TableImpl>& table, Const<T>* iterator)
      : _table(table)
      , _iterator(iterator)
    {}

    template <std::size_t ... I>
    TableImpl::Index
    _index(int offset, std::index_sequence<I...>)
    {
      auto& t = this->_table;
      return {
        offset %
        t._index_offset(std::make_index_sequence<sizeof...(Indexes) - I>()) /
        t._index_offset(std::make_index_sequence<sizeof...(Indexes) - I - 1>())
        ...
      };
    }

    ELLE_ATTRIBUTE((Const<TableImpl>&), table);
    ELLE_ATTRIBUTE(Const<T>*, iterator);
  };

  template <typename T, typename ... Indexes>
  elle::detail::range<T*>
  TableImpl<T, Indexes...>::elements()
  {
    auto p = reinterpret_cast<T*>(&this->_table[0]);
    return elle::as_range(p, p + this->size());
  }

  template <typename T, typename ... Indexes>
  elle::detail::range<T const*>
  TableImpl<T, Indexes...>::elements() const
  {
    auto p = reinterpret_cast<T const*>(&this->_table[0]);
    return elle::as_range(p, p + this->size());
  }

  template <typename T, typename ... Indexes>
  elle::detail::range<T*>
  TableImpl<T, Indexes...>::_range()
  {
    return elle::as_range(
      reinterpret_cast<T*>(&this->_table[0]),
      reinterpret_cast<T*>(&this->_table[this->size()]));
  }

  template <typename T, typename ... Indexes>
  elle::detail::range<T const*>
  TableImpl<T, Indexes...>::_range() const
  {
    return elle::as_range(
      reinterpret_cast<T const*>(&this->_table[0]),
      reinterpret_cast<T const*>(&this->_table[this->size()]));
  }

  /*-----------.
  | Comparison |
  `-----------*/

  template <typename T, typename ... Indexes>
  bool
  TableImpl<T, Indexes...>::operator ==(TableImpl const& rhs) const
  {
    if (this->dimensions() != rhs.dimensions())
      return false;
    return
      ranges::accumulate(
        ranges::view::zip_with([] (T const& a, T const& b) { return a == b; },
                               this->_range(), rhs._range()),
        true, [] (bool a, bool b) { return a && b; });
  }

  template <typename T, typename ... Indexes>
  bool
  TableImpl<T, Indexes...>::operator !=(TableImpl const& table) const
  {
    return !(*this == table);
  }
}
