//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Access.cc
//
// created       julien quintard   [wed mar 11 16:55:36 2009]
// updated       julien quintard   [wed mar  3 16:10:14 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Access.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Access::Add(const Subject&		subject,
				    const Permissions&		permissions,
				    const Code&			token)
    {
      Access::Entry*	entry;

      enter(instance(entry));

      // look for an existing entry.
      if (this->Search(subject) == StatusTrue)
	escape("another entry with the same name seems to already exist");

      // allocate a new entry.
      entry = new Access::Entry;

      // build the entry.
      entry->subject = subject;
      entry->permissions = permissions;
      entry->token = token;

      // add the entry to the catalog entries.
      this->entries.push_back(entry);

      // stop tracking the new element since it has been pushed in the
      // container.
      waive(entry);

      // mark the catalog as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Update(const Subject&		subject,
				       const Permissions&	permissions,
				       const Code&		token)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Remove(const Subject&		subject)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Retrieve(const Subject&		subject,
					 Permissions&		permissions)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Retrieve(const Subject&		subject,
					 const PrivateKey&	k,
					 Code&			token)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Access::Search(const Subject&		subject,
				       Access::Iterator*	pointer)
    {
      Access::Iterator		iterator;

      enter();

      // go through the entries.
      for (iterator = this->entries.begin();
	   iterator != this->entries.end();
	   iterator++)
	{
	  Access::Entry*	entry = *iterator;

	  // check if this entry represents the given subject.
	  if (subject == entry->subject)
	    {
	      // return an iterator if requested i.e different from NULL.
	      if (pointer != NULL)
		*pointer = iterator;

	      true();
	    }
	}

      false();
    }

    ///
    /// XXX
    ///
    Status		Access::Size(Offset&			size) const
    {
      enter();

      // set the size.
      size = this->entries.size();

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the access.
    ///
    Status		Access::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      // XXX

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

      // XXX

      leave();
    }

    ///
    /// this method extracts the access object.
    ///
    Status		Access::Extract(Archive&		archive)
    {
      enter();

      // XXX

      leave();
    }

  }
}
