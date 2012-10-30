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

    /// This class represents an access control record, composed
    /// of the subject, its permissions and the token allowing the subject
    /// to access the data.
    ///
    /// Note that a record could contain no token, because there is no data
    /// to decrypt for example. In this case, a null pointer is returned
    /// and serialized though RPCs could rely on the null() static method
    /// for transmitting such a value over the network.
    class Record:
      public elle::Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a null record i.e a record which represent an non-applicable
      /// value.
      static
      Record const&
      null();

      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      enum class Type
      {
        null,
        valid
      };

      /*------.
      | Types |
      `------*/
    public:
      typedef Subject Symbol;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Record();
      /// Construct a record with a subject/permissions. Note that the token
      /// can be left meaning that the record will not allow the referenced
      /// user to retrieve a secret.
      Record(Subject const& subject,
             Permissions permissions,
             Token const& token = Token::null());
      /// Same as above but based on a pointer token.
      Record(Subject const& subject,
             Permissions permissinos,
             Token const* token);
      Record(Record const& other);
      ~Record();
    private:
      Record(Type const type);

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Record const& other) const;
      ELLE_OPERATOR_NEQ(Record);
      /// Do not allow record assignment: use the copy constructor instead.
      ELLE_OPERATOR_NO_ASSIGNMENT(Record);

      /*--------.
      | Methods |
      `--------*/
    public:
      // Return the record's subject.
      Subject const&
      subject() const;
      // Return the record's permissions.
      Permissions
      permissions() const;
      // Update the record's permissions.
      void
      permissions(Permissions permissions);
      // Return the record's token though it could be null.
      Token const*
      token() const;
      // Update the record's token.
      void
      token(Token const& token);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Record);
      // rangeable
      virtual
      Subject const&
      symbol();

      /*-----------.
      | Structures |
      `-----------*/
    public:
      struct Valid
      {
        // construction
      public:
        Valid();
        Valid(Subject const& subject,
              Permissions permissions,
              Token const& token);
        ~Valid();

        // methods
      public:
        /// Update the token.
        void
        token(Token const& token);

      public:
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Record::Valid);

        // attributes
      private:
        ELLE_ATTRIBUTE_R(Subject, subject);
        ELLE_ATTRIBUTE_RW(Permissions, permissions);
        /// The token is optional since the referenced user may
        /// not have access to the object's content i.e no read
        /// permission.
        ELLE_ATTRIBUTE_R(Token*, token);
      };

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type, type);
      ELLE_ATTRIBUTE(Valid*, valid);
    };

  }
}

# include <nucleus/neutron/Record.hxx>

#endif
