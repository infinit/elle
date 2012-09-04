#ifndef NUCLEUS_NEUTRON_ACCESS_HH
# define NUCLEUS_NEUTRON_ACCESS_HH

# include <elle/types.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/serialize/BufferArchive.hh>

# include <nucleus/proton/ContentHashBlock.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Record.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    /// This class represents an access control list.
    class Access:
      public proton::ContentHashBlock,
      public elle::serialize::SerializableMixin<Access>,
      public elle::serialize::SerializableMixin<
        Access,
        elle::serialize::BufferArchive
      >,
      public elle::concept::MakeUniquable<Access>
    {
      //
      // constants
      //
    public:
      static const Component component;

      //
      // constants
      //
    public:
      static const Access               Null;

      //
      // construction
      //
    public:
      Access();

      //
      // methods
      //
      elle::Status      Add(Record*);
      elle::Status      Exist(const Subject&) const;
      elle::Status      Lookup(const Subject&,
                               Record const*&) const;
      elle::Status      Lookup(const Subject&,
                               Record*&) const;
      elle::Status      Lookup(const Subject&,
                               Index&) const; // XXX[rename to seek()]
      elle::Status      Lookup(const Index&,
                               Record*&) const;
      /// XXX
      template <typename T>
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             T const& secret,
             elle::cryptography::PublicKey const& K);
      /// XXX
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             Token const& token);
      elle::Status      Consult(const Index&,
                                const Size&,
                                Range<Record>&) const;
      elle::Status      Remove(const Subject&);
      elle::Status      Capacity(Size&) const;
      elle::Status      Fingerprint(elle::cryptography::Digest&) const;

      /// XXX
      typename Range<Record>::Scoutor
      begin() const;
      /// XXX
      typename Range<Record>::Scoutor
      end() const;

      //
      // interfaces
      //

      // object
      declare(Access);
      elle::Boolean     operator==(const Access&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // fileable
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Access);
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Access, elle::serialize::BufferArchive);

      //
      // attributes
      //
      Range<Record>     range;
    };

  }
}

# include <nucleus/neutron/Access.hxx>

#endif
