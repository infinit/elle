#ifndef NUCLEUS_NEUTRON_RECORD_HH
# define NUCLEUS_NEUTRON_RECORD_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/neutron/Subject.hh>
# include <nucleus/neutron/Permissions.hh>
# include <nucleus/neutron/Token.hh>

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
      typedef Subject                   Symbol;

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
      /// XXX
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             elle::cryptography::SecretKey const& secret,
             elle::cryptography::PublicKey const& K);
      /// XXX
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             Token const& token);

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
      Subject&
      symbol();

      //
      // attributes
      //
      Subject subject;
      Permissions permissions;
      Token token;
    };

  }
}

# include <nucleus/neutron/Record.hxx>

#endif
