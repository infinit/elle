//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar 17 11:23:38 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/udp/Neighbour.hh>

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a neighbour.
    ///
    Status              Neighbour::Create(Node*                 node,
                                          const Locus&          locus,
                                          const String&         name)
    {
      // assign the attributes.
      this->node = node;

      this->locus = locus;
      this->name = name;

      // create the timer.
      if (this->timer.Create(Timer::ModeSingle) == Status::Error)
        escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (this->timer.signal.timeout.Subscribe(
            Callback<>::Infer(&Neighbour::Discard, this)) == Status::Error)
        escape("unable to subscribe to the signal");

      // start the timer.
      if (this->timer.Start(Neighbour::Timeout) == Status::Error)
        escape("unable to start the timer");

      return Status::Ok;
    }

    ///
    /// this method updates the neighbour's name.
    ///
    Status              Neighbour::Update(const String&         name)
    {
      // update the name.
      this->name = name;

      return Status::Ok;
    }

    ///
    /// this method refreshes the neighbour.
    ///
    Status              Neighbour::Refresh()
    {
      // re-set the timer.
      if (this->timer.Restart(Neighbour::Timeout) == Status::Error)
        escape("unable to restart the timer");

      return Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a neighbour.
    ///
    Status              Neighbour::Serialize(Archive&           archive) const
    {
      // serialize the locus and name.
      if (archive.Serialize(this->locus,
                            this->name) == Status::Error)
        escape("unable to serialize the neighbour attributes");

      return Status::Ok;
    }

    ///
    /// this method extracts a neighbour.
    ///
    Status              Neighbour::Extract(Archive&             archive)
    {
      // extract the locus and name.
      if (archive.Extract(this->locus,
                          this->name) == Status::Error)
        escape("unable to extract the neighbour attributes");

      return Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a neighbour entry.
    ///
    Status              Neighbour::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Neighbour]" << std::endl;

      // dump the locus.
      if (this->locus.Dump(margin + 2) == Status::Error)
        escape("unable to dump the locus");

      // dump the name.
      std::cout << alignment << Dumpable::Shift
                << "[Name] " << this->name << std::endl;

      return Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method discards the neighbour.
    ///
    Status              Neighbour::Discard()
    {
      // discard the current neighbour as it has not been refreshed in time.
      if (this->node->table.Remove(this->locus) == Status::Error)
        escape("unable to remove the current neighbour");

      return Status::Ok;
    }

  }
}
