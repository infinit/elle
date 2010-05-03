//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Record.cc
//
// created       julien quintard   [thu apr  1 22:00:03 2010]
// updated       julien quintard   [mon may  3 23:03:05 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Record.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines an unexisting record.
    ///
    const Record		Record::Null;

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
    Record::Record(const Subject&				subject,
		   const Permissions&				permissions,
		   const Token&					token):
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
    elle::Status	Record::Update(const Subject&		subject,
				       const Permissions&	permissions,
				       const elle::SecretKey&	key)
    {
      enter();

      // set the subject.
      this->subject = subject;

      // set the permissions.
      this->permissions = permissions;

      // then, if the subject has the read permission, create a token.
      if ((this->permissions & PermissionRead) != 0)
	{
	  // create the token with the public key of the user or the
	  // group owner, depending on the subject.
	  switch (subject.type)
	    {
	    case Subject::TypeUser:
	      {
		// create the token with the user's public key.
		if (this->token.Update(key, *subject.user) == elle::StatusError)
		  escape("unable to create the token");

		break;
	      }
	    case Subject::TypeGroup:
	      {
		printf("[XXX] Record::Create() NYI\n");

		// XXX retrieve group block, pick owner public key, and
		// update token

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

      leave();
    }

    ///
    /// this method creates/updates a record with a ready-to-be-used token.
    ///
    elle::Status	Record::Update(const Subject&		subject,
				       const Permissions&	permissions,
				       const Token&		token)
    {
      enter();

      // set the subject.
      this->subject = subject;

      // set the permissions.
      this->permissions = permissions;

      // set the token.
      this->token = token;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Record::operator==(const Record&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attributes.
      if ((this->subject != element.subject) ||
	  (this->permissions != element.permissions) ||
	  (this->token != element.token))
	false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Record, _(), _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a record.
    ///
    elle::Status	Record::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Record]" << std::endl;

      // dump the subject.
      if (this->subject.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the subject");

      // dump the permissions.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Permissions] " << std::dec
		<< (elle::Natural32)this->permissions << std::endl;

      // dump the token.
      if (this->token.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the token");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the record object.
    ///
    elle::Status	Record::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->subject,
			    this->permissions,
			    this->token) == elle::StatusError)
	escape("unable to serialize the record");

      leave();
    }

    ///
    /// this method extracts the record object.
    ///
    elle::Status	Record::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->subject,
			  this->permissions,
			  this->token) == elle::StatusError)
	escape("unable to extract the record");

      leave();
    }

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of a record i.e the subject.
    ///
    Subject&		Record::Symbol()
    {
      return (this->subject);
    }

  }
}
