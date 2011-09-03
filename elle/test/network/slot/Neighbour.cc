//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/slot/Neighbour.cc
//
// created       julien quintard   [wed mar 17 11:23:38 2010]
// updated       julien quintard   [fri sep  2 20:30:56 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/slot/Neighbour.hh>

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Neighbour::Create(Node*			node,
					  const Point&		point,
					  const String&		name)
    {
      enter();

      // assign the attributes.
      this->node = node;

      this->point = point;
      this->name = name;

      // create the timer.
      if (this->timer.Create(Timer::ModeSingle) == StatusError)
	escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (this->timer.signal.timeout.Subscribe(
	    Callback<>::Infer(&Neighbour::Discard, this)) == StatusError)
	escape("unable to subscribe to the signal");

      // start the timer.
      if (this->timer.Start(Neighbour::Timeout) == StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Neighbour::Update(const String&		name)
    {
      enter();

      // update the name.
      this->name = name;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Neighbour::Refresh()
    {
      enter();

      // re-set the timer.
      if (this->timer.Restart(Neighbour::Timeout) == StatusError)
	escape("unable to restart the timer");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a neighbour.
    ///
    Status		Neighbour::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the point and name.
      if (archive.Serialize(this->point,
			    this->name) == StatusError)
	escape("unable to serialize the neighbour attributes");

      leave();
    }

    ///
    /// this method extracts a neighbour.
    ///
    Status		Neighbour::Extract(Archive&		archive)
    {
      enter();

      // extract the point and name.
      if (archive.Extract(this->point,
			  this->name) == StatusError)
	escape("unable to extract the neighbour attributes");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a neighbour entry.
    ///
    Status		Neighbour::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Neighbour]" << std::endl;

      // dump the point.
      if (this->point.Dump(margin + 2) == StatusError)
	escape("unable to dump the point");

      // dump the name.
      std::cout << alignment << Dumpable::Shift
		<< "[Name] " << this->name << std::endl;

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Neighbour::Discard()
    {
      enter();

      // discard the current neighbour as it has not been refreshed in time.
      if (this->node->table.Remove(this->point) == StatusError)
	escape("unable to remove the current neighbour");

      leave();
    }

  }
}
