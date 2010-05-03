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
// updated       julien quintard   [mon may  3 19:46:54 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Context.hh>

#include <etoile/user/User.hh>

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
    elle::Status	Context::Create()
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check that the current client is an application.
      if (user->type != user::User::TypeApplication)
	escape("unable to create a context for non-applications");

      // set the application.
      this->application = user->application;

      // set the fiber so it can be destroyed should the application die.
      this->fiber = elle::Fiber::Current;

      // generate an identifier.
      if (this->identifier.Generate() == elle::StatusError)
	escape("unable to generate an identifier");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a context.
    ///
    elle::Status	Context::Dump(const elle::Natural32	margin) const
    {
      elle::String		alignment(margin, ' ');
      journal::Bucket::Scoutor	scoutor;

      enter();

      // dump the parent context.
      std::cout << alignment << "[Context]" << std::endl;

      // dump the format.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Format] " << std::hex << (elle::Natural32)this->format
		<< std::endl;

      // dump the type.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Type] " << std::hex << (elle::Natural32)this->type
		<< std::endl;

      // dump the application.
      if (this->application->Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the application");

      // dump the fiber.
      if (this->fiber->Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the fiber");

      // dump the identifier.
      if (this->identifier.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the identifier");

      // dump the bucket
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Bucket]" << std::endl;

      // go through the bucket.
      for (scoutor = this->bucket.container.begin();
	   scoutor != this->bucket.container.end();
	   scoutor++)
	{
	  journal::Item*	item = *scoutor;

	  // dump the item.
	  if (item->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the item");
	}

      leave();
    }

  }
}
