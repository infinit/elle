#ifndef NUCLEUS_NEUTRON_CATALOG_HH
# define NUCLEUS_NEUTRON_CATALOG_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/proton/Node.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Entry.hh>

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
      public elle::Printable,
      private boost::noncopyable
    {
    public:
      //
      // types
      //
      typedef elle::String K;

      //
      // constants
      //
    public:
      struct Constants
      {
        static const proton::Node::Type seam;
        static const proton::Node::Type quill;
        static const proton::Node::Type value;
        static const proton::Node::Type type;
      };

      //
      // types
      //
      typedef std::map<elle::String const, Entry*> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      //
      // static methods
      //
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

      //
      // constructors & destructors
      //
    public:
      Catalog();
      ~Catalog();

      //
      // methods
      //
    public:
      /// XXX
      void
      insert(Entry* entry);
      /// XXX
      elle::Boolean
      exists(elle::String const& name) const;
      /// XXX
      Scoutor
      locate_iterator(elle::String const& name) const;
      /// XXX
      Iterator
      locate_iterator(elle::String const& name);
      /// XXX
      Entry const*
      locate_entry(elle::String const& name) const;
      /// XXX
      Range<Entry>
      consult(Index const index,
              Size const size) const;
      /// XXX
      void
      erase(elle::String const& name);
      /// XXX
      proton::Handle
      split();
      /// XXX
      void
      merge(proton::Handle& handle);
      /// XXX[changer et fournir un begin() et end() plutot]
      Container&
      container();

      //
      // interfaces
      //
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

      //
      // attributes
      //
    private:
      Container  _container;
    };

  }
}

# include <nucleus/neutron/Catalog.hxx>

#endif
