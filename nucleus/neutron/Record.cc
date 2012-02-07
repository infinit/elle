//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu apr  1 22:00:03 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Record.hh>

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
    Record::Record()
    {
    }

    ///
    /// default constructor.
    ///
    Record::Record(const Subject&                               subject,
                   const Permissions&                           permissions,
                   const Token&                                 token):
      subject(subject),
      permissions(permissions),
      token(token)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates/updates a record.
    ///
    elle::Status        Record::Update(const Subject&           subject,
                                       const Permissions&       permissions,
                                       const elle::SecretKey&   key)
    {
      ;

      // set the subject.
      this->subject = subject;

      // set the permissions.
      this->permissions = permissions;

      // then, if the subject has the read permission, create a token.
      if ((this->permissions & PermissionRead) == PermissionRead)
        {
          // create the token with the public key of the user or the
          // group owner, depending on the subject.
          switch (subject.type)
            {
            case Subject::TypeUser:
              {
                // create the token with the user's public key.
                if (this->token.Update(key,
                                       *subject.user) == elle::StatusError)
                  escape("unable to create the token");

                break;
              }
            case Subject::TypeGroup:
              {
                // XXX to implement.

                break;
              }
            default:
              {
                escape("unknown subject type");
              }
            }
        }
      else
        {
          // reinitialize the token.
          this->token = Token::Null;
        }

      return elle::StatusOk;
    }

    ///
    /// this method creates/updates a record with a ready-to-be-used token.
    ///
    elle::Status        Record::Update(const Subject&           subject,
                                       const Permissions&       permissions,
                                       const Token&             token)
    {
      ;

      // set the subject.
      this->subject = subject;

      // set the permissions.
      this->permissions = permissions;

      // set the token.
      this->token = token;

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Record::operator==(const Record&        element) const
    {
      ;

      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::StatusTrue;

      // compare the attributes.
      if ((this->subject != element.subject) ||
          (this->permissions != element.permissions) ||
          (this->token != element.token))
        return elle::StatusFalse;

      return elle::StatusTrue;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Record, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a record.
    ///
    elle::Status        Record::Dump(elle::Natural32            margin) const
    {
      elle::String      alignment(margin, ' ');

      ;

      std::cout << alignment << "[Record]" << std::endl;

      // dump the subject.
      if (this->subject.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the subject");

      // dump the permissions.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Permissions] " << std::dec
                << this->permissions << std::endl;

      // dump the token.
      if (this->token.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the token");

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the record object.
    ///
    elle::Status        Record::Serialize(elle::Archive&        archive) const
    {
      ;

      // serialize the attributes.
      if (archive.Serialize(this->subject,
                            this->permissions,
                            this->token) == elle::StatusError)
        escape("unable to serialize the record");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the record object.
    ///
    elle::Status        Record::Extract(elle::Archive&          archive)
    {
      ;

      // extract the attributes.
      if (archive.Extract(this->subject,
                          this->permissions,
                          this->token) == elle::StatusError)
        escape("unable to extract the record");

      return elle::StatusOk;
    }

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of a record i.e the subject.
    ///
    Subject&            Record::Symbol()
    {
      return (this->subject);
    }

  }
}
