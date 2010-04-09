//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Access.cc
//
// created       julien quintard   [wed mar 11 16:55:36 2009]
// updated       julien quintard   [wed apr  7 19:30:58 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Access.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds the given record to the ACL.
    ///
    Status		Access::Add(Record*			record)
    {
      enter();

      // add the record in the range.
      if (this->range.Add(record) == StatusError)
	escape("unable to add the record in the range");

      // set the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method returns true if the subject exists in the access
    /// block.
    Status		Access::Exist(const Subject&		subject)
    {
      enter();

      // look in the range.
      if (this->range.Exist(subject) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method returns the record corresponding to the given subject.
    ///
    Status		Access::Lookup(const Subject&		subject,
				       Record*&			record)
    {
      enter();

      // look in the range.
      if (this->range.Lookup(subject, record) == StatusError)
	escape("unable to retrieve the record");

      leave();
    }

    ///
    /// this method returns a range representing a subset of the access
    /// control list delimited by the given index and size.
    ///
    Status		Access::Consult(const Index&		index,
					const Size&		size,
					Range&			range) const
    {
      Range::Scoutor	scoutor;
      Index		i;

      enter();

      for (scoutor = this->range.container.begin(), i = 0;
	   scoutor != this->range.container.end();
	   scoutor++, i++)
	{
	  Record*	record = *scoutor;

	  // if this record lies in the selected range [index, index + size[
	  if ((i >= index) && (i < (index + size)))
	    {
	      Record*	r;

	      enter(instance(r));

	      // duplicate the record.
	      r = new Record(record->subject,
			     record->permissions,
			     record->token);

	      // add the new record to the return range.
	      if (range.Add(r) == StatusError)
		escape("unable to add the record to the given range");

	      // stop tracking _r_.
	      waive(r);

	      release();
	    }
	}

      leave();
    }

    ///
    /// this method removes the given record.
    ///
    Status		Access::Remove(const Subject&		subject)
    {
      enter();

      // remove the record from the range.
      if (this->range.Remove(subject) == StatusError)
	escape("unable to remove the record");

      // set the object as dirty.
      this->state = StateDirty;

      leave();
    }


    ///
    /// this method updates the records with the given secret key by
    /// encrypted the given key with every subject's public key.
    ///
    Status		Access::Upgrade(const SecretKey&	key)
    {
      Range::Iterator	iterator;

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

		PublicKey*	K = record->subject.user;

		// update the token.
		if (record->token.Update(key, *K) == StatusError)
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

		PublicKey*	K;
		hole::Address*	address;
		Token		token;

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
    /// this method returns the size of the access control list.
    ///
    Status		Access::Capacity(Size&			size) const
    {
      enter();

      // look at the size of the range.
      if (this->range.Capacity(size) == StatusError)
	escape("unable to retrieve the range size");

      leave();
    }

    ///
    /// this method returns the index location of the given subject.
    ///
    Status		Access::Locate(const Subject&		subject,
				       Index&			index)
    {
      enter();

      // look in the range.
      if (this->range.Locate(subject, index) != StatusTrue)
	escape("unable to locate the given subject");

      leave();
    }

    ///
    /// this method computes a hash of the (subject, permissions) tuples
    /// composing the access object.
    ///
    /// this is required by the object class for access control purposes.
    ///
    Status		Access::Fingerprint(Digest&		digest) const
    {
      Range::Scoutor	scoutor;
      Archive		archive;

      enter();

      // create the archive.
      if (archive.Create() == StatusError)
	escape("unable to create an archive");

      // go through the range and serialize every tuple (subject, permissions).
      for (scoutor = this->range.container.begin();
	   scoutor != this->range.container.end();
	   scoutor++)
	{
	  Record*	record = *scoutor;

	  // serialize the subject and permissions.
	  if (archive.Serialize(record->subject,
				record->permissions) == StatusError)
	    escape("unable to serialize the (subject, permissions) tuple");
	}

      // hash the archive.
      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the set of archived tuples");

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Access);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the access.
    ///
    Status		Access::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Access]" << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the range.
      if (this->range.Dump(margin + 2) == StatusError)
	escape("unable to dump the range");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the access object.
    ///
    Status		Access::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the range.
      if (archive.Serialize(this->range) == StatusError)
	escape("unable to serialize the range");

      leave();
    }

    ///
    /// this method extracts the access object.
    ///
    Status		Access::Extract(Archive&		archive)
    {
      enter();

      // extract the range.
      if (archive.Extract(this->range) == StatusError)
	escape("unable to extract the range");

      leave();
    }

  }
}
