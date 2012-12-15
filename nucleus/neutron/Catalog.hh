#ifndef NUCLEUS_NEUTRON_CATALOG_HH
# define NUCLEUS_NEUTRON_CATALOG_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Entry.hh>
# include <nucleus/neutron/Size.hh>

namespace nucleus
{
  namespace neutron
  {
    ///
    /// this class represents the content of a directory and is composed
    /// of a set of tuples (name, address).
    ///
    /// note that the Catalog does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    /// XXX to rewrite
    ///
    class Catalog:
      public proton::Value,
      public elle::serialize::SerializableMixin<Catalog>
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<elle::String const, std::shared_ptr<Entry>> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static proton::Nature const seam;
        static proton::Nature const quill;
        static proton::Nature const value;
        static proton::Nature const nature;
      };

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Transfer a number of entries from the _left_ catalog to the _right_.
      /// The process stops when the _left_ catalog is left with _size_ bytes of
      /// entries i.e footprint.
      ///
      /// Note that the _right_ catalog is supposed to contain higher names
      /// so that the entries from the _left_ catalog with the highest names
      /// are moved to _right_ in order to maintain consistency.
      static
      void
      transfer_right(Catalog& left,
                     Catalog& right,
                     proton::Extent const size);
      /// Does the same as transfer_right() by moving entries from the _right_
      /// catalog to the _left_.
      static
      void
      transfer_left(Catalog& left,
                    Catalog& right,
                    proton::Extent const size);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Catalog();
      Catalog(Catalog const&) = default;

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Insert the given entry in the catalog.
      ///
      /// Note that the ownership of the given entry is transferred to the
      /// catalog which will therefore be responsible for deleting it.
      void
      insert(Entry* entry);
      /// Insert the given entry and updates accordingly the catalog's
      /// capacity and state.
      void
      insert(std::shared_ptr<Entry> const& entry);
      /// Return true if the catalog contains an entry with the given name.
      elle::Boolean
      exist(elle::String const& name) const;
      /// Rename an entry.
      void
      rename(elle::String const& from,
             elle::String const& to);
      /// Return the entry associated with the given name.
      Entry const&
      locate(elle::String const& name) const;
      /// Return the entry associated with the given name.
      Entry&
      locate(elle::String const& name);
      /// Return a subset of the catalog i.e [index, index + size[.
      Range<Entry>
      consult(Index const& index,
              Size const& size) const;
      /// Remove and release the entry associated with the given name from
      /// the catalog.
      void
      erase(elle::String const& name);
      /// Remove the entry associated with the given name and return the entry.
      ///
      /// This method is especially useful when moving entries between catalogs.
      std::shared_ptr<Entry>
      takeout(elle::String const& name);
      /// Return the size of the catalog i.e the number of entries.
      Size
      size() const;
    private:
      /// Take the give entry and inject it into the catalog's container.
      ///
      /// Note that this method does not update the other catalog attributes
      /// (except the footprint) so as to be used by the deserializer as well.
      void
      _inject(std::shared_ptr<Entry> const& entry);
      /// Return an iterator on the entry associated with the given name.
      Scoutor
      _iterator(elle::String const& name) const;
      /// Return an iterator on the entry associated with the given name.
      Iterator
      _iterator(elle::String const& name);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Catalog);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // value
      typedef elle::String K;
      elle::Boolean
      empty() const;
      elle::String
      mayor() const;
      proton::Handle
      split();
      void
      merge(proton::Handle& other);
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Catalog);
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Catalog);
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;
      Iterator
      begin();
      Iterator
      end();

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Container, container);
    };

  }
}

# include <nucleus/neutron/Catalog.hxx>

#endif
