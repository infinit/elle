#ifndef NUCLEUS_NEUTRON_OBJECT_HH
# define NUCLEUS_NEUTRON_OBJECT_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/ImprintBlock.hh>
# include <nucleus/proton/Version.hh>

# include <nucleus/neutron/Genre.hh>
# include <nucleus/neutron/Author.hh>
# include <nucleus/neutron/Size.hh>
# include <nucleus/neutron/Permissions.hh>
# include <nucleus/neutron/Token.hh>
# include <nucleus/neutron/Attributes.hh>
# include <nucleus/neutron/Access.hh>
# include <nucleus/neutron/Role.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is the most important of the whole Infinit project
    /// as it describes file system objects being files, directories and
    /// references.
    ///
    /// an object can rely on several physical constructors such as an
    /// ImprintBlock, an OwnerKeyBlock or else.
    ///
    /// basically, an object contains the author i.e the user having performed
    /// the latest modification along with the address of the Access block
    /// which contains the set of authorised users and groups. note however
    /// than most objects do not reference an access block in which cases
    /// the object is considered private i.e only its owner has access.
    ///
    /// in addition, several meta information are contained such as the
    /// owner permissions, some stamps, the attributes etc.
    ///
    /// finally, the data section contains the address of the object's
    /// contents though the nature of the contents depends upon the
    /// object's nature: file, directory or reference.
    ///
    /// noteworthy is tha meta.owner.record is generated in order to
    /// make it as easy to manipulate the owner entry as for other access
    /// records. thus, this attribute is never serialized.
    ///
    class Object:
      public proton::ImprintBlock
    {
    public:
      //
      // constructors & destructors
      //
      Object();

      //
      // methods
      //
      elle::Status      Create(const Genre,
                               elle::cryptography::PublicKey const&);

      elle::Status      Update(const Author&,
                               const proton::Address&,
                               const Size&,
                               const proton::Address&,
                               const Token&);
      elle::Status      Administrate(const Attributes&,
                                     const Permissions&);

      elle::Status      Seal(elle::cryptography::PrivateKey const&,
                             const Access&);

      elle::Status      Validate(const proton::Address&) const;
      elle::Status      Validate(const proton::Address&,
                                 const Access&) const;

      //
      // interfaces
      //

      // object
#include <elle/idiom/Open.hh>
      declare(Object);
#include <elle/idiom/Close.hh>

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Author                    author;

      struct
      {
        // XXX to implement: proton::Base               base;

        struct
        {
          Permissions           permissions;
          Token                 token;

          Record                record;
        }                       owner;

        Genre                   genre;
        elle::utility::Time              stamp;

        Attributes              attributes;

        proton::Address         access;

        proton::Version         version;
        elle::cryptography::Signature         signature;

        proton::State           state;
      }                         meta;

      struct
      {
        // XXX to implement: proton::Base               base;

        proton::Address         contents;

        Size                    size;
        elle::utility::Time              stamp;

        proton::Version         version;
        elle::cryptography::Signature         signature;

        proton::State           state;
      }                         data;
    };

  }
}

#endif
