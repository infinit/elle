//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Object.cc
//
// created       julien quintard   [mon aug 17 12:19:13 2009]
// updated       julien quintard   [fri jun  3 11:46:00 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Object.hh>
#include <etoile/gear/Nature.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    Object::Object():
      Context(NatureObject),

      object(NULL),
      access(NULL)
    {
    }

    ///
    /// the type-specific constructor.
    ///
    Object::Object(const Nature&				nature):
      Context(nature),

      object(NULL),
      access(NULL)
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

      std::cout << alignment << "[Object]" << std::endl;

      // dump the parent context.
      if (Context::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the parent context");

      // dump the object.
      if (this->object != NULL)
	{
	  if (this->object->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the object");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Object] " << none << std::endl;
	}

      // dump the access.
      if (this->access != NULL)
	{
	  if (this->access->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the access");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Access] " << none << std::endl;
	}

      leave();
    }

  }
}
