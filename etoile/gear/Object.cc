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
// updated       julien quintard   [fri jun 17 16:24:07 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Object.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/depot/Depot.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Object::Object(const Nature					nature):
      Context(nature),

      rights(NULL),
      author(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Object::~Object()
    {
      // delete the rights.
      if (this->rights != NULL)
	delete this->rights;

      // delete the author.
      if (this->author != NULL)
	delete this->author;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method fetches the object.
    ///
    elle::Status	Object::Load(const nucleus::Location&	location)
    {
      enter();

      // retrieve the object block.
      if (depot::Depot::Pull(location.address,
			     location.version,
			     this->object) == elle::StatusError)
	escape("unable to retrieve the object block");

      // if there is an attached access block.
      if (this->object.meta.access != nucleus::Address::Null)
	{
	  // retrieve the access block.
	  if (depot::Depot::Pull(this->object.meta.access,
				 nucleus::Version::Any,
				 this->access) == elle::StatusError)
	    escape("unable to load the access block");
	}
      else
	{
	  // otherwise the block is left empty, as new.
	}

      leave();
    }

    ///
    /// this method fills in the information instance.
    ///
    elle::Status	Object::Information(
			  miscellaneous::Information&		information)
    {
      enter();

      // generate the information based on the object.
      if (information.Create(this->object) == elle::StatusError)
	escape("unable to generate the information");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Object::Destroy()
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Object::Store()
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// this method determines the user's rights over this object
    /// by computing the role and permissions of the user along with the
    /// key for decrypting the data.
    ///
    elle::Status	Object::Rights()
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Object::Author()
    {
      enter();

      // XXX

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the object.
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
      if (this->object.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the object");

      // dump the access.
      if (this->access.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the access");

      leave();
    }

  }
}
