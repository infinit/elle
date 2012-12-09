#ifndef NUCLEUS_NEUTRON_RANGE_HH
# define NUCLEUS_NEUTRON_RANGE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/serialize/Serializable.hh>

# include <nucleus/neutron/fwd.hh>

# include <vector>
# include <memory>

namespace nucleus
{
  namespace neutron
  {
    /// Represent a set of items from a container-based nucleus class
    /// such as Catalog, Access, Ensemble etc.
    ///
    /// Whenever such a class is consulted, a range is returned contained
    /// some or all of the items requested.
    ///
    /// A range must be parameterised with a type providing two things:
    ///
    ///  1) A type T::Symbol which defines the key type used to differenciate
    ///     items.
    ///  2) A symbol() method which must returns a T::Symbol const&, this method
    ///     being used to retrieve the key value of a given item.
    ///
    /// Note that since range represent a subset of another container-based
    /// instance, it is preferable to directly reference the items within the
    /// original container so as to avoid copying them all. However, since one
    /// may want to use a range after its original instance has been destroyed,
    /// ranges make use of shared pointers.
    ///
    /// It is important to understand that the class Range has not been designed
    /// for locating a precise element but rather to iterate over it. Bear in
    /// mind that any locate operation is performed in O(n).
    template <typename T>
    class Range:
      public elle::Printable
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef typename T::Symbol Symbol;

      typedef std::vector<std::shared_ptr<T const>> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Range();
      /// Construct a range with the given initialize size.
      explicit
      Range(nucleus::neutron::Size const size);
      Range(Range<T> const& other);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Insert an item to the range.
      void
      insert(std::shared_ptr<T const> const& item);
      /// Add the _other_'s elements into the current range i.e perform
      /// a merging process.
      void
      add(Range<T> const& other);
      /// Return true if the given symbol exists.
      elle::Boolean
      exist(Symbol const& symbol) const;
      /// Return the item associated with the given symbol.
      T const&
      locate(Symbol const& symbol) const;
      /// Erase the item associated with the given symbol.
      void
      erase(Symbol const& symbol);
      /// Return the size of the range.
      elle::Size
      size() const;
    private:
      /// Return an iterator on the given symbol's item.
      Scoutor
      _iterator(Symbol const& symbol) const;
      /// Return an iterator on the given symbol's item.
      Iterator
      _iterator(Symbol const& symbol);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_ASSIGNMENT(Range<T>); // XXX

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Range<T>);
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Container, container);
    };
  }
}

# include <nucleus/neutron/Range.hxx>

#endif
