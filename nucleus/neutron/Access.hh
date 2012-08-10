#ifndef NUCLEUS_NEUTRON_ACCESS_HH
# define NUCLEUS_NEUTRON_ACCESS_HH

# include <elle/types.hh>
# include <elle/concept/Serializable.hh>
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
    class Access
      : public proton::ContentHashBlock
      , public elle::concept::Serializable<Access>
      , public elle::concept::Serializable<
            Access
          , elle::serialize::BufferArchive
        >
    {
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
                               Record*&) const;
      elle::Status      Lookup(const Subject&,
                               Index&) const; // XXX[rename to seek()]
      elle::Status      Lookup(const Index&,
                               Record*&) const;
      template <typename T>
      elle::Status      Update(const Subject&,
                               const Permissions&,
                               const T&);
      elle::Status      Consult(const Index&,
                                const Size&,
                                Range<Record>&) const;
      elle::Status      Upgrade(elle::cryptography::SecretKey const&);
      elle::Status      Downgrade();
      elle::Status      Remove(const Subject&);
      elle::Status      Capacity(Size&) const;
      elle::Status      Fingerprint(elle::cryptography::Digest&) const;

      //
      // interfaces
      //

      // object
      declare(Access);
      elle::Boolean     operator==(const Access&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // fileable
      ELLE_CONCEPT_SERIALIZABLE_METHODS(Access);
      ELLE_CONCEPT_SERIALIZABLE_METHODS(Access, elle::serialize::BufferArchive);

      //
      // attributes
      //
      Range<Record>     range;
    };

  }
}

# include <nucleus/neutron/Access.hxx>

#endif
