#include <elle/standalone/Morgue.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Scheduler.hh>

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
      // allocate the morgue.
      Morgue::Current = new Morgue;

      return StatusOk;
    }

    ///
    /// this method cleans the morgue.
    ///
    Status              Morgue::Clean()
    {
      // delete the morgue.
      if (Morgue::Current != NULL)
        delete Morgue::Current;

      return StatusOk;
    }

    ///
    /// this method returns the current morgue instance.
    ///
    Status              Morgue::Instance(Morgue*&               morgue)
    {
      // verify the morgue's presence.
      if (Morgue::Current == NULL)
        return StatusFalse;

      morgue = Morgue::Current;

      return StatusTrue;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    void Morgue::GraveDigger()
    {
      while (true)
        {
          elle::concurrency::scheduler().current()
            ->wait(this->_corpses_available);
          this->Bury();
        }
    }

    ///
    /// default constructor.
    ///
    Morgue::Morgue()
      : _corpses_available()
      , _deleter(elle::concurrency::scheduler(), "Grave digger",
                 boost::bind(&Morgue::GraveDigger, this))
    {}

    ///
    /// destructor.
    ///
    Morgue::~Morgue()
    {
      // Bury the pending instances.
      this->Bury();
      // Stop the gravedigger.
      _deleter.terminate_now();
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

      return StatusOk;
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

      return StatusOk;
    }

  }
}
