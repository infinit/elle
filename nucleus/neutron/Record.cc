#include <nucleus/neutron/Record.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines an unexisting record.
    ///
    const Record                Record::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// empty constructor.
    ///
    Record::Record():
      token(new Token)
    {
    }

    ///
    /// default constructor.
    ///
    Record::Record(Subject const& subject,
                   Permissions permissions,
                   Token const& token):
      subject(subject),
      permissions(permissions),
      token(new Token(token))
    {
    }

    Record::Record(Subject const& subject,
                   Permissions permissions,
                   elle::cryptography::SecretKey const& secret,
                   elle::cryptography::PublicKey const& K):
      subject(subject),
      permissions(permissions)
    {
      if (this->Update(subject, permissions, secret, K) == elle::Status::Error)
        throw Exception("unable to update the recor"); // XXX[useless later]
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates/updates a record.
    ///
    elle::Status
    Record::Update(Subject const& subject,
                   Permissions permissions,
                   elle::cryptography::SecretKey const& key,
                   elle::cryptography::PublicKey const& K)
    {
      Token token;

      // then, if the subject has the read permission, create a token.
      if ((this->permissions & PermissionRead) == PermissionRead)
        {
          // create the token with the given public key K.
          if (token.Update(K, key) == elle::Status::Error)
            escape("unable to create the token");
        }
      else
        {
          // reinitialize the token.
          token = Token::Null;
        }

      return (this->Update(subject, permissions, token));
    }

    ///
    /// this method creates/updates a record with a ready-to-be-used token.
    ///
    elle::Status
    Record::Update(Subject const& subject,
                   Permissions permissions,
                   Token const& token)
    {
      this->subject = subject;
      this->permissions = permissions;

      delete this->token;
      this->token = new Token(token);

      return elle::Status::Ok;
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Record::operator ==(Record const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the attributes.
      if ((this->subject != other.subject) ||
          (this->permissions != other.permissions) ||
          (*this->token != *other.token)) // XXX[WARNING: to improve as can be null]
        return false;

      return true;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a record.
    ///
    elle::Status        Record::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Record]" << std::endl;

      // dump the subject.
      if (this->subject.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the subject");

      // dump the permissions.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Permissions] " << std::dec
                << (int)this->permissions << std::endl;

      // dump the token.
      if (this->token->Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the token");

      return elle::Status::Ok;
    }

    ///
    /// this method returns the symbol of a record i.e the subject.
    ///
    Subject&
    Record::symbol()
    {
      return (this->subject);
    }

  }
}
