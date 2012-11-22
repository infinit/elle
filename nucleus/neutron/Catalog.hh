#ifndef NUCLEUS_NEUTRON_CATALOG_HH
# define NUCLEUS_NEUTRON_CATALOG_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/proton/Node.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Entry.hh>
# include <nucleus/neutron/Size.hh>

# include <boost/noncopyable.hpp>

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
      public elle::serialize::SerializableMixin<Catalog>,
      private boost::noncopyable
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef elle::String K;

      typedef std::map<elle::String const, std::shared_ptr<Entry>> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static const proton::Node::Type seam;
        static const proton::Node::Type quill;
        static const proton::Node::Type value;
        static const proton::Node::Type type;
      };

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// XXX
      static
      proton::Footprint
      compute_initial_footprint();
      /// XXX
      static
      void
      transfer_right(Catalog* left,
                     Catalog* right,
                     proton::Extent const size);
      /// XXX
      static
      void
      transfer_left(Catalog* left,
                    Catalog* right,
                    proton::Extent const size);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Catalog();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// XXX[ownership transfered]
      void
      insert(Entry* entry);
      /// XXX
      elle::Boolean
      exist(elle::String const& name) const;
      /// XXX
      Entry const&
      locate(elle::String const& name) const;
      /// XXX
      Entry&
      locate(elle::String const& name);
      /// XXX
      Range<Entry>
      consult(Index const& index,
              Size const& size) const;
      /// XXX
      void
      erase(elle::String const& name);
      /// XXX
      Size
      size() const;
      /// XXX
      proton::Handle
      split();
      /// XXX
      void
      merge(proton::Handle& handle);
    private:
      /// XXX
      void
      _insert(std::shared_ptr<Entry> const& entry);
      /// XXX
      Scoutor
      _iterator(elle::String const& name) const;
      /// XXX
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
      elle::Boolean
      empty() const;
      elle::String
      mayor() const;
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
