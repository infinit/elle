//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Entrance.hxx
//
// created       julien quintard   [wed mar 24 23:43:50 2010]
// updated       julien quintard   [thu mar 25 00:27:51 2010]
//

#ifndef ELLE_MISC_ENTRANCE_HXX
#define ELLE_MISC_ENTRANCE_HXX

namespace elle
{
  namespace misc
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method triggers the entrance implementation specific
    /// Call() method.
    ///
    template <typename... T>
    Status		Entrance::Trigger(T*...			arguments)
    {
      enter();

      // check if the routine is present.
      if (this->routine == NULL)
	escape("unable to call an undefined entrance");

      // call the routine.
      if (this->routine->Trigger(arguments...) == StatusError)
	escape("an error occured in the routine");

      leave();
    }

  }
}

#endif
