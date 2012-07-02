#ifndef NUCLEUS_NEUTRON_RECORD_HH
#define NUCLEUS_NEUTRON_RECORD_HH

#include <elle/types.hh>

#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {

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
#include <elle/idiom/Open.hh>
      declare(Record);
#include <elle/idiom/Close.hh>
      elle::Boolean     operator==(const Record&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

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

# include <nucleus/neutron/Record.hxx>

#endif
