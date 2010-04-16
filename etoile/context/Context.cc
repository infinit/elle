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
// updated       julien quintard   [fri apr 16 09:23:20 2010]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the contexts in use.
    ///
    Context::Container		Context::Contexts;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Context::Context(const Format&				format):
      format(format)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method identify a context.
    ///
    Status		Context::Identify(const Identifier&	identifier)
    {
      enter();

      // set the identifier.
      this->identifier = identifier;

      leave();
    }

  }
}
