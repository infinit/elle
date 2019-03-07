#pragma once

#include <memory>

#include <elle/assert.hh>
#include <elle/attribute.hh>
#include <elle/err.hh>
#include <elle/math.hh>
#include <elle/meta.hh>

namespace elle
{
  template <typename T, typename ... Indexes>
  class TableImpl
  {
  public:
    using Index = std::tuple<Indexes...>;

    TableImpl(Indexes ... dimensions)
      : _size((product(dimensions...)))
      , _dimensions(std::forward<Indexes>(dimensions)...)
      , _table(new T[this->_size])
    {}

    T&
    at(Indexes const& ... indexes)
    {
      return this->at(Index(indexes...));
    }

    T&
    at(std::tuple<Indexes...> const& index)
    {
      if (!this->_check_boundaries(index, std::make_index_sequence<sizeof...(Indexes)>()))
        elle::err("{} is out on bounds ({})", index, this->_dimensions);
      // Don't just check index is in bound, because {2, 2} fits in {1, 100} but
      // is still out of bounds.
      auto const i = this->_index(index, std::make_index_sequence<sizeof...(Indexes)>());
      ELLE_ASSERT_LT(i, this->_size);
      return this->_table[i];
    }

    T const&
    at(Indexes const& ... indexes) const
    {
      return unconst(*this).at(indexes...);
    }

  private:
    template <std::size_t ... S>
    bool
    _check_boundaries(Index const& index, std::index_sequence<S...>)
    {
      return ((std::get<S>(index) < std::get<S>(this->_dimensions) &&
               std::get<S>(index) >= 0) && ...);
    }

    template <std::size_t ... S>
    int
    _index(Index const& index, std::index_sequence<S...>)
    {
      return sum((std::get<S>(index) * this->_index_offset(std::make_index_sequence<sizeof...(Indexes) - S - 1>()))...);
    }

    template <std::size_t ... S>
    int
    _index_offset(std::index_sequence<S...>)
    {
      return product(
        std::get<sizeof...(Indexes) - S - 1>(this->_dimensions)...);
    }

    ELLE_ATTRIBUTE_R(int, size);
    ELLE_ATTRIBUTE_R(std::tuple<Indexes...>, dimensions);
    ELLE_ATTRIBUTE(std::unique_ptr<T[]>, table);
  };

  template <typename T, int dimension, typename ... Indexes>
  class Table:
    public elle::meta::repeat<int, dimension>::template apply<TableImpl, T>
  {
    using elle::meta::repeat<int, dimension>::
    template apply<TableImpl, T>::TableImpl;
  };
}
