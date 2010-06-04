//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Access.cc
//
// created       julien quintard   [wed mar 11 16:55:36 2009]
// updated       julien quintard   [fri may 28 17:48:24 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Access.hh>

#include <etoile/hole/Address.hh>
#include <etoile/hole/Component.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Access::Access():
      ContentHashBlock(hole::ComponentAccess),

      state(StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds the given record to the ACL.
    ///
    elle::Status	Access::Add(Record*			record)
    {
      enter();

      // add the record in the range.
      if (this->range.Add(record) == elle::StatusError)
	escape("unable to add the record in the range");

      // set the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method tests if the given subject exists.
    ///
    elle::Status	Access::Exist(const Subject&		subject)
    {
      enter();

      // test.
      if (this->range.Exist(subject) == false)
	false();

      true();
    }

    ///
    /// this method returns the record corresponding to the given subject.
    ///
    elle::Status	Access::Lookup(const Subject&		subject,
				       Record*&			record)
    {
      enter();

      // look in the range.
      if (this->range.Lookup(subject, record) == elle::StatusError)
	escape("unable to retrieve the record");

      leave();
    }

    ///
    /// this method returns a range representing a subset of the access
    /// control list delimited by the given index and size.
    ///
    elle::Status	Access::Consult(const Index&		index,
					const Size&		size,
					Range<Record>&		range) const
    {
      Range<Record>::Scoutor	scoutor;
      Index			i;

      enter();

      // first detach the data from the range.
      if (range.Detach() == elle::StatusError)
	escape("unable to detach the data from the range");

      // go through the records.
      for (scoutor = this->range.container.begin(), i = 0;
	   scoutor != this->range.container.end();
	   scoutor++, i++)
	{
	  Record*	record = *scoutor;

	  // if this record lies in the selected range [index, index + size[
	  if ((i >= index) && (i < (index + size)))
	    {
	      // add the record to the range.
	      if (range.Add(record) == elle::StatusError)
		escape("unable to add the record to the given range");
	    }
	}

      leave();
    }

    ///
    /// this method updates the records with the given secret key by
    /// encrypted the given key with every subject's public key.
    ///
    elle::Status	Access::Upgrade(const elle::SecretKey&	key)
    {
      Range<Record>::Iterator	iterator;

      enter();

      // go through the range.
      for (iterator = this->range.container.begin();
	   iterator != this->range.container.end();
	   iterator++)
	{
	  Record*	record = *iterator;

	  // check if the subject has the proper permissions.
	  if (!(record->permissions & PermissionRead))
	    continue;

	  // depending on the subject's type.
	  switch (record->subject.type)
	    {
	    case Subject::TypeUser:
	      {
		//
		// if the subject is a user, encrypt the key with the
		// user's public key so that she will be the only one
		// capable of decrypting it.
		//

		elle::PublicKey*	K = record->subject.user;

		// update the token.
		if (record->token.Update(key, *K) == elle::StatusError)
		  escape("unable to update the token");

		break;
	      }
	    case Subject::TypeGroup:
	      {
		//
		// if the subject is a group, the key is made available
		// to the group's owner. this is especially useful in
		// order to increase the number of delegates available to
		// respond to vassal's requests.
		//

		elle::PublicKey*	K;
		hole::Address*		address;
		Token			token;

		// retrieve the group's block address.
		// XXX

		// retrieve the owner's public key.
		// XXX

		// encrypt the secret key.
		// XXX

		// assign the token to the entry.
		// XXX

		printf("[XXX] NIY\n");

		break;
	      }
	    case Subject::TypeUnknown:
	      {
		escape("the access block contains unknown entries");
	      }
	    }

	  // set the access block as being dirty.
	  this->state = StateDirty;
	}

      leave();
    }

    ///
    /// this method removes the given record.
    ///
    elle::Status	Access::Remove(const Subject&		subject)
    {
      enter();

      // remove the record from the range.
      if (this->range.Remove(subject) == elle::StatusError)
	escape("unable to remove the record");

      // set the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method returns the size of the access control list.
    ///
    elle::Status	Access::Capacity(Size&			size) const
    {
      enter();

      // look at the size of the range.
      if (this->range.Capacity(size) == elle::StatusError)
	escape("unable to retrieve the range size");

      leave();
    }

    ///
    /// this method returns the index location of the given subject.
    ///
    elle::Status	Access::Locate(const Subject&		subject,
				       Index&			index)
    {
      Range<Record>::Scoutor	scoutor;

      enter();

      // go through the range and serialize every tuple (subject, permissions).
      for (scoutor = this->range.container.begin(), index = 0;
	   scoutor != this->range.container.end();
	   scoutor++, index++)
	{
	  Record*	record = *scoutor;

	  // if found, stop.
	  if (record->subject == subject)
	    break;
	}

      leave();
    }

    ///
    /// this method computes a hash of the (subject, permissions) tuples
    /// composing the access object.
    ///
    /// this is required by the object class for access control purposes.
    ///
    elle::Status	Access::Fingerprint(elle::Digest&	digest) const
    {
      Range<Record>::Scoutor	scoutor;
      elle::Archive		archive;

      enter();

      // create the archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create an archive");

      // go through the range and serialize every tuple (subject, permissions).
      for (scoutor = this->range.container.begin();
	   scoutor != this->range.container.end();
	   scoutor++)
	{
	  Record*	record = *scoutor;

	  // serialize the subject and permissions.
	  if (archive.Serialize(record->subject,
				record->permissions) == elle::StatusError)
	    escape("unable to serialize the (subject, permissions) tuple");
	}

      // hash the archive.
      if (elle::OneWay::Hash(archive, digest) == elle::StatusError)
	escape("unable to hash the set of archived tuples");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Access, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the access.
    ///
    elle::Status	Access::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Access]" << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the range.
      if (this->range.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the range");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the access object.
    ///
    elle::Status	Access::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the range.
      if (archive.Serialize(this->range) == elle::StatusError)
	escape("unable to serialize the range");

      leave();
    }

    ///
    /// this method extracts the access object.
    ///
    elle::Status	Access::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the range.
      if (archive.Extract(this->range) == elle::StatusError)
	escape("unable to extract the range");

      leave();
    }

  }
}
