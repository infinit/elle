//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Context.hxx
//
// created       julien quintard   [fri jun 24 12:25:31 2011]
// updated       julien quintard   [fri jun 24 13:17:30 2011]
//

#ifndef ETOILE_GEAR_CONTEXT_HXX
#define ETOILE_GEAR_CONTEXT_HXX

namespace etoile
{
  namespace gear
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the context by checking that it complies
    /// with the required type.
    ///
    template <typename T>
    elle::Status	Context::Cast(T*&			context)
    {
      enter();

      // dynamic cast the context into the given type.
      if ((context = dynamic_cast<T*>(this)) == NULL)
	escape("unable to cast the context into the given type");

      leave();
    }

  }
}

#endif
