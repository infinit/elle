//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Scope.cc
//
// created       julien quintard   [wed jun 15 13:09:29 2011]
// updated       julien quintard   [thu jun 23 16:22:20 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Scope.hh>
#include <etoile/gear/Gear.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Scope::Scope(const Nature					nature):
      context(NULL),
      chronicle(NULL)
    {
      // create the context according to the nature.
      switch (nature)
	{
	case NatureObject:
	  {
	    // allocate the context.
	    this->context = new Object;

	    break;
	  }
	case NatureDirectory:
	  {
	    // allocate the context.
	    this->context = new Directory;

	    break;
	  }
	case NatureFile:
	  {
	    // allocate the context.
	    // XXX this->context = new File;

	    break;
	  }
	case NatureLink:
	  {
	    // allocate the context.
	    // XXX this->context = new Link;

	    break;
	  }
	}
    }

    ///
    /// destructor.
    ///
    Scope::~Scope()
    {
      // delete the context.
      if (this->context != NULL)
	delete this->context;

      // delete the chronicle.
      if (this->chronicle != NULL)
	delete this->chronicle;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method exports the scope so that Etoile can retrieve the scope
    /// given its identifier.
    ///
    elle::Status	Scope::Export()
    {
      enter();

      // check if this scope already has an identifier.
      if (this->identifier != Identifier::Null)
	escape("this scope seems to have been exported already");

      // generate an identifier.
      if (this->identifier.Generate() == elle::StatusError)
	escape("unable to generate the identifier");

      // add the scope to the gear container.
      if (Gear::Add(this->identifier, this) == elle::StatusError)
	escape("unable to add the scope to the gear container");

      leave();
    }

    ///
    /// this method performs the opposite function of Export() namely
    /// to render the scope unaccessible through its identifier.
    ///
    elle::Status	Scope::Import()
    {
      enter();

      // check if this scope has an identifier.
      if (this->identifier == Identifier::Null)
	escape("this scope does not seem to have an identifier");

      // remove the scope from the gear container.
      if (Gear::Remove(this->identifier) == elle::StatusError)
	escape("unable to remove the scope from the gear container");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Scope, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a scope.
    ///
    elle::Status	Scope::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Scope]" << std::endl;

      // dump the identifier.
      if (this->identifier.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the identifier");

      // dump the chemin.
      if (this->chemin.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the chemin");

      // dump the context, if present.
      if (this->context != NULL)
	{
	  if (this->context.Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the context");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Context] " << elle::none << std::endl;
	}

      // dump the chronicle, if present.
      if (this->chronicle != NULL)
	{
	  if (this->chronicle->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the chronicle");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Scope] " << elle::none << std::endl;
	}

      leave();
    }

  }
}

#endif
