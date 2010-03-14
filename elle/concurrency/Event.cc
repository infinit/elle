//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Event.cc
//
// created       julien quintard   [sun mar 14 13:08:57 2010]
// updated       julien quintard   [sun mar 14 14:18:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Event.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method processes pending events.
    ///
    Status		Event::Process()
    {
      enter();

      while (true)
	{
	  // processEvents();

	  // sleep() a few ms
	}

      leave();
    }

    ///
    /// this method processes pending events until the given semaphore
    /// is acquired.
    ///
    Status		Event::Wait(Semaphore&			semaphore)
    {
      enter();

      while (true)
	{
	  // processEvents();

	  // if semaphore acquire OK, return.

	  // sleep() a few ms
	}

      /// XXX \todo cette fonction n'est pas parfaite car si elle est
      /// appellee recurisvement, la premiere fois en attente de l'event E1
      /// et une seconde fois en attente du l'event E2. si l'event E1
      /// arrive, la method sera toujours bloquee dans la boucle de processing
      /// en attente de E2.
      ///
      /// moyen de contrer ca avec des frames et de lancer  les frames
      /// mais du coup on doit manipuler la pile donc c'est dangereux.
      ///
      /// en revanche, peu de fonctions font du bloquant et ces fonctions
      /// sont sures d'avoir une reponse dans une delai tres court (connexion
      /// via un pipe etc.)
      ///
      /// XXX Frame

      leave();
    }

  }
}
