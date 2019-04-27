#pragma once

#include <array>
#include <memory>

#include <elle/assert.hh>
#include <elle/attribute.hh>
#include <elle/err.hh>
#include <elle/math.hh>
#include <elle/meta.hh>
#include <elle/range.hh>
#include <elle/serialization/Serializer.hh>

namespace elle
{
  template <typename T, typename ... Indexes>
  class TableImpl
    : public Printable::as<TableImpl<T, Indexes...>>
  {
  /*------.
  | Types |
  `------*/
  public:
    static auto constexpr dimension = sizeof...(Indexes);
    // If we either decide on heterogeneous indexes, this can be a tuple
    // using Index = std::tuple<Indexes...>;
    using Index = std::array<int, dimension>;
    using Dimensions = std::array<int, dimension>;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// A Table with given \a dimensions and elements default-constructed.
    TableImpl(Indexes ... dimensions);
    /// A Table with given \a dimensions and elements default-constructed.
    TableImpl(Dimensions dimensions);
    /// A Table with dimension deduced and elements moved from \a init.
    TableImpl(elle::meta::fold1<dimension, std::initializer_list, T> init);
    /// Table \a src moved.
    TableImpl(TableImpl&& src);
    /// A copy of table \a src.
    TableImpl(TableImpl const& src);
    /// Dispose of a table.
    ~TableImpl() noexcept(noexcept(std::declval<T>().~T()));

  private:
    TableImpl(Dimensions dimensions, bool);

  /*-----------.
  | Dimensions |
  `-----------*/
  public:
    /// Number of elements.
    ELLE_ATTRIBUTE_R(int, size);
    /// Dimensions.
    ///
    /// When resizing, all elements that are still part of the table are
    /// moved. New elements are default constructed.
    ELLE_ATTRIBUTE_Rw(Dimensions, dimensions);

  /*-------.
  | Access |
  `-------*/
  public:
    /// Whether \a index is within the table boundaries.
    bool
    contains(Index const& index) const;
    using Access = decltype(std::declval<std::vector<T>&>()[0]);
    /// Get element at index (\a indexes...).
    Access
    at(Indexes const& ... indexes);
    /// Get element at index \a index.
    Access
    at(Index const& index);
    using CAccess = decltype(std::declval<std::vector<T> const&>()[0]);
    /// Get element at index (\a indexes...).
    CAccess
    at(Indexes const& ... indexes) const;
    /// Get element at index \a index.
    CAccess
    at(Index const& index) const;

  private:
    template <std::size_t ... S>
    bool
    _contains(Index const& index, std::index_sequence<S...>) const;

  /*------.
  | Print |
  `------*/
  public:
    void
    print(std::ostream& s) const;
  private:
    template <int I>
    void
    _print(std::ostream& s, typename std::vector<T>::const_iterator& it) const;

  /*---------.
  | Indexing |
  `---------*/
  public:
    /// A unique, implementation-defined integer different for each index.
    ///
    /// Result is contained within [0, this->size()[
    int
    index(Index const& index) const;
    /// The index corresponding to the implementation-defined integer.
    Index
    index(int index) const;

  private:
    template <std::size_t ... S>
    int
    _index(Index const& index, std::index_sequence<S...>) const;
    template <std::size_t ... S>
    Index
    _index(int index, std::index_sequence<S...>) const;
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
    template <typename It>
    class iterator_base;
    /// Constant iterator to (index, element) pairs.
    using const_iterator =
      iterator_base<typename std::vector<T>::const_iterator>;
    /// Iterator to (index, element) pairs.
    using iterator = iterator_base<typename std::vector<T>::iterator>;;
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
    elle::detail::range<typename std::vector<T>::iterator>
    elements();
    /// Iterate on elements only.
    elle::detail::range<typename std::vector<T>::const_iterator>
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

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    /// Serialization version.
    using serialization_tag = elle::serialization_tag;
    /// Deserialize.
    TableImpl(serialization::SerializerIn& s);
    /// Serialize.
    void
    serialize(serialization::Serializer& s);

  /*--------.
  | Details |
  `--------*/
  private:
    ELLE_ATTRIBUTE(std::vector<T>, table, protected);
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
