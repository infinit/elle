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
// updated       julien quintard   [tue apr  6 21:50:38 2010]
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
    Status		Record::Update(const Subject&		subject,
				       const Permissions&	permissions,
				       const SecretKey&		key)
    {
      enter();

      // set the subject.
      this->subject = subject;

      // set the permissions.
      this->permissions = permissions;

      // then, if the subject has the read permission, create a token.
      if (this->permissions & PermissionRead)
	{
	  // create the token with the public key of the user or the
	  // group owner, depending on the subject.
	  switch (subject.type)
	    {
	    case Subject::TypeUser:
	      {
		// create the token with the user's public key.
		if (this->token.Update(key, *subject.user) == StatusError)
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

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a record.
    ///
    Status		Record::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Record]" << std::endl;

      // dump the subject.
      if (this->subject.Dump(margin + 2) == StatusError)
	escape("unable to dump the subject");

      // dump the permissions.
      std::cout << alignment << Dumpable::Shift
		<< "[Permissions] " << std::dec
		<< (Natural32)this->permissions << std::endl;

      // dump the token.
      if (this->token.Dump(margin + 2) == StatusError)
	escape("unable to dump the token");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the record object.
    ///
    Status		Record::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->subject,
			    this->permissions,
			    this->token) == StatusError)
	escape("unable to serialize the record");

      leave();
    }

    ///
    /// this method extracts the record object.
    ///
    Status		Record::Extract(Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->subject,
			  this->permissions,
			  this->token) == StatusError)
	escape("unable to extract the record");

      leave();
    }

  }
}
