#ifndef NUCLEUS_NEUTRON_ACCESS_HH
# define NUCLEUS_NEUTRON_ACCESS_HH

# include <elle/types.hh>
# include <elle/concept/Fileable.hh>

# include <nucleus/proton/ContentHashBlock.hh>

# include <nucleus/neutron/Index.hh>
# include <nucleus/neutron/Size.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Subject.hh>
# include <nucleus/neutron/Record.hh>

namespace nucleus
{
  namespace neutron
  {

    /// This class represents an access control list.
    class Access
      : public proton::ContentHashBlock
      , public elle::concept::Fileable<Access>
    {
    public:
      typedef proton::ContentHashBlock SuperClass;

    public:
      ELLE_CONCEPT_FILEABLE_METHODS(Access);

      static const Access               Null;

      Access();

      //
      // methods
      //
      elle::Status      Add(Record*);
      elle::Status      Exist(const Subject&) const;
      elle::Status      Lookup(const Subject&,
                               Record*&) const;
      elle::Status      Lookup(const Subject&,
                               Index&) const;
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

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Range<Record>     range;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Access.hxx>

#endif
