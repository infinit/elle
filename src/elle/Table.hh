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

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// A Table with given \a dimensions and elements default-constructed.
    TableImpl(Indexes ... dimensions);
    /// A Table with given \a dimensions and elements default-constructed.
    TableImpl(std::tuple<Indexes...> dimensions);
    /// A Table with dimension deduced and elements moved from \a init.
    TableImpl(elle::meta::fold1<dimension, std::initializer_list, T> init);
    /// Table \a src moved.
    TableImpl(TableImpl&& src);
    /// A copy of table \a src.
    TableImpl(TableImpl const& src);
    /// Dispose of a table.
    ~TableImpl() noexcept(noexcept(std::declval<T>().~T()));

  private:
    TableImpl(std::tuple<Indexes...> dimensions, bool);

  /*-----------.
  | Dimensions |
  `-----------*/
  public:
    /// Number of elements.
    ELLE_ATTRIBUTE_R(int, size);
    /// Dimensions.
    ELLE_ATTRIBUTE_R(std::tuple<Indexes...>, dimensions);

  /*-------.
  | Access |
  `-------*/
  public:
    /// Get element at index (\a indexes...).
    T&
    at(Indexes const& ... indexes);
    /// Get element at index \a index.
    T&
    at(Index const& index);
    /// Get element at index (\a indexes...).
    T const&
    at(Indexes const& ... indexes) const;
    /// Get element at index \a index.
    T const&
    at(Index const& index) const;

  private:
    template <std::size_t ... S>
    bool
    _check_boundaries(Index const& index, std::index_sequence<S...>);

  /*---------.
  | Indexing |
  `---------*/
  public:
    /// A unique, implementation-defined integer different for each index.
    int
    index(Index const& index) const;

  private:
    template <std::size_t ... S>
    int
    _index(Index const& index, std::index_sequence<S...>) const;
    template <std::size_t ... S>
    int
    _index_offset(std::index_sequence<S...>) const;

  /*-------------.
  | Modification |
  `-------------*/
  public:
    /// Replace this content with a copy of \a table's content.
    TableImpl&
    operator =(TableImpl const& table);

  /*----------.
  | Iteration |
  `----------*/
  public:
    /// Base iterator.
    template <template <typename> class Const>
    class iterator_base;
    /// Constant iterator to (index, element) pairs.
    using const_iterator = iterator_base<std::add_const_t>;
    /// Iterator to (index, element) pairs.
    using iterator = iterator_base<std::identity_t>;;
    /// Beginning iterator to (index, element) pairs.
    iterator
    begin();
    /// Beginning cosntent iterator to (index, element) pairs.
    const_iterator
    begin() const;
    /// End iterator to (index, element) pairs.
    iterator
    end();
    /// End constant iterator to (index, element) pairs.
    const_iterator
    end() const;
    /// Iterate on elements only.
    elle::detail::range<T*>
    elements();
    /// Iterate on elements only.
    elle::detail::range<T const*>
    elements() const;

  private:
    elle::detail::range<T*>
    _range();
    elle::detail::range<T const*>
    _range() const;

  /*-----------.
  | Comparison |
  `-----------*/
  public:
    /// Whether two tables are identical.
    bool
    operator ==(TableImpl const& table) const;
    /// Whether two tables are different.
    bool
    operator !=(TableImpl const& table) const;

  /*--------.
  | Details |
  `--------*/
  private:
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
