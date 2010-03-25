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
// updated       julien quintard   [thu mar 25 18:16:53 2010]
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
					  const Address&	address,
					  const String&		name)
    {
      Callback<>	discard(&Neighbour::Discard, this);

      enter();

      // assign the attributes.
      this->node = node;

      this->address = address;
      this->name = name;

      // create the timer.
      if (this->timer.Create(Timer::ModeSingle, discard) == StatusError)
	escape("unable to create the timer");

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

      // serialize the address and name.
      if (archive.Serialize(this->address,
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

      // extract the address and name.
      if (archive.Extract(this->address,
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

      // dump the address.
      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

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
      if (this->node->table.Remove(this->address) == StatusError)
	escape("unable to remove the current neighbour");

      leave();
    }

  }
}
