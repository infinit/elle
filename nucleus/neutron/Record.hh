//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu apr  1 19:48:42 2010]
//

#ifndef NUCLEUS_NEUTRON_RECORD_HH
#define NUCLEUS_NEUTRON_RECORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an access control record, composed
    /// of the subject, its permissions and the token allowing the subject
    /// to access the data.
    ///
    class Record:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Record               Null;

      //
      // types
      //
      typedef Subject                   S;

      //
      // constructors & destructors
      //
      Record();
      Record(const Subject&,
             const Permissions&,
             const Token&);

      //
      // methods
      //
      elle::Status      Update(const Subject&,
                               const Permissions&,
                               elle::cryptography::SecretKey const&);
      elle::Status      Update(const Subject&,
                               const Permissions&,
                               const Token&);

      //
      // interfaces
      //

      // object
      declare(Record);
      elle::Boolean     operator==(const Record&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      // rangeable
      Subject&          Symbol();

      //
      // attributes
      //
      Subject           subject;
      Permissions       permissions;
      Token             token;
    };

  }
}

#endif
