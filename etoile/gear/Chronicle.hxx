//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Chronicle.hxx
//
// created       julien quintard   [fri jun  3 13:21:44 2011]
// updated       julien quintard   [fri jun  3 13:27:00 2011]
//

#ifndef ETOILE_GEAR_CHRONICLE_HXX
#define ETOILE_GEAR_CHRONICLE_HXX

namespace etoile
{
  namespace gear
  {

//
// ---------- template methods ------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename... T>
    elle::Status	Chronicle::Record(const Operation	operation,
					  const T&...		objects)
    {
      Transaction<T...>*	transaction;

      enter();

      // allocate a new transaction.
      transaction = new Transaction<T...>(operation,
					  objects);

      // create the transaction.
      if (transaction->Create() == elle::StatusError)
	escape("unable to create the transaction");

      // add the transaction to the diary.
      // XXX

      leave();
    }

  }
}

#endif
