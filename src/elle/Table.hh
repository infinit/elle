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
  // Required so we can bounce to ADL definitions of get(<tuple_like>).
  template <std::size_t I, typename T>
  auto
  get(T&& v)
    -> decltype(std::get<I>(std::forward<T>(v)))
  {
    return std::get<I>(std::forward<T>(v));
  }

  template <typename T, std::size_t S>
  struct array_like
    : public std::array<T, S>
  {
    using Super = std::array<T, S>;
    using Super::Super;

    template <typename I>
    array_like(I&& i)
      : array_like(std::forward<I>(i), std::make_index_sequence<S>())
    {}

    template <typename I, std::size_t ... Indices>
    array_like(I&& i, std::integer_sequence<std::size_t, Indices...> const&)
      : Super{get<Indices>(std::forward<I>(i))...}
    {}

    array_like(std::initializer_list<T> init)
      : array_like(
        std::move(init),
        (ELLE_ASSERT(init.size() == S), std::make_index_sequence<S>()))
    {}

    template <std::size_t ... Indices>
    array_like(std::initializer_list<T> init,
               std::integer_sequence<std::size_t, Indices...> const&)
      : Super{(*(std::begin(init) + Indices))...}
    {}
  };

  template <typename T, typename DefaultConstructible, typename ... Indexes>
  class TableImpl
    : public Printable::as<TableImpl<T, DefaultConstructible, Indexes...>>
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
    TableImpl(Indexes ... dimensions, T value);
    /// A Table with given \a dimensions and elements default-constructed.
    TableImpl(Dimensions dimensions, T value);
    /// A Table with dimension deduced and elements moved from \a init.
    TableImpl(elle::meta::fold1<dimension, std::initializer_list, T> init);
    /// Table \a src moved.
    TableImpl(TableImpl&& src);
    /// A copy of table \a src.
    TableImpl(TableImpl const& src);
    /// Dispose of a table.
    ~TableImpl() noexcept(noexcept(std::declval<T>().~T()));

  private:
    struct no_init;
    TableImpl(Dimensions dimensions, no_init);

  /*-----------.
  | Dimensions |
  `-----------*/
  public:
    /// Number of elements.
    ELLE_ATTRIBUTE_R(int, size);
    /// Dimensions.
    ELLE_ATTRIBUTE_R(Dimensions, dimensions);
  public:
    /// Resize, copying \a value into potential new cells.
    void
    dimensions(Dimensions dimensions, T value);

  /*-------.
  | Access |
  `-------*/
  public:
    /// Whether \a index is within the table boundaries.
    bool
    contains(array_like<int, dimension> index) const;
    using Access = decltype(std::declval<std::vector<T>&>()[0]);
    /// Get element at index (\a indexes...).
    Access
    at(Indexes const& ... indexes);
    /// Get element at index \a index.
    Access
    at(array_like<int, dimension> index);
    using CAccess = decltype(std::declval<std::vector<T> const&>()[0]);
    /// Get element at index (\a indexes...).
    CAccess
    at(Indexes const& ... indexes) const;
    /// Get element at index \a index.
    CAccess
    at(array_like<int, dimension> index) const;

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
    index(array_like<int, dimension> index) const;
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

  template <typename T, typename ... Indexes>
  class TableImpl<T, std::true_type, Indexes ...>
    : public TableImpl<T, std::false_type, Indexes ...>
  {
  public:
    using Super = TableImpl<T, std::false_type, Indexes ...>;
    using Dimensions = typename Super::Dimensions;
    using Super::Super;
    /// A Table with given \a dimensions and elements default-constructed.
    TableImpl(Indexes ... dimensions);
    /// A Table with given \a dimensions and elements default-constructed.
    TableImpl(Dimensions dimensions);
    /// When resizing, all elements that are still part of the table are
    /// moved. New elements are default constructed.
    void
    dimensions(Dimensions d);
    using Super::dimensions;
  };

  template <typename T, int dimension>
  class Table:
    public elle::meta::repeat<int, dimension>::template
    apply<TableImpl, T, typename std::is_default_constructible<T>::type>
  {
  public:
    using Super = typename elle::meta::repeat<int, dimension>:: template apply<
      TableImpl,
      T,
      typename std::is_default_constructible<T>::type>;
    using Super::Super;
  };
}

#include <elle/Table.hxx>
