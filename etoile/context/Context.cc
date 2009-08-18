//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Context.cc
//
// created       julien quintard   [mon aug 17 12:14:42 2009]
// updated       julien quintard   [mon aug 17 12:26:19 2009]
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
    /// the constructor.
    ///
    Context::Context():
      component(core::ComponentUnknown)
    {
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the context.
    ///
    Context&		Context::operator=(const Context&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // recycle the context.
      if (this->Recycle<Context>(&element) == StatusError)
	yield("unable to recycle the context", *this);

      return (*this);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Context::operator==(const Context&	element) const
    {
      return (this->component == element.component);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Context::operator!=(const Context&	element) const
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a context object.
    ///
    Status		Context::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Context]" << std::endl;

      std::cout << alignment << shift << "[Component] "
		<< this->component << std::endl;

      leave();
    }

  }
}
