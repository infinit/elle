#pragma once

#include <memory>

#include <elle/assert.hh>
#include <elle/attribute.hh>
#include <elle/err.hh>
#include <elle/math.hh>
#include <elle/meta.hh>
#include <elle/range.hh>

namespace elle
{
  template <typename T, typename ... Indexes>
  class TableImpl
  {
  public:
    using Index = std::tuple<Indexes...>;
    using Storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    static auto constexpr dimension = sizeof...(Indexes);

    TableImpl(Indexes ... dimensions);
    TableImpl(std::tuple<Indexes...> dimensions);
    TableImpl(elle::meta::fold1<dimension, std::initializer_list, T> init);
    TableImpl(TableImpl&&);
    TableImpl(TableImpl const&);
    ~TableImpl() noexcept(noexcept(std::declval<T>().~T()));

    T&
    at(Indexes const& ... indexes);

    T&
    at(std::tuple<Indexes...> const& index);

    T const&
    at(Indexes const& ... indexes) const;

    template <template <typename> class Const>
    class iterator_base;
    using const_iterator = iterator_base<std::add_const_t>;
    using iterator = iterator_base<std::identity_t>;;

    iterator
    begin();
    const_iterator
    begin() const;
    iterator
    end();
    const_iterator
    end() const;

  private:
    TableImpl(std::tuple<Indexes...> dimensions, bool);

    template <std::size_t ... S>
    bool
    _check_boundaries(Index const& index, std::index_sequence<S...>);
    template <std::size_t ... S>

    int
    _index(Index const& index, std::index_sequence<S...>) const;
    template <std::size_t ... S>
    int
    _index_offset(std::index_sequence<S...>) const;

    ELLE_ATTRIBUTE_R(int, size);
    ELLE_ATTRIBUTE_R(std::tuple<Indexes...>, dimensions);
    ELLE_ATTRIBUTE(std::unique_ptr<Storage[]>, table, protected);
  };

  template <typename T, int dimension>
  class Table:
    public elle::meta::repeat<int, dimension>::template apply<TableImpl, T>
  {
  public:
    using Super = typename elle::meta::repeat<int, dimension>::
      template apply<TableImpl, T>::TableImpl;
    using Super::Super;
  };
}

#include <elle/Table.hxx>
