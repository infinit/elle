#ifndef NUCLEUS_NEUTRON_RECORD_HH
# define NUCLEUS_NEUTRON_RECORD_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

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
      public elle::Printable
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
      Record(Subject const& subject,
             Permissions permissions,
             Token const& token);
      Record(Subject const& subject,
             Permissions permissions,
             elle::cryptography::SecretKey const& secret,
             elle::cryptography::PublicKey const& K);

      //
      // methods
      //
      /// XXX @deprecated
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             elle::cryptography::SecretKey const& secret,
             elle::cryptography::PublicKey const& K);
      /// XXX @deprecated
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             Token const& token);

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Record const& other) const;
      ELLE_OPERATOR_NEQ(Record);
      ELLE_OPERATOR_ASSIGNMENT(Record);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // rangeable
      virtual
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
