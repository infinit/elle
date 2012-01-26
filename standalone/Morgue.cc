//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri aug 26 17:10:36 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Morgue.hh>

#include <elle/concurrency/Callback.hh>

namespace elle
{
  namespace standalone
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the address of the current morgue.
    ///
    Morgue*                             Morgue::Current = NULL;

    ///
    /// this constants defines the morgue's frequency in milliseconds i.e
    /// the delay between each session of deletions.
    ///
    const Natural32                     Morgue::Frequency = 1000;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the morgue system.
    ///
    Status              Morgue::Initialize()
    {
      enter();

      // allocate the morgue.
      Morgue::Current = new Morgue;

      leave();
    }

    ///
    /// this method cleans the morgue.
    ///
    Status              Morgue::Clean()
    {
      enter();

      // delete the morgue.
      if (Morgue::Current != NULL)
        delete Morgue::Current;

      leave();
    }

    ///
    /// this method returns the current morgue instance.
    ///
    Status              Morgue::Instance(Morgue*&               morgue)
    {
      enter();

      // verify the morgue's presence.
      if (Morgue::Current == NULL)
        false();

      morgue = Morgue::Current;

      true();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Morgue::Morgue():
      timer(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Morgue::~Morgue()
    {
      // if present, delete the timer.
      if (this->timer != NULL)
        delete this->timer;

      // bury the pending instances.
      this->Bury();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered at a certain frequency and is
    /// responsible for deleting the pending instances.
    ///
    Status              Morgue::Bury()
    {
      enter();

      // as long as instances remain.
      while (this->container.empty() == false)
        {
          Meta*         instance;

          // retrieve the first instance.
          instance = this->container.front();

          // delete it.
          delete instance;

          // remove it from the container.
          this->container.pop_front();
        }

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the morgue's content.
    ///
    Status              Morgue::Dump(Natural32          margin) const
    {
      String            alignment(margin, ' ');
      Morgue::Scoutor   scoutor;

      enter();

      // display the name.
      std::cout << alignment << "[Morgue]" << std::endl;

      // go through the instances.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          Meta*         instance = *scoutor;

          // dump the instance's address.
          std::cout << alignment << Dumpable::Shift
                    << "[Instance] " << std::hex << instance << std::endl;
        }

      leave();
    }

  }
}
