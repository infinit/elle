//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Context.cc
//
// created       julien quintard   [wed mar 31 02:37:45 2010]
// updated       julien quintard   [thu apr 22 16:13:13 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Context.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Context::Context(const Format&				format):
      application(NULL),
      fiber(NULL),
      type(Context::TypeInternal),
      format(format)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a context.
    ///
    Status		Context::Create()
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // check that the current client is an application.
      if (user->type != user::User::TypeApplication)
	escape("unable to create a context for non-applications");

      // set the application.
      this->application = user->application;

      // set the fiber so it can be destroyed should the application die.
      this->fiber = Fiber::Current;

      // generate an identifier.
      if (this->identifier.Generate() == StatusError)
	escape("unable to generate an identifier");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a context.
    ///
    Status		Context::Dump(const Natural32		margin) const
    {
      String			alignment(margin, ' ');
      journal::Bucket::Scoutor	scoutor;

      enter();

      // dump the parent context.
      std::cout << alignment << "[Context]" << std::endl;

      // dump the format.
      std::cout << alignment << Dumpable::Shift
		<< "[Format] " << std::hex << (Natural32)this->format
		<< std::endl;

      // dump the type.
      std::cout << alignment << Dumpable::Shift
		<< "[Type] " << std::hex << (Natural32)this->type
		<< std::endl;

      // dump the application.
      if (this->application->Dump(margin + 2) == StatusError)
	escape("unable to dump the application");

      // dump the fiber.
      if (this->fiber->Dump(margin + 2) == StatusError)
	escape("unable to dump the fiber");

      // dump the identifier.
      if (this->identifier.Dump(margin + 2) == StatusError)
	escape("unable to dump the identifier");

      // dump the bucket
      std::cout << alignment << Dumpable::Shift << "[Bucket]" << std::endl;

      // go through the bucket.
      for (scoutor = this->bucket.container.begin();
	   scoutor != this->bucket.container.end();
	   scoutor++)
	{
	  journal::Item*	item = *scoutor;

	  // dump the item.
	  if (item->Dump(margin + 4) == StatusError)
	    escape("unable to dump the item");
	}

      leave();
    }

  }
}
