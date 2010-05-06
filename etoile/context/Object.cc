//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Object.cc
//
// created       julien quintard   [mon aug 17 12:19:13 2009]
// updated       julien quintard   [tue may  4 10:19:21 2010]
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
    Object::Object():
      Context::Context(FormatObject),

      object(NULL),
      access(NULL),
      rights(NULL),
      author(NULL)
    {
    }

    ///
    /// the type-specific constructor.
    ///
    Object::Object(const Format&				format):
      Context::Context(format),

      object(NULL),
      access(NULL),
      rights(NULL),
      author(NULL)
    {
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
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a context.
    ///
    elle::Status	Object::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Context] Object" << std::endl;

      // dump the parent context.
      if (Context::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the parent context");

      // dump the route.
      if (this->route.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the route");

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the address");

      // dump the object.
      if (this->object != NULL)
	if (this->object->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the object");

      // dump the access.
      if (this->access != NULL)
	if (this->access->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the access");

      // dump the rights.
      if (this->rights != NULL)
	if (this->rights->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the rights");

      // dump the author.
      if (this->author != NULL)
	if (this->author->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the author");

      leave();
    }

  }
}
