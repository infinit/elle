#include <nucleus/proton/Transcript.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Transcript::~Transcript()
    {
      // flush the container.
      this->Flush();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records the given address/block as needed to be
    /// pushed onto the storage layer.
    ///
    elle::Status        Transcript::Push(const Address&         address,
                                         const Block*           block)
    {
      // add the action to the transcript's container.
      this->container.push_back(new Action(address, block));

      return elle::Status::Ok;
    }

    ///
    /// this method records the given address as needed to be removed
    /// from the storage layer.
    ///
    elle::Status        Transcript::Wipe(const Address&         address)
    {
      // add the action to the transcript's container.
      this->container.push_back(new Action(address));

      return elle::Status::Ok;
    }

    ///
    /// this method clears the transcript from some previously registered
    /// actions i.e depending on the given type of action.
    ///
    elle::Status        Transcript::Clear(const Action::Type    type)
    {
      Transcript::Iterator      iterator;

      // for every action.
      for (iterator = this->container.begin();
           iterator != this->container.end();
           )
        {
          Action*      action = *iterator;

          // ignore actions mismatching the given type.
          if (action->type != type)
            {
              // increase the iterator.
              iterator++;

              continue;
            }

          // delete the action.
          delete action;

          // erase the entry.
          this->container.erase(iterator);

          // reset the iterator.
          iterator = this->container.begin();
        }

      return elle::Status::Ok;
    }

    ///
    /// this method flushes the transcript from the previously registered
    /// actions.
    ///
    elle::Status        Transcript::Flush()
    {
      Transcript::Iterator      iterator;

      // for every action.
      for (iterator = this->container.begin();
           iterator != this->container.end();
           iterator++)
        {
          Action*      action = *iterator;

          // delete the action.
          delete action;
        }

      // clear the container.
      this->container.clear();

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a transcript.
    ///
    elle::Status        Transcript::Dump(const elle::Natural32  margin) const
    {
      elle::String              alignment(margin, ' ');
      Transcript::Scoutor       scoutor;

      std::cout << alignment << "[Transcript]" << std::endl;

      // for every action.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          Action*      action = *scoutor;

          // dump the action.
          if (action->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the action");
        }

      return elle::Status::Ok;
    }

  }
}
