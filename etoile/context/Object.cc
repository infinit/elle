//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Object.cc
//
// created       julien quintard   [mon aug 17 12:19:13 2009]
// updated       julien quintard   [sun mar 21 16:58:19 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    /// note that this constructor does *not* initialize the contents.
    ///
    Object::Object():
      object(NULL),
      access(NULL),
      rights(NULL),
      author(NULL)
    {
    }

    ///
    /// the copy constructor.
    ///
    Object::Object(const Object&				context)
    {
      // XXX \todo XXX
      printf("NOT IMPLEMENTED YET\n");
    }

    ///
    /// the destructor
    ///
    Object::~Object()
    {
      // release the object.
      if (this->object != NULL)
	delete this->object;

      // release the access.
      if (this->access != NULL)
	delete this->access;

      // release the rights.
      if (this->rights != NULL)
	delete this->rights;

      // release the author.
      if (this->author != NULL)
	delete this->author;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Object::Register(journal::Set::Container&	set)
    {
      enter();

      // if there is an object.
      if (this->object != NULL)
	set.push_front(this->object);

      // if there is an access.
      if (this->access != NULL)
	set.push_front(this->access);

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a context.
    ///
    Status		Object::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      // dump the parent context.
      std::cout << alignment << "[Object]" << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      // dump the object.
      if (this->object != NULL)
	if (this->object->Dump(margin + 2) == StatusError)
	  escape("unable to dump the object");

      // dump the access.
      if (this->access != NULL)
	if (this->access->Dump(margin + 2) == StatusError)
	  escape("unable to dump the access");

      // dump the rights.
      if (this->rights != NULL)
	if (this->rights->Dump(margin + 2) == StatusError)
	  escape("unable to dump the rights");

      // dump the author.
      if (this->author != NULL)
	if (this->author->Dump(margin + 2) == StatusError)
	  escape("unable to dump the author");

      leave();
    }

  }
}
